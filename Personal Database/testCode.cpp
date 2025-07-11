#include "Tokenizer.h"
#include "abstractSyntaxTree.h"
#include "Manager.h"
#include "Parser.h"
#include <iostream>

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
        if (entry.path().extension() == ".bin")
        {
            std::ifstream binFile(entry.path(), std::ios::binary);
            if (!binFile)
            {
                std::cerr << "Failed to open binary table file: " << entry.path() << "\n";
                continue;
            }

            // Read schema
            size_t colCount;
            binFile.read(reinterpret_cast<char*>(&colCount), sizeof(colCount));
            std::vector<std::string> colNames;
            std::vector<int> colTypes;
            for (size_t i = 0; i < colCount; ++i)
            {
                size_t nameLen;
                binFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
                std::string name(nameLen, '\0');
                binFile.read(&name[0], nameLen);
                int typeInt;
                binFile.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
                colNames.push_back(name);
                colTypes.push_back(typeInt);
            }

            // Prepare CSV file
            fs::path csvPath = entry.path().parent_path() / (entry.path().stem().string() + ".csv");
            std::ofstream csvFile(csvPath);
            if (!csvFile)
            {
                std::cerr << "Failed to create CSV file: " << csvPath << "\n";
                continue;
            }

            // Write header
            for (size_t i = 0; i < colCount; ++i)
            {
                csvFile << colNames[i];
                if (i < colCount - 1) csvFile << ",";
            }
            csvFile << "\n";

            // Write rows
            while (binFile.peek() != EOF)
            {
                std::vector<std::string> row;
                for (size_t i = 0; i < colCount; ++i)
                {
                    if (colTypes[i] == 0) // INT
                    {
                        int val;
                        binFile.read(reinterpret_cast<char*>(&val), sizeof(val));
                        if (binFile.eof()) break;
                        row.push_back(std::to_string(val));
                    }
                    else if (colTypes[i] == 1) // FLOAT
                    {
                        float val;
                        binFile.read(reinterpret_cast<char*>(&val), sizeof(val));
                        if (binFile.eof()) break;
                        row.push_back(std::to_string(val));
                    }
                    else if (colTypes[i] == 2) // TEXT
                    {
                        size_t len;
                        binFile.read(reinterpret_cast<char*>(&len), sizeof(len));
                        if (binFile.eof()) break;
                        std::string txt(len, '\0');
                        binFile.read(&txt[0], len);
                        if (binFile.eof()) break;
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
            binFile.close();
            csvFile.close();
            std::cout << "Exported table to CSV: " << csvPath << "\n";
        }
    }
}