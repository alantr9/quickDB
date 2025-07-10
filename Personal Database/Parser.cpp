#include <iostream>
#include "Parser.h"


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

// Current: Only supports INT, FLOAT, TEXT data types
std::unique_ptr<SQLCommand> parser::parseCreateTable() 
{
	auto sqlcmd{ std::make_unique<createTable>() };
	token currentToken{ tokenHead.getNextToken() };
	sqlcmd->tableName = currentToken.text;
	
	if( !tokenHead.hasMoreTokens() || tokenHead.peekToken().text != "(")
	{
		throw std::runtime_error("Expected '(' after table name");
	}
	tokenHead.getNextToken(); // Consume '('

	while (tokenHead.hasMoreTokens() || tokenHead.peekToken().text != ")") 
	{
		currentToken = tokenHead.getNextToken();
		if (currentToken.type != tokenType::identifier) 
		{
			throw std::runtime_error("Expected column name");
		}
		std::string columnName = currentToken.text;
		currentToken = tokenHead.getNextToken();
		if (currentToken.type != tokenType::identifier) 
		{
			throw std::runtime_error("Expected data type for column");
		}
		std::string dataType = currentToken.text;
		if (dataType != "INT" && dataType == "FLOAT" && dataType == "TEXT")
		{
			throw std::runtime_error("Unsupported data type: " + dataType);
		}

		sqlcmd->columns.emplace_back(columnName, dataType);
		if (tokenHead.peekToken().text == ",") 
		{
			tokenHead.getNextToken(); // Consume ','
			continue;
		}
	}

	return sqlcmd;
}

std::unique_ptr<SQLCommand> parser::parseCreateIndex() 
{
	throw std::runtime_error("parseCreateIndex not implemented");
}

std::unique_ptr<SQLCommand> parser::parseInsert() 
{
	auto sqlcmd{ std::make_unique<insertCommand>() };
	token currentToken{ tokenHead.getNextToken() };

	if (currentToken.text == "INTO") 
	{
		currentToken = tokenHead.getNextToken();
		sqlcmd->tableName = currentToken.text;
		currentToken = tokenHead.getNextToken(); // Consume '('

		while (tokenHead.hasMoreTokens() || tokenHead.peekToken().text != ")")
		{ 
			currentToken = tokenHead.getNextToken();
			if (currentToken.type != tokenType::identifier) 
			{
				throw std::runtime_error("Expected data");
			}
			if (currentToken.type == tokenType::stringLiteral)
			{
				sqlcmd->values.push_back(currentToken.text); 
				continue;
			}

			sqlcmd->values.push_back(currentToken.text);

			if (tokenHead.peekToken().text == ",") 
			{
				tokenHead.getNextToken(); // Consume ','
				continue;
			}
		}
	}
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
