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

// Current: Must support INT,TEXT,DATE,TIMESTAMP,BOOLEAN,DECIMAL, VARCHAR()
std::unique_ptr<SQLCommand> parser::parseCreateTable() 
{
	auto sqlcmd{ std::make_unique<createTable>() };
	token currentToken{ tokenHead.getNextToken() };
	sqlcmd->tableName = currentToken.text;

	// Parsing Columnn Names + datatypes
	currentToken = tokenHead.getNextToken();
	if (currentToken.text != "(")
		throw std::runtime_error("Expected '(' after table name");

	while (true) 
	{
		token colNameToken = tokenHead.getNextToken();
		if (colNameToken.type != tokenType::identifier)
			throw std::runtime_error("Expected column name");

		token dataTypeToken = tokenHead.getNextToken();
		if (dataTypeToken.type != tokenType::identifier && dataTypeToken.type != tokenType::keyword)
			throw std::runtime_error("Expected data type for column");

		std::string dataType = dataTypeToken.text;
		token nextToken = tokenHead.peekToken();
		if (nextToken.text == "(") 
		{
			tokenHead.getNextToken(); 
			token sizeToken = tokenHead.getNextToken(); 
			if (sizeToken.type != tokenType::numberLiteral)
				throw std::runtime_error("Expected size for data type");
			token closingParen = tokenHead.getNextToken();
			if (closingParen.text != ")")
				throw std::runtime_error("Expected ')' after size");
			dataType += "(" + sizeToken.text + ")";
		}
		

		sqlcmd->columns.emplace_back(colNameToken.text, dataTypeToken.text);

		nextToken = tokenHead.peekToken();
		if (nextToken.text == ",") 
		{
			tokenHead.getNextToken(); 
			continue;
		}
		else if (nextToken.text == ")") 
		{
			tokenHead.getNextToken(); 
			break;
		}
		else 
		{
			throw std::runtime_error("Expected ',' or ')' after column definition");
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
