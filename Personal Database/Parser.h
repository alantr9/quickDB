#pragma once
#ifndef PARSER_H
#define PARSER_H

#include<iostream>
#include "abstractSyntaxTree.h"
#include "Tokenizer.h"

class parser {
private:
    tokenizer tokenizer_;

    std::unique_ptr<SQLCommand> parseCreateTable();
    std::unique_ptr<SQLCommand> parseInsert();
    std::unique_ptr<SQLCommand> parseSelect();
    std::unique_ptr<SQLCommand> parseCreateIndex();
    std::unique_ptr<SQLCommand> parseDelete();
    std::unique_ptr<SQLCommand> parseDropTable();

    token expectedToken(tokenType expectedType, const std::string& expectedText = "");
    bool  matchedToken(tokenType type, const std::string& text = "");
public:
    explicit parser(tokenizer tokenizer);

    std::unique_ptr<SQLCommand> parse();  // Main pointer for token
};

#endif 