#pragma once
#include <string>
#include <vector>
#include <array>
#include "Token.h"

class Lexer {
private:
    static constexpr std::array<char, 9> s_Separators = {';', ',', '(', ')', '{', '}', '[', ']', ':'};
    static inline const std::array<std::string, 11> s_Keywords = {"if", "else", "while", "return", "func", "i32", "u32", "f32", "string", "char", "bool"};
    static inline const std::array<std::string, 13> s_SingleOps = {"+", "-", "*", "/", "=", "!", "<", ">", "&", "|", "^", "%", "~"};
    static inline const std::array<std::string, 18> s_MultiOps = {"&&", "||", "==", "<=", ">=", "!=", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<=", ">>="};

public:
    explicit Lexer(const std::string &source);
    std::vector<Token> tokenize();

private:
    const std::string &m_Src;
    std::string::const_iterator m_Iter;
    char m_CurentChar;
    SourceLoc m_Loc;

    void advance();
    bool isCurrentSingleOperator() const;
    bool isCurrentMultiOperator() const;
    bool isCurrentSeparator() const;
    bool isKeyword(const std::string &lexeme) const;
    bool isCurrentComment() const;
    bool isStartBlockComment() const;
    bool isAtEnd() const;
    char peek() const;
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