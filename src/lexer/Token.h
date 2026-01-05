#pragma once

#include <iostream>
#include <string>

#include "core/U8String.h"

enum class TokenType {
	IDENTIFIER,
	STRING_LITERAL,
	NUMERIC_LITERAL,
	CHAR_LITERAL,
	BOOL_LITERAL,
	KEYWORD,
	OPERATOR,
	SEPARATOR,
	COMMENT,
	ILLEGAL,
	END_OF_FILE
};

enum class ErrorTypeToken {
	NOT_ILLEGAL,

	UNTERMINATED_STRING,
	SOLO_BACKSLASH_IN_CHAR_LITERAL,
	INVALID_ESCAPE_SEQUENCE,
	MULTIPLE_CHAR_IN_CHAR_LITERAL,
	UNTERMINATED_CHAR_LITERAL,
	EMPTY_CHAR_LITERAL,
	UNTERMINATED_BLOCK_COMMENT,
	ILLEGAL_IDENTIFIER
};

struct SourceLoc {
	size_t line, column, index;
};

std::ostream &operator<<(std::ostream &os, TokenType type);

struct Token {
public:
	TokenType type;
	U8String lexeme;
	SourceLoc loc;

	ErrorTypeToken errorType = ErrorTypeToken::NOT_ILLEGAL;

	Token(TokenType type, U8String lexeme, SourceLoc loc = {0, 0, 0},
		  ErrorTypeToken errorType = ErrorTypeToken::NOT_ILLEGAL)
		: type(type)
		, lexeme(std::move(lexeme))
		, loc(loc)
		, errorType(errorType) {}
};

std::ostream &operator<<(std::ostream &os, const Token &t);

bool operator==(const Token &left, const Token &right);
bool operator!=(const Token &left, const Token &right);