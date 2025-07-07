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

    if (isFound)
        databaseNames.push_back(name);
}

void manager::execute(std::unique_ptr<SQLCommand> cmd) 
{
    if (!cmd) return;

    if (cmd->type() == commandType::CREATE_DATABASE) 
    { 
        // createDatabase type
        auto* cdb = dynamic_cast<createDatabase*>(cmd.get());
        if (cdb) 
        {
            currentDB = cdb->dbName;
            dbLogger(currentDB);
            std::cout << "Database '" << currentDB << "' created and opened.\n";
        }
    }
    else 
    {
        if (hasOpenDatabase() == false) 
        {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }

        std::cout << "Executing command on database: " << currentDB << "\n";
    }
}