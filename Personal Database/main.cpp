#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "abstractSyntaxTree.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "Manager.h"


int main(int argc, char* argv[]) 
{
    std::filesystem::create_directory(std::filesystem::path("./databases")); // Creates databases folder if not exists

    std::cout << "Welcome to Alan and Pranav's SQLite! No database opened yet.\n";
    manager manager;

    std::string inputLine;
    while (true) 
    {
        std::cout << (manager.hasOpenDatabase() ? manager.getCurrentDatabase() + "> " : "sqlite> ");

        if (!std::getline(std::cin, inputLine)) break;  // EOF or error
        if (inputLine.empty()) continue;
        if (inputLine == "exit;" || inputLine == "quit;") break;


        try 
        {
            tokenizer tokenizerObj(inputLine);
            parser parserObj(tokenizerObj);

            auto commandAST = parserObj.parseCommand();

            manager.execute(std::move(commandAST));
        }
        catch (const std::exception& err) 
        {
            std::cerr << "Error: " << err.what() << "\n";
        }
    }
    return 0;
}
