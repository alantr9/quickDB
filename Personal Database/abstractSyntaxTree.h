#pragma once
#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <optional>

enum class commandType
{
    CREATE_TABLE,
    INSERT,
    SELECT,
    CREATE_INDEX,
    DELETE,
    DROP_TABLE
};

// Intializer to point to a command
struct SQLCommand 
{
    virtual ~SQLCommand() = default;
    virtual commandType type() const = 0;
};

// Child Structs that will overtide commandType type() const
struct createTable : SQLCommand 
{
    std::string tableName;
    std::vector<std::pair<std::string, std::string>> columns; // (columns, data)

    commandType type() const override { return commandType::CREATE_TABLE; }
};


struct insertCommand : SQLCommand 
{
    std::string tableName;
    std::vector<std::string> values;

    commandType type() const override { return commandType::INSERT; }
};


struct selectCommand : SQLCommand 
{
    std::string tableName;
    std::vector<std::string> columns;
    std::optional<std::pair<std::string, std::string>> specificLocation; // (column, value)

    commandType type() const override { return commandType::SELECT; }
};


struct createIndex : SQLCommand 
{
    std::string tableName;
    std::string columnName;

    commandType type() const override { return commandType::CREATE_INDEX; }
};


struct deleteCommand : SQLCommand 
{
    std::string tableName;
    std::pair<std::string, std::string> specificLocation; // (column, value), assume simple WHERE

    commandType type() const override { return commandType::DELETE; }
};


struct dropTable: SQLCommand 
{
    std::string tableName;

    commandType type() const override { return commandType::DROP_TABLE; }
};

#endif 
