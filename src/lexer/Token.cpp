#include "Token.h"

#include <sstream>

#include "Macros.h"

namespace lexer {
	Token::Token(const TokenType type, U8String lexeme, const SourceLoc &loc)
		: type(type)
		, lexeme(std::move(lexeme))
		, loc(loc) {}

	bool Token::matches(TokenType otherType, U8String otherLexeme) const {
		return type == otherType && lexeme == otherLexeme;
	}

	bool Token::matches(TokenType otherType) const {
		return type == otherType;
	}

	bool operator==(const Token &left, const Token &right) {
		return left.type == right.type && left.lexeme == right.lexeme;
	}

	bool operator!=(const Token &left, const Token &right) {
		return !(left == right);
	}
}