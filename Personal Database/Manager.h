#pragma once
#ifndef MANAGER_H
#define MANAGER_H

#include <iostream>
#include "abstractSyntaxTree.h"

class manager {
private:
    std::string currentDB;

public:
    explicit manager();
    bool hasOpenDatabase() const;
    std::string getCurrentDatabase() const;
    void execute(std::unique_ptr<SQLCommand> cmd);
};


#endif 