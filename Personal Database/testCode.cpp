#include "Tokenizer.h"
#include "abstractSyntaxTree.h"
#include "Manager.h"
#include "Parser.h"
#include <iostream>

void test()
{
    tokenizer tokenizerTest("INSERT INTO students VALUES ('Alice', 20);");
    while (tokenizerTest.hasMoreTokens()) {
        token t = tokenizerTest.getNextToken();
        std::cout << "Token: " << t.text << " | Type: " << static_cast<int>(t.type) << "\n";
    }
}