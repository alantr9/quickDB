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
		if (nextToken.text == "NEWCOLUMN")
		{
			tokenHead.getNextToken();
			return parseCreateNewColumn();
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
		token nextToken = tokenHead.getNextToken(); 

		if (nextToken.text == "TABLE")
			return parseDropTable();
		else if (nextToken.text == "DATABASE")
			return parseDropDatabase();
		else
			throw std::runtime_error("Expected TABLE or DATABASE after DROP");
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

std::unique_ptr<SQLCommand> parser::parseCreateNewColumn()
{
	auto sqlcmd { std::make_unique<createIndex>() };
	token currentToken{ tokenHead.getNextToken() };

	sqlcmd->tableName = currentToken.text;
	if (!tokenHead.hasMoreTokens() || tokenHead.peekToken().text != "(")
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

		sqlcmd->columnData.first = columnName;
		sqlcmd->columnData.second = dataType;
		if (tokenHead.peekToken().text == ",")
		{
			throw(std::runtime_error("Plase add only one column at a time"));
		}
	}

	return sqlcmd;
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
			if (currentToken.type != tokenType::identifier && currentToken.type != tokenType::numberLiteral) 
			{

                std::cout << static_cast<int>(currentToken.type) << "\n";
				throw std::runtime_error("Expected data:");
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
	return sqlcmd;
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
	auto sqlcmd = std::make_unique<dropTable>();

	if (!tokenHead.hasMoreTokens())
	{
		throw std::runtime_error("Expected TABLE keyword after DROP");
	}

	token nextToken = tokenHead.getNextToken();

	if (nextToken.text != "TABLE")
	{
		throw std::runtime_error("Expected TABLE keyword after DROP");
	}

	if (!tokenHead.hasMoreTokens())
	{
		throw std::runtime_error("Expected table name after DROP TABLE");
	}

	sqlcmd->tableName = tokenHead.getNextToken().text;
	return sqlcmd;
}
std::unique_ptr<SQLCommand> parser::parseDropDatabase()
{
	auto sqlcmd = std::make_unique<dropDatabase>();

	if (!tokenHead.hasMoreTokens())
	{
		throw std::runtime_error("Expected database name after DROP DATABASE");
	}

	sqlcmd->dbName = tokenHead.getNextToken().text;
	return sqlcmd;
}

