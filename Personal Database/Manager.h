#pragma once
#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "abstractSyntaxTree.h"


class manager {
private:
    std::string currentDB;

public:
    explicit manager();
    bool hasOpenDatabase() const;
    std::string getCurrentDatabase() const;
    void execute(std::unique_ptr<SQLCommand> cmd);
    void dbLogger(std::string name) const;
    bool doesDatabaseExists(const std::string& dbName) const;
};


#endif 