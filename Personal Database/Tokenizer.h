#pragma once
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>

enum class tokenType
{
    keyword,
    identifier,
    symbol,
    stringLiteral,
    numberLiteral,
    endOfFile,
    Unknown
};


struct Token {
    tokenType   type;
    std::string text;
    size_t      position; // position in token

    Token(tokenType t, std::string val, size_t pos)
        : type(t), text(std::move(val)), position(pos) {
    }
};


#endif 