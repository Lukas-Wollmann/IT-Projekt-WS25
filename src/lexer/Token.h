#pragma once

#include <iostream>
#include <string>

#include "core/SourceLoc.h"
#include "core/U8String.h"

namespace lexer {
	enum struct TokenType {
		Identifier,
		StringLiteral,
		IntLiteral,
		CharLiteral,
		BoolLiteral,
		Keyword,
		Operator,
		Separator,
		Comment,
		Illegal,
		EndOfFile
	};

	enum struct TokenError {
		None,
		UnterminatedStringLiteral,
		UnterminatedBlockComment,
		UnterminatedCharLiteral,
		EmptyCharLiteral,
		MultipleCharsInCharLiteral,
		InvalidEscapeSequence,
		IllegalIdentifier
	};

	struct Token {
		TokenType type;
		U8String lexeme;
		SourceLoc loc;
		TokenError error;

		Token(TokenType type, U8String = u8"", const SourceLoc &loc = {0, 0, 0},
			  TokenError error = TokenError::None);

		bool matches(TokenType otherType, U8String otherLexeme) const;
		bool matches(TokenType otherType) const;
		U8String str() const;
	};

	bool operator==(const Token &left, const Token &right);
	bool operator!=(const Token &left, const Token &right);

	std::ostream &operator<<(std::ostream &os, TokenType type);
	std::ostream &operator<<(std::ostream &os, TokenError error);
	std::ostream &operator<<(std::ostream &os, const Token &token);
}