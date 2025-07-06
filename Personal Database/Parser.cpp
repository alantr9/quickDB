#include <iostream>
#include "Parser.h"
#include "Tokenizer.h"

parser::parser(const tokenizer& token) :
	tokenHead{ token } {}; // Maybe optimzed using std::move()

std::unique_ptr<SQLCommand> parser::parseCommand()
{
	token currentToken = tokenHead.peekToken();

	std::string keyWord;
	if (currentToken.type == tokenType::keyword)
		keyWord = currentToken.text;
	else
		throw std::runtime_error("Unknown command: " + currentToken.text);

	if (keyWord == "CREATE")
	{
		tokenHead.getNextToken();
		token nextToken = tokenHead.peekToken();

		if (nextToken.text == "TABLE") return parseCreateTable();
		if (nextToken.text == "INDEX") return parseCreateIndex();
	}
	else if (keyWord == "INSERT")
	{
		tokenHead.getNextToken();
		return parseInsert();
	}
	else if (keyWord == "SELECT")
	{
		tokenHead.getNextToken();
		return parseSelect();
	}
	else if (keyWord == "DELETE")
	{
		tokenHead.getNextToken();
		return parseDelete();
	}
	else if (keyWord == "DROP")
	{
		tokenHead.getNextToken();
		return parseDropTable();
	}

	throw std::runtime_error("Unknown Command");
}
