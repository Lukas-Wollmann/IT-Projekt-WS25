#pragma once
#include <string>
#include <iostream>

enum class TokenType {
	IDENTIFIER, STRING_LITERAL, NUMERIC_LITERAL, CHAR_LITERAL, KEYWORD, OPERATOR, SEPERATOR
};

std::string tokenTypeToString(const TokenType type);

struct Token {
	TokenType type;
	std::string lexeme;
	size_t line, column, index;

	friend std::ostream &operator<<(std::ostream &os, const Token &t);
};
