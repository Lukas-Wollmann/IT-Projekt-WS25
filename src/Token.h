#pragma once

#include <iostream>
#include <string>

enum class TokenType
{
	IDENTIFIER,
	STRING_LITERAL,
	NUMERIC_LITERAL,
	CHAR_LITERAL,
	KEYWORD,
	OPERATOR,
	SEPERATOR
};

std::ostream &operator<<(std::ostream &os, TokenType type);

struct Token {
  public:
	const TokenType type;
	const std::string lexeme;
	const size_t line, column, index;

	Token(TokenType type, std::string lexeme, size_t line, size_t column, size_t index)
		: type(type), lexeme(std::move(lexeme)), line(line), column(column), index(index) {};
};

std::ostream &operator<<(std::ostream &os, const Token &t);