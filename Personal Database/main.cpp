#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "abstractSyntaxTree.h"
#include "Tokenizer.h"

void test()
{
    tokenizer tokenizer("INSERT INTO students VALUES ('Alice', 20);");
    while (tokenizer.hasMoreTokens()) {
        token t = tokenizer.getNextToken();
        std::cout << "Token: " << t.text << " | Type: " << static_cast<int>(t.type) << "\n";
    }
}
std::vector<std::string> tokenize(const std::string& input) 
{
    std::stringstream ss(input);
    std::vector<std::string> tokenVector;
    std::string token;

    // Add CLI strings to tokenVector
    while (ss >> token) 
    {
        tokenVector.push_back(token);
    }

    return tokenVector;
}


void executeQuery(const std::string& input) {
    auto tokens { tokenize(input) };
    if (tokens.empty()) return;

    std::string command{ tokens[0] };

    if (command == "CREATE") 
    {
        std::cout << "[CREATE] Not yet implemented\n";
    }
    else if (command == "INSERT") 
    {
        std::cout << "[INSERT] Not yet implemented\n";
    }
    else if (command == "SELECT") 
    {
        std::cout << "[SELECT] Not yet implemented\n";
    }
    else if (command == "CREATE TABLE")
    {
        std::cout << "[CREATE TABLE] Not yet implemented\n";
    }
    else if (command == "DELETE")
    {
        std::cout << "[DELETE] Not yet implemented\n";
    }
    else if (command == "DROP")
    {
        std::cout << "[DROP] Not yet implemented\n";
    }
    else if (command == "TEST")
    {
        test();
    }
    else 
    {
        std::cout << "[ERROR] Unknown command: " << command << "\n";
    }
}

int main(int argc, char* argv[]) // Personal Data.sln -> Properties -> Configure Properties -> Debugging -> Command Arguments
{ 
    if (argc < 2) 
    {
        std::cerr << "Usage: " << argv[0] << " <database_file>\n";
        return 1;
    }

    std::string dbFilename{ argv[1] };
    std::cout << "Opening database: " << dbFilename << "\n";

    std::string input;
    std::cout << "SQLite CLI (type 'exit;' to quit)\n";

    while (true) 
    {
        std::cout << "sqlite> ";
        std::getline(std::cin, input);

        if (input == "exit;" || input == "quit;") 
        {
            break;
        }

        executeQuery(input);
    }

    return 0;
}


