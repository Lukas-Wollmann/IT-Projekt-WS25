#pragma once

#include <iostream>
#include <string>
#include "core/U8String.h"

enum class TokenType
{
	IDENTIFIER,
	STRING_LITERAL,
	NUMERIC_LITERAL,
	CHAR_LITERAL,
	KEYWORD,
	OPERATOR,
	SEPARATOR,
	COMMENT,
	ILLEGAL
};

struct SourceLoc {
    size_t line, column, index;
};

std::ostream &operator<<(std::ostream &os, TokenType type);

struct Token {
  public:
	const TokenType type;
	const U8String lexeme;
	const SourceLoc loc;
	
	Token(TokenType type, U8String lexeme, SourceLoc loc)
		: type(type), lexeme(std::move(lexeme)), loc(loc) 
	{};
};

std::ostream &operator<<(std::ostream &os, const Token &t);

bool operator==(const Token &left, const Token &right);
bool operator!=(const Token &left, const Token &right);