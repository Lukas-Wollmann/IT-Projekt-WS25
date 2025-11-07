#pragma once
#include <string>
#include <vector>
#include <array>
#include "Token.h"
#include "core/U8String.h"

class Lexer {
private:
    static constexpr std::array<char32_t, 9> s_Separators = {U';', U',', U'(', U')', U'{', U'}', U'[', U']', U':'};
    static inline const std::array<U8String, 11> s_Keywords = {u8"if", u8"else", u8"while", u8"return", u8"func", u8"i32", u8"u32", u8"f32", u8"string", u8"char", u8"bool"};
    static inline const std::array<U8String, 13> s_SingleOps = {u8"+", u8"-", u8"*", u8"/", u8"=", u8"!", u8"<", u8">", u8"&", u8"|", u8"^", u8"%", u8"~"};
    static inline const std::array<U8String, 18> s_MultiOps = {u8"&&", u8"||", u8"==", u8"<=", u8">=", u8"!=", u8"<<", u8">>", u8"+=", u8"-=", u8"*=", u8"/=", u8"%=", u8"^=", u8"&=", u8"|=", u8"<<=", u8">>="};

public:
    explicit Lexer(const U8String &source);
    std::vector<Token> tokenize();

private:
    const U8String &m_Src;
    U8String::ConstIterator m_Iter;
    char32_t m_CurentChar;
    SourceLoc m_Loc;

    void advance();
    bool isCurrentSingleOperator() const;
    bool isCurrentMultiOperator() const;
    bool isCurrentSeparator() const;
    bool isKeyword(const U8String &lexeme) const;
    bool isCurrentComment() const;
    bool isStartBlockComment() const;
    bool isAtEnd() const;
    char32_t peek() const;
    void skipWhitespace();

    Token lexNumber(SourceLoc startLoc);
    Token lexString(SourceLoc startLoc);
    Token lexChar(SourceLoc startLoc);
    Token lexSeparator(SourceLoc startLoc);
    Token lexOperator(SourceLoc startLoc);
    Token lexIdentifierOrKeyword(SourceLoc startLoc);
    Token lexComment(SourceLoc startLoc);
    Token lexBlockComment(SourceLoc startLoc);
};