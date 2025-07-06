#include <iostream>
#include "Parser.h"

parser::parser(const tokenizer& token) :
	tokenHead{ token } {}; // Maybe optimzed using std::move() 