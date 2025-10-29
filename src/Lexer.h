#pragma once
#include <string>
#include <vector>
#include <array>
#include "Token.h"

struct SourceLoc {
    size_t line;
    size_t column;
    size_t index;
};

class Lexer {
public:
    explicit Lexer(const std::string &source);
    std::vector<Token> tokenize();

private:
    std::string src;
    SourceLoc loc;

    static constexpr std::array<char, 9> separators = {';', ',', '(', ')', '{', '}', '[', ']', ':'};
    static inline const std::array<std::string, 11> keywords = {"if", "else", "while", "return", "func", "i32", "u32", "f32", "string", "char", "bool"};
    static inline const std::array<std::string, 13> singleOps = {"+", "-", "*", "/", "=", "!", "<", ">", "&", "|", "^", "%", "~"};
    static constexpr std::array<std::string, 18> multiOps = {"&&", "||", "==", "<=", ">=", "!=", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<=", ">>="};

    bool isOperator(char c) const;
    bool isSeparator(char c) const;
    bool isAtEnd() const;
    char advance();
    char current() const;
    char peek() const;
    void skipWhitespace();

    Token lexNumber(SourceLoc tokenLoc);
    Token lexString(SourceLoc tokenLoc);
    Token lexChar(SourceLoc tokenLoc);
    Token lexOperator(SourceLoc tokenLoc);
    Token lexIdentifierOrKeyword(SourceLoc tokenLoc);
};