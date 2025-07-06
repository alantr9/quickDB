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


struct token 
{
    tokenType   type;
    std::string text;
    size_t      position; // starting position in token

    token(tokenType t, std::string val, size_t pos)
        : type(t), text(std::move(val)), position(pos) {}
};

class tokenizer
{
private:
    const std::string m_input;

    size_t position = 0;
    bool   hasPeeked = false;
    token  cachedToken = token(tokenType::Unknown, "", 0); // Stores the peeked token

    void   skipWhiteSpace();
    token  readIdentifierOrKeyword();
    token  readNumber();
    token  readStringLiteral();
    token  readSymbol();

public:
    explicit tokenizer(const std::string& input);
    token    getNextToken();         // Consume next token
    token    peekToken();            // Look ahead without consuming
    bool     hasMoreTokens() const;
};

#endif 