#pragma once
#include <string>
#include <iostream>

enum class TokenType 
{
	IDENTIFIER, STRING_LITERAL, NUMERIC_LITERAL, CHAR_LITERAL, KEYWORD, OPERATOR, SEPERATOR
};

std::ostream &operator<<(std::ostream &os, TokenType type);

struct Token 
{
	const TokenType type;
	const std::string lexeme;
	const size_t line, column, index;
};

std::ostream &operator<<(std::ostream &os, const Token &t);