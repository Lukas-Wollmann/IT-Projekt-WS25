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
		UnterminatedString,
		UnterminatedBlockComment,
		UnterminatedCharLiteral,
		EmptyCharLiteral,
		MultipleCharsInCharLiteral,
		InvalidEscapeSequence,
		IllegalIdentifier
	};

	struct Token {
	public:
		TokenType type;
		U8String lexeme;
		SourceLoc loc;
		TokenError error;

		Token(TokenType type, U8String lexeme, SourceLoc loc, TokenError error = TokenError::None);
	};
}

std::ostream &operator<<(std::ostream &os, lexer::TokenType type);
std::ostream &operator<<(std::ostream &os, lexer::TokenError error);
std::ostream &operator<<(std::ostream &os, const lexer::Token &token);

bool operator==(const lexer::Token &left, const lexer::Token &right);
bool operator!=(const lexer::Token &left, const lexer::Token &right);