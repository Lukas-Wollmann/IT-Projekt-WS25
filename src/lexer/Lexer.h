#pragma once
#include <array>
#include <string>
#include <vector>

#include "Token.h"
#include "core/U8String.h"

class Lexer {
private:
	static constexpr std::array<char32_t, 10> s_Separators = {U';', U',', U'(', U')', U'{',
															  U'}', U'[', U']', U':', U'.'};
	static inline const std::array<U8String, 6> s_Keywords = {u8"if",	  u8"else", u8"while",
															  u8"return", u8"func", u8"new"};
	// u8"i32", u8"u32", u8"f32", u8"string", u8"char", u8"bool"
	static inline const std::array<U8String, 13> s_SingleOps = {u8"+", u8"-", u8"*", u8"/", u8"=",
																u8"!", u8"<", u8">", u8"&", u8"|",
																u8"^", u8"%", u8"~"};
	static inline const std::array<U8String, 17> s_DoubleOps = {u8"&&", u8"||", u8"==", u8"<=",
																u8">=", u8"!=", u8"<<", u8">>",
																u8"+=", u8"-=", u8"*=", u8"/=",
																u8"%=", u8"^=", u8"&=", u8"|=",
																u8"->"};
	static inline const std::array<U8String, 2> s_TripleOps = {u8"<<=", u8">>="};

public:
	explicit Lexer(U8String &&source);
	std::vector<Token> tokenize();

private:
	U8String m_Src;
	U8String::ConstIterator m_Iter;
	char32_t m_CurentChar;
	SourceLoc m_Loc;

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