#pragma once

#include "Token.h"
#include "core/ErrorHandler.h"
#include "core/U8String.h"

namespace lex {
///
/// Tokenize a string of utf-8 characters into a vector of tokens. All functionality is public
/// for testing purposes, do only use the static tokenize(...) as an interface for this class.
///
struct Lexer {
	static Vec<Token> tokenize(const U8String &source, ErrorHandler &err);

	const U8String &m_Source;
	U8String::ConstIterator m_Iter;
	char32_t m_Current;
	SourceLoc m_CurrentLoc;
	ErrorHandler &m_ErrorHandler;

	Lexer(const U8String &source, ErrorHandler &err);

	Token nextToken();

	[[nodiscard]] SourceLoc span(SourceLoc loc) const;
	[[nodiscard]] bool isAtEnd() const;
	void advance();
	void skipWhitespace();
	[[nodiscard]] char32_t peek() const;
	[[nodiscard]] bool doesMatch(const U8String &str) const;

	static char32_t getEscapedChar(char32_t c);

	Opt<Token> tryLexIdentifier();
	Opt<Token> tryLexIntLiteral();
	Opt<Token> tryLexStringLiteral();
	Opt<Token> tryLexCharLiteral();
	Opt<Token> tryLexOperator();
	Opt<Token> tryLexSeparator();
	Opt<Token> tryLexSingleLineComment();
	Opt<Token> tryLexMultiLineComment();
	Token lexIllegal();
};
}