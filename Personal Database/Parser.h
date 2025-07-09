#pragma once
#ifndef PARSER_H
#define PARSER_H

#include<iostream>
#include "Tokenizer.h"
#include "abstractSyntaxTree.h"

class parser {
private:
    tokenizer tokenHead;

    std::unique_ptr<SQLCommand> parseOpenDatabase();
    std::unique_ptr<SQLCommand> parseCreateDatabase();
    std::unique_ptr<SQLCommand> parseCreateTable();
    std::unique_ptr<SQLCommand> parseInsert();
    std::unique_ptr<SQLCommand> parseSelect();
    std::unique_ptr<SQLCommand> parseCreateIndex();
    std::unique_ptr<SQLCommand> parseDelete();
    std::unique_ptr<SQLCommand> parseDropTable();

    token expect(tokenType expectedType, const std::string& expectedText = "");
    bool  match(tokenType type, const std::string& text = "");

public:
    explicit parser(const tokenizer& token);

    std::unique_ptr<SQLCommand> parseCommand();  // Main entry point
};

#endif 