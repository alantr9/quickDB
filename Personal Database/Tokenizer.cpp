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

