#include <iostream>
#include <string>
#include "Manager.h"

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

void manager::dbLogger(std::string name)
{
    bool isFound{ false };

    for (auto i : databaseNames) 
    {
        if (i == name) 
        {
            isFound = true;
            break;
        }
    }

    if (!isFound)
        databaseNames.push_back(name);
    else
        std::cout << "Database already exists";
}

void manager::execute(std::unique_ptr<SQLCommand> cmd) 
{
    if (!cmd) return;

    if (cmd->type() == commandType::CREATE_DATABASE) 
    { 
        auto* cdb = dynamic_cast<createDatabase*>(cmd.get());
        if (cdb) 
        {
            currentDB = cdb->dbName;
            dbLogger(currentDB);
            std::cout << "Database '" << currentDB << "' created/opened.\n";
        }
    }
    else 
    {
        if (hasOpenDatabase() == false) 
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }
    }

    if (cmd->type() == commandType::CREATE_TABLE)
    {
        auto* cdb = dynamic_cast<createTable*>(cmd.get());
        
        if(hasOpenDatabase() == false) 
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }

        // How to store this
    }
}