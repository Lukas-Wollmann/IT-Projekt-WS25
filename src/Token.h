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

struct SourceLoc {
    size_t line, column, index;
};

std::ostream &operator<<(std::ostream &os, TokenType type);

struct Token {
  public:
	const TokenType type;
	const std::string lexeme;
	const SourceLoc loc;
	
	Token(TokenType type, std::string lexeme, SourceLoc loc)
		: type(type), lexeme(std::move(lexeme)), loc(loc) 
	{};
};

std::ostream &operator<<(std::ostream &os, const Token &t);