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
    OPEN,
    CREATE_DATABASE,
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
struct openCommand : SQLCommand 
{
    std::string dbName;
    commandType type() const override { return commandType::OPEN; }
};

struct createDatabase : SQLCommand
{
    std::string dbName;
    std::vector<std::string> tableNames;

    commandType type() const override { return commandType::CREATE_DATABASE; }
};

struct createTable : SQLCommand 
{
    std::string tableName;
    std::vector<std::pair<std::string, std::string>> columns; // fetching columnName and datatype

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
    std::vector<std::string> columnName;
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
    std::string deleteRow; // using WHERE
    std::optional<std::pair<std::string, std::string>> specificLocation; // (column, value), assume simple WHERE

    commandType type() const override { return commandType::DELETE; }
};

struct dropTable: SQLCommand 
{
    std::string tableName;

    commandType type() const override { return commandType::DROP_TABLE; }
};

#endif 
