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
        std::ofstream csvFile(csvPath);
        if (!csvFile)
        {
            std::cerr << "Failed to create CSV file: " << csvPath << "\n";
            return;
        }


        for (size_t i = 0; i < cdb->columns.size(); ++i)
        {
            csvFile << cdb->columns[i].first << " " << cdb->columns[i].second;
            if (i < cdb->columns.size() - 1)
                csvFile << ",";
        }
        csvFile << "\n";
        csvFile.close();

        std::cout << "Table created with CSV file: " << csvPath << "\n";
    }

   /*************************/
   /*  SELECT COMMAND */
   /*************************/
   
    if (cmd->type() == commandType::SELECT) 
    {
        auto* scmd = dynamic_cast<selectCommand*>(cmd.get());

        if (!hasOpenDatabase())
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }

        if (!scmd)
        {
            std::cout << "Invalid SELECT command.\n";
            return;
        }

        fs::path tablePath = fs::path("./databases") / currentDB / (scmd->tableName + ".csv");

        if (!fs::exists(tablePath))
        {
            std::cerr << "Table does not exist: " << scmd->tableName << "\n";
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




}