#include "Token.h"

namespace lex {
Token::Token(const TokenType type, U8String lexeme, const SourceLoc &loc)
	: type(type)
	, lexeme(std::move(lexeme))
	, loc(loc) {}

bool Token::matches(const TokenType otherType, const U8String &otherLexeme) const {
	return type == otherType && lexeme == otherLexeme;
}

bool Token::matches(const TokenType otherType) const {
	return type == otherType;
}

bool Token::operator==(const Token &other) const {
	return type == other.type && lexeme == other.lexeme;
}

bool Token::operator!=(const Token &other) const {
	return !(*this == other);
}
}