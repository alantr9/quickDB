#pragma once
#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "abstractSyntaxTree.h"


class manager {
private:
    std::string currentDB;
    std::vector<std::string> databaseNames;

public:
    explicit manager();
    bool hasOpenDatabase() const;
    std::string getCurrentDatabase() const;
    void execute(std::unique_ptr<SQLCommand> cmd);
    void dbLogger(std::string name);
};


#endif 