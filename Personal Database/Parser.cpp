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

		if (nextToken.text == "DATABASE")
		{
			tokenHead.getNextToken();
			return parseCreateDatabase();
		}
		if (nextToken.text == "TABLE")
		{
			tokenHead.getNextToken();
			return parseCreateTable();
		}
		if (nextToken.text == "INDEX")
		{
			tokenHead.getNextToken();
			return parseCreateIndex();
		}
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

std::unique_ptr<SQLCommand> parser::parseCreateDatabase() 
{
	auto sqlcmd{ std::make_unique<createDatabase>() };
	token currentToken{ tokenHead.getNextToken() };
	sqlcmd->dbName = currentToken.text;
	
	return sqlcmd;
}

std::unique_ptr<SQLCommand> parser::parseCreateTable() 
{
	throw std::runtime_error("parseCreateTable not implemented");
}

std::unique_ptr<SQLCommand> parser::parseCreateIndex() 
{
	throw std::runtime_error("parseCreateIndex not implemented");
}

std::unique_ptr<SQLCommand> parser::parseInsert() 
{
	throw std::runtime_error("parseInsert not implemented");
}

std::unique_ptr<SQLCommand> parser::parseSelect() 
{
	throw std::runtime_error("parseSelect not implemented");
}

std::unique_ptr<SQLCommand> parser::parseDelete() 
{
	throw std::runtime_error("parseDelete not implemented");
}

std::unique_ptr<SQLCommand> parser::parseDropTable() 
{
	throw std::runtime_error("parseDropTable not implemented");
}

/*
std::unique_ptr<SQLCommand> parser::parseCreateIndex()
{
	auto sqlcmd{ std::make_unique<createIndex>() };
	
	token currentToken = tokenHead.getNextToken();
	sqlcmd->tableName{ currentToken };

}
*/