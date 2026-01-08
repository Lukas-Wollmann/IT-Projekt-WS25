#pragma once
#include <array>
#include <string>
#include <vector>

#include "Token.h"
#include "core/U8String.h"

namespace lexer {
	struct Lexer {
	public:
		explicit Lexer(U8String &&source, bool inludeComments = false);
		std::vector<Token> tokenize();

	private:
		U8String m_Src;
		U8String::ConstIterator m_Iter;
		char32_t m_CurentChar;
		SourceLoc m_Loc;
		bool m_includeComments;

		void advance();
		bool isCurrentSingleOperator() const;
		bool isCurrentDoubleOperator() const;
		bool isCurrentTripleOperator() const;
		bool isCurrentSeparator() const;
		bool isKeyword(const U8String &lexeme) const;
		bool isCurrentComment() const;
		bool isStartBlockComment() const;
		bool isAtEnd() const;
		char32_t peek(size_t distance) const;
		void skipWhitespace();
		U8String skipToClosing();

		Token lexNumber(SourceLoc startLoc);
		Token lexString(SourceLoc startLoc);
		Token lexEscapedChar(SourceLoc startLoc);
		Token lexChar(SourceLoc startLoc);
		Token lexSeparator(SourceLoc startLoc);
		Token lexOperator(SourceLoc startLoc);
		Token lexIdentifierOrKeyword(SourceLoc startLoc);
		Token lexComment(SourceLoc startLoc);
		Token lexBlockComment(SourceLoc startLoc);
		Token lexIllegal(SourceLoc startLoc);
	};
}