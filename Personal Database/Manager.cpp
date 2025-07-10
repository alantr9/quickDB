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


void manager::dbLogger(std::string name)
{

    if (doesDatabaseExists(name)) 
    {
        std::cout << "Database opened. \n";
        return;
	}
    else if (!doesDatabaseExists(name))
    {
        std::ofstream writeFile("databaseNames.csv", std::ios::app); // Append mode
		writeFile << name << ",\n";
        writeFile.close();
        std::cout << "Database created: " << name << "\n";

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
        auto* cdb = dynamic_cast<createDatabase*>(cmd.get());
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
        auto* cdb = dynamic_cast<createTable*>(cmd.get());
        
        if(hasOpenDatabase() == false) 
        {
            std::cout << "No database opened. Please create or open a database first.\n";
			std::cout << currentDB << "\n";
            return;
        }

        fs::path dbFolder = fs::path("./databases") / fs::path(currentDB);  // ?? main fix
        std::error_code ec;

        fs::create_directories(dbFolder, ec);
        if (ec)
        {
            std::cerr << "Failed to create database directory: " << ec.message() << "\n";
            return;
        }

        fs::path csvPath = dbFolder / (cdb->tableName + ".csv");
        if (fs::exists(csvPath))
        {
            std::cerr << "Table Already Exists \n";
            return;
        }
        std::ofstream csvFile(csvPath);
        if (!csvFile)
        {
            std::cerr << "Failed to create CSV file: " << csvPath << "\n";
            return;
        }


        for (size_t i{ 0 }; i < cdb->columns.size(); ++i)
        {
            csvFile << cdb->columns[i].first;
            if (i < cdb->columns.size() - 1)
                csvFile << ",";
        }
        csvFile << "\n";
        csvFile.close();

        std::cout << "Table created with CSV file: " << csvPath << "\n";

        fs::path metaPath = dbFolder / (cdb->tableName + ".txt"); // to differentiate between .csv files
        std::ofstream metaFile(metaPath);
        if (!metaFile)
        {
            std::cerr << "Failed to create metadata file: " << metaPath << "\n";
            return;
        }

        
        for (const auto& col : cdb->columns)
        {
            metaFile << col.second << "\n";
        }

        metaFile.close();
        std::cout << "Metadata file created: " << metaPath << "\n";
    }


    /*************************/
    /*  CREATE INDEX COMMAND */
    /*************************/

    if(cmd->type() == commandType::CREATE_INDEX)
    {
        auto* cdb = dynamic_cast<createIndex*>(cmd.get());
        if (!hasOpenDatabase())
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }
        if (!cdb)
        {
            std::cerr << "Invalid CREATE INDEX command.\n";
            return;
        }


	}

   /*************************/
   /*  SELECT COMMAND */
   /*************************/
   
    // NOT FINISHED - IMPLEMENT WITH BTREES

    if (cmd->type() == commandType::SELECT) 
    {
        auto* cdb = dynamic_cast<selectCommand*>(cmd.get());

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
    
    if (cmd->type() == commandType::INSERT) // have to fix to match actual insert command
    {
        auto* cdb = dynamic_cast<insertCommand*>(cmd.get());

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

        
        fs::path tablePath = fs::path("./databases") / currentDB / (cdb->tableName + ".csv");
        if (!fs::exists(tablePath))
        {
            std::cerr << "Table not found: " << cdb->tableName << "\n";
            return;
        }

        std::ofstream tableFile(tablePath, std::ios::app);
        if (!tableFile)
        {
            std::cerr << "Failed to open table file for writing.\n";
            return;
        }

        std::ifstream metaFile(fs::path("./databases") / currentDB / (cdb->tableName + ".txt"));
        std::vector<std::string> columnTypes;
        std::string line;
        while (std::getline(metaFile, line)) 
        {
			columnTypes.push_back(line);
        }
		metaFile.close();

        for (size_t g{ 0 }; g < cdb->values.size(); ++g)
        {
            if (g >= columnTypes.size())
            {
                std::cerr << "Error: More values than columns in table.\n";
                return;
            }

            if (columnTypes[g] == "INT" && !std::all_of(cdb->values[g].begin(), cdb->values[g].end(), ::isdigit))
            {
                std::cerr << "Error: Value '" << cdb->values[g] << "' is not a valid INT.\n";
                return;
            }
            else if (columnTypes[g] == "FLOAT")
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

        for (size_t i{ 0 }; i < cdb->values.size(); ++i)
        {
            tableFile << cdb->values[i];
            if (i < cdb->values.size() - 1)
                tableFile << ",";
        }

        tableFile << "\n";
        tableFile.close();

        std::cout << "Row inserted into " << cdb->tableName << ".\n";
    }

    /*************************/
       /*  NEW COLUMN COMMAND */
   /*************************/


    if (cmd->type() == commandType::CREATE_INDEX)  // use your actual enum value here
    {
        auto* cdb = dynamic_cast<createIndex*>(cmd.get());
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