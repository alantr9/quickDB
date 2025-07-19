#include "Tokenizer.h"
#include "abstractSyntaxTree.h"
#include "Manager.h"
#include "Parser.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>


void exportAllTablesToCSV(const std::string& dbName)
{
    namespace fs = std::filesystem;
    fs::path dbFolder = fs::path("./databases") / dbName;
    if (!fs::exists(dbFolder) || !fs::is_directory(dbFolder))
    {
        std::cerr << "Database folder not found: " << dbFolder << "\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(dbFolder))
    {
        auto path = entry.path();
        
        if (path.extension() == ".bin" && path.stem().string().size() > 6 &&
            path.stem().string().substr(path.stem().string().size() - 6) == "Schema")
        {
            std::string tableName = path.stem().string().substr(0, path.stem().string().size() - 6);
            fs::path schemaPath = dbFolder / (tableName + "Schema.bin");
            fs::path dataPath = dbFolder / (tableName + ".bin");
            fs::path csvPath = dbFolder / (tableName + ".csv");

            
            std::ifstream schemaFile(schemaPath, std::ios::binary);
            if (!schemaFile)
            {
                std::cerr << "Failed to open schema file: " << schemaPath << "\n";
                continue;
            }

            size_t colCount{ 0 };
            schemaFile.read(reinterpret_cast<char*>(&colCount), sizeof(colCount));
            std::vector<std::string> colNames;
            std::vector<int> colTypes;
            for (size_t i = 0; i < colCount; ++i)
            {
                size_t nameLen{ 0 };
                schemaFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
                std::string name(nameLen, '\0');
                schemaFile.read(&name[0], nameLen);
                int typeInt{ 0 };
                schemaFile.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
                colNames.push_back(name);
                colTypes.push_back(typeInt);
            }
            schemaFile.close();

            
            std::ofstream csvFile(csvPath);
            if (!csvFile)
            {
                std::cerr << "Failed to create CSV file: " << csvPath << "\n";
                continue;
            }

           
            for (size_t i = 0; i < colCount; ++i)
            {
                csvFile << colNames[i];
                if (i < colCount - 1) csvFile << ",";
            }
            csvFile << "\n";

            
            std::ifstream dataFile(dataPath, std::ios::binary);
            if (!dataFile)
            {
                std::cerr << "Failed to open data file: " << dataPath << "\n";
                csvFile.close();
                continue;
            }

            while (dataFile.peek() != EOF)
            {
                std::vector<std::string> row;
                for (size_t i = 0; i < colCount; ++i)
                {
                    if (colTypes[i] == 0) // INT
                    {
                        int val{ 0 };
                        dataFile.read(reinterpret_cast<char*>(&val), sizeof(val));
                        if (dataFile.eof()) break;
                        row.push_back(std::to_string(val));
                    }
                    else if (colTypes[i] == 1) // FLOAT
                    {
                        float val{ 0 };
                        dataFile.read(reinterpret_cast<char*>(&val), sizeof(val));
                        if (dataFile.eof()) break;
                        row.push_back(std::to_string(val));
                    }
                    else if (colTypes[i] == 2) // TEXT
                    {
                        size_t len{ 0 };
                        dataFile.read(reinterpret_cast<char*>(&len), sizeof(len));
                        if (dataFile.eof()) break;
                        std::string txt(len, '\0');
                        dataFile.read(&txt[0], len);
                        if (dataFile.eof()) break;
                        row.push_back(txt);
                    }
                }
                if (row.size() == colCount)
                {
                    for (size_t i = 0; i < colCount; ++i)
                    {
                        csvFile << row[i];
                        if (i < colCount - 1) csvFile << ",";
                    }
                    csvFile << "\n";
                }
            }
            dataFile.close();
            csvFile.close();
            std::cout << "Exported table to CSV: " << csvPath << "\n";
        }
    }
}

