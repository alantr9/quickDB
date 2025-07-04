#include <iostream>
#include "Tokenizer.h"

tokenizer::tokenizer(const std::string& input) :
	m_input{ input } {};

token tokenizer::getNextToken()
{
	if (hasPeeked)
	{
		hasPeeked = false;
		return cachedToken;
	}
	skipWhiteSpace();


	if (position >= m_input.size()) 
	{
		return token(tokenType::endOfFile, "", position);
	}

	if (std::isalpha(m_input[position]) || m_input[position] == '_') 
	{
		return readIdentifierOrKeyword();
	} 
	else if (std::isdigit(m_input[position])) 
	{
		return readNumber();
	} 
	else if (m_input[position] == '\'' || m_input[position] == '"') 
	{
		return readStringLiteral();
	} 
	else if (std::ispunct(m_input[position])) 
	{
		return readSymbol();
	} 
	else 
	{
		std::cerr << "Error: Unknown character '" << m_input[position] << "' at position " << position << "\n";
		return token(tokenType::Unknown, std::string(1, m_input[position++]), position - 1);
	}
}

token tokenizer::peekToken()
{
	if (hasPeeked == false)
	{
		cachedToken = getNextToken();
		hasPeeked = true;
	}
	return cachedToken;
}

void tokenizer::skipWhiteSpace()
{
	while (position < m_input.size() && std::isspace(m_input[position])) 
	{
		++position;
	}
}

bool tokenizer::hasMoreTokens() const
{
	return position < m_input.size();
}

token tokenizer::readIdentifierOrKeyword()
{
	size_t start = position;
	// Reads one word in the the query
	while(position < m_input.size() && (std::isalnum(m_input[position]) || m_input[position] == '_')) 
	{
		++position;
	}	

	std::string queryWord = m_input.substr(start, position - start);
	if (queryWord == "CREATE" || queryWord == "INSERT" || queryWord == "SELECT" ||
		queryWord == "DELETE" || queryWord == "DROP" || queryWord == "TABLE" || 
		queryWord == "INDEX") 
	{
		return token(tokenType::keyword, queryWord, start);
	} 
	else 
	{
		return token(tokenType::identifier, queryWord, start);
	}
}

token tokenizer::readNumber()
{
	size_t start = position;
	while (position < m_input.size() && std::isdigit(m_input[position])) 
	{
		++position;
	}
	return token(tokenType::numberLiteral, m_input.substr(start, position - start), start);
}

token tokenizer::readStringLiteral()
{
	char quoteType = m_input[position];
	size_t start = ++position;
	while (position < m_input.size() && m_input[position] != quoteType)
	{
		++position;
	}

	std::string strLiteral = m_input.substr(start, position - start);

	// Chek if we reached the end of the input without finding the closing quote
	if (position >= m_input.size() || m_input[position] != quoteType) 
	{
		std::cerr << "Error: Unterminated string literal at position " << start << std::endl;
		return token(tokenType::Unknown, strLiteral, start - 1);
	}

	// Move past the closing quote
	++position;

	return token(tokenType::stringLiteral, strLiteral, start - 1);
}

token tokenizer::readSymbol()
{
	return token(tokenType::symbol, std::string(1, m_input[position++]), position - 1);
}