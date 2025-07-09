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
    namespace fs = std::filesystem;

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

    }
}