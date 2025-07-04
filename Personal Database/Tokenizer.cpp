#include <iostream>
#include "Tokenizer.h"

tokenizer::tokenizer(const std::string& input) :
	m_input{ input } {};

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

