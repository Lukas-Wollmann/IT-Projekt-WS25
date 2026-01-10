#pragma once

#include "Token.h"
#include "core/U8String.h"

namespace lexer {
	struct Lexer {
	public:
		static Vec<Token> tokenize(const U8String &source, const U8String &filename,
								   bool comments = false);

	private:
		const U8String &m_Source;
		U8String::ConstIterator m_Iter;
		char32_t m_Current;
		SourceLoc m_CurrentLoc;

		Lexer(const U8String &source, U8String filename);

		Token nextToken();

		bool isAtEnd() const;
		void advance();
		void skipWhitespace();
		char32_t peek() const;
		bool doesMatch(const U8String &str) const;

		char32_t getEscapedChar(char32_t c);

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