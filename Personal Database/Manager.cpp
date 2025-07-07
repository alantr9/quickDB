#include <iostream>
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

void manager::execute(std::unique_ptr<SQLCommand> cmd) 
{
    if (!cmd) return;

    // Example: handle CREATE_DATABASE command
    if (cmd->type() == commandType::CREATE_DATABASE) { // createDatabase type
        auto* cdb = dynamic_cast<createDatabase*>(cmd.get());
        if (cdb) {
            currentDB = cdb->dbName;
            std::cout << "Database '" << currentDB << "' created and opened.\n";
        }
    }
    else {
        if (hasOpenDatabase() == false) {
            std::cout << "No database opened. Please create or open a database first.\n";
            return;
        }

        std::cout << "Executing command on database: " << currentDB << "\n";
    }
}