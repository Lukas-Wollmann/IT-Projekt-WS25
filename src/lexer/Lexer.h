#pragma once

#include "Token.h"
#include "core/ErrorHandler.h"
#include "core/U8String.h"

namespace lexer {
	struct LexerError {
		U8String message;
		Token token;
	};

	struct Lexer {
		static Vec<Token> tokenize(const U8String &source, ErrorHandler &err);

	private:
		const U8String &m_Source;
		U8String::ConstIterator m_Iter;
		char32_t m_Current;
		SourceLoc m_CurrentLoc;

		explicit Lexer(const U8String &source);

		Token nextToken();

		SourceLoc offset(SourceLoc loc) const;
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