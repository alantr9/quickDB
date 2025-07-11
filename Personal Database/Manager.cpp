#include <iostream>
#include <string>
#include "Manager.h"


namespace fs = std::filesystem;

manager::manager() :
    currentDB("") {};

bool manager::hasOpenDatabase() const 
{
    return !currentDB.empty();
}

std::string manager::getCurrentDatabase() const 
{
    return currentDB;
}

bool manager::doesDatabaseExists(const std::string& dbName) const
{
    std::string currName;
    std::ifstream searchFile("databaseNames.csv");

    while (std::getline(searchFile, currName))
    {
        if (currName == currentDB + ",")
        {
            return true;
            break;
        }
    }
    searchFile.close();
    return false;
}


void manager::dbLogger(std::string dbName) const
{

    if (doesDatabaseExists(dbName)) 
    {
        std::cout << "Database opened. \n";
        return;
	}
    else if (!doesDatabaseExists(dbName))
    {
        std::ofstream writeFile("databaseNames.csv", std::ios::app); // Append mode
		writeFile << dbName << ",\n";
        writeFile.close();
        std::cout << "Database created: " << dbName << "\n";

        try
        {
            std::filesystem::create_directory(currentDB);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            std::cerr << "Error creating database directory: " << e.what() << "\n";
            return;
        }

    }
    else
        std::cout << "Error opening file." << "\n";
}

void manager::execute(std::unique_ptr<SQLCommand> cmd) 
{
    if (!cmd) return;

    /*************************/
      /*  CREATE DB COMMAND */
    /*************************/

    if (cmd->type() == commandType::CREATE_DATABASE) 
    { 
        auto* cdb{ dynamic_cast<createDatabase*>(cmd.get()) };
		// Future: Check if database already opened or needs to be switched
        if (cdb && currentDB == cdb->dbName) 
        {
			std::cout << "Database already opened: " << cdb->dbName << "\n";
		}

        else if (cdb) 
        {
            currentDB = cdb->dbName;
            dbLogger(currentDB);
        }
        else
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }
    }

    /*************************/
    /*  CREATE TABLE COMMAND */
	/*************************/
    
    if (cmd->type() == commandType::CREATE_TABLE)
    {
        auto* cdb{ dynamic_cast<createTable*>(cmd.get()) };

        if (hasOpenDatabase() == false)
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            std::cout << currentDB << "\n";
            return;
        }

        // ??? Don't remove code will break
        fs::path dbFolder = fs::path("./databases") / fs::path(currentDB);
        std::error_code ec;
        fs::create_directories(dbFolder, ec);
        if (ec)
        {
            std::cerr << "Failed to create database directory: " << ec.message() << "\n";
            return;
        }
        

        fs::path binPath = dbFolder / (cdb->tableName + ".bin");
        if (fs::exists(binPath))
        {
            std::cerr << "Table Already Exists\n";
            return;
        }

        std::ofstream binFile(binPath, std::ios::binary);
        if (!binFile)
        {
            std::cerr << "Failed to create binary table file: " << binPath << "\n";
            return;
        }

        // Writing Column Info to BIN file

        /*
        FORMAT:

        columnCount - count, size
            
            column1 - length, size
            column1 - columnName tokenized to char
            column1 - datatype represented as typeInt

        */
        size_t colCount = cdb->columns.size();
        binFile.write(reinterpret_cast<const char*>(&colCount), sizeof(colCount));

        for (const auto& col : cdb->columns)
        {
            size_t nameLen = col.first.size();
            binFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            binFile.write(col.first.data(), nameLen);

            int typeInt;
            if (col.second == "INT") typeInt = 0;
            else if (col.second == "FLOAT") typeInt = 1;
            else if (col.second == "TEXT") typeInt = 2;
            else
            {
                std::cerr << "Unsupported column type: " << col.second << "\n";
                binFile.close();
                fs::remove(binPath); // File will be deleted if datatypes don't match
                return;
            }
            binFile.write(reinterpret_cast<const char*>(&typeInt), sizeof(typeInt));
        }

        binFile.close();
        std::cout << "Table created with binary file: " << binPath << "\n";
    }

   /*************************/
   /*  SELECT COMMAND */
   /*************************/
   
    // NOT FINISHED - IMPLEMENT WITH BTREES

    if (cmd->type() == commandType::SELECT) 
    {
        auto* cdb{ dynamic_cast<selectCommand*>(cmd.get()) };

        if (!hasOpenDatabase())
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }

        if (!cdb)
        {
            std::cout << "Invalid SELECT command.\n";
            return;
        }

        fs::path tablePath = fs::path("./databases") / currentDB / (cdb->tableName + ".csv");
        if (!fs::exists(tablePath))
        {
            std::cerr << "Table does not exist: " << cdb->tableName << "\n";
            return;
        }

        std::ifstream tableFile(tablePath);
        if (!tableFile)
        {
            std::cerr << "Failed to open table file: " << tablePath << "\n";
            return;
        }

        std::string line;
        while (std::getline(tableFile, line))
        {
            std::cout << line << "\n";
        }

        tableFile.close();
    }

    /*************************/
       /*  INSERT COMMAND */
   /*************************/

    if (cmd->type() == commandType::INSERT)
    {
        auto* cdb{ dynamic_cast<insertCommand*>(cmd.get()) };

        if (!hasOpenDatabase())
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }
        if (!cdb)
        {
            std::cerr << "Invalid INSERT command.\n";
            return;
        }

        fs::path binPath = fs::path("./databases") / currentDB / (cdb->tableName + ".bin");
        if (!fs::exists(binPath))
        {
            std::cerr << "Table not found: " << cdb->tableName << "\n";
            return;
        }

        std::ifstream binFileViewer(binPath, std::ios::binary);
        if (!binFileViewer)
        {
            std::cerr << "Failed to open binary table file: " << binPath << "\n";
            return;
        }

        size_t colCount{ 0 };
        binFileViewer.read(reinterpret_cast<char*>(&colCount), sizeof(colCount));
        std::vector<int> colTypes;

        for (size_t i = 0; i < colCount; ++i)
        {
            size_t nameLen{ 0 };
            //Reading binary files dont use const char*

            binFileViewer.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            binFileViewer.seekg(nameLen, std::ios::cur); // Moves nameLen bytes from current position

            int typeInt{ 0 };
            binFileViewer.read(reinterpret_cast<char*>(&typeInt), sizeof(typeInt));
            colTypes.push_back(typeInt);
        }
        binFileViewer.close();

        std::ofstream binFileWriter(binPath, std::ios::binary | std::ios::app);
        if (!binFileWriter)
        {
            std::cerr << "Failed to open binary table file for writing: " << binPath << "\n";
            return;
        }

        for (size_t g{ 0 }; g < colCount; ++g)
        {
            if (colTypes[g] == 0 && !std::all_of(cdb->values[g].begin(), cdb->values[g].end(), ::isdigit)) // INT
            {
                std::cerr << "Error: Value '" << cdb->values[g] << "' is not a valid INT.\n";
            }
            else if (colTypes[g] == 1)
            {
                try
                {
                    std::stof(cdb->values[g]);
                }
                catch (const std::invalid_argument&)
                {
                    std::cerr << "Error: Value '" << cdb->values[g] << "' is not a valid FLOAT.\n";
                    return;
                }
            }
        }

        for (size_t i = 0; i < colCount; ++i)
        {
            if (colTypes[i] == 0) // INT
            {
                int val{ std::stoi(cdb->values[i]) };
                binFileWriter.write(reinterpret_cast<const char*>(&val), sizeof(val));
            }

            else if (colTypes[i] == 1) // FLOAT
            {
                float val{ std::stof(cdb->values[i]) };
                binFileWriter.write(reinterpret_cast<const char*>(&val), sizeof(val));
            }

            else if (colTypes[i] == 2) // TEXT
            {
                size_t len = cdb->values[i].size();
                binFileWriter.write(reinterpret_cast<const char*>(&len), sizeof(len));
                binFileWriter.write(cdb->values[i].data(), len); // Needed since strings vary in length
            }
        }
    }

    /*************************/
    /*  NEW COLUMN COMMAND */
   /*************************/

    
    if (cmd->type() == commandType::CREATE_INDEX)  // use your actual enum value here
    {
        auto* cdb{ dynamic_cast<createIndex*>(cmd.get()) };
        if (!cdb) {
            std::cerr << "Invalid CREATE NEW COLUMN command.\n";
            return;
        }

        if (!hasOpenDatabase()) {
            std::cerr << "No database opened. Please create or open a database first.\n";
            return;
        }

        std::string tableName = cdb->tableName;
        std::string newColumnType = cdb->columnData.second;

        fs::path dbFolder = fs::path("./databases") / fs::path(currentDB);
        fs::path metaPath = dbFolder / (tableName + ".txt");
        fs::path csvPath = dbFolder / (tableName + ".csv");

        if (!fs::exists(metaPath) || !fs::exists(csvPath)) {
            std::cerr << "Table does not exist: " << tableName << "\n";
            return;
        }

        
        std::ofstream metaOut(metaPath, std::ios::app);
        if (!metaOut) {
            std::cerr << "Failed to open metadata file for appending: " << metaPath << "\n";
            return;
        }
        metaOut << newColumnType << "\n";
        metaOut.close();

       
        std::ifstream csvIn(csvPath);
        if (!csvIn) {
            std::cerr << "Failed to open CSV file: " << csvPath << "\n";
            return;
        }

        std::vector<std::string> rows;
        std::string line;
        while (std::getline(csvIn, line)) {
            rows.push_back(line + ",");
        }
        csvIn.close();

        std::ofstream csvOut(csvPath);
        if (!csvOut) {
            std::cerr << "Failed to open CSV file for writing: " << csvPath << "\n";
            return;
        }
        for (const auto& row : rows) {
            csvOut << row << "\n";
        }
        csvOut.close();

        std::cout << "New column of type '" << newColumnType
            << "' added to table: " << tableName << "\n";
    }
    

}