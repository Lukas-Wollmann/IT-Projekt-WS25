#pragma once
#include <string>
#include "Token.h"
#include <vector>
#include <array>

class Lexer {
public:
    explicit Lexer(const std::string &source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t line, column, index;

    static constexpr std::array<char, 10> separators = {';', ',', '(', ')', '{', '}', '[', ']', ':'};
    static constexpr std::array<std::string, 11> keywords = {"if", "else", "while", "return", "func", "i32", "u32", "f32", "string", "char", "bool"};
    static constexpr std::array<std::string, 13> singleOps = {"+", "-", "*", "/", "=", "!", "<", ">", "&", "|", "^", "%", "~"};
    static constexpr std::array<std::string, 18> multiOps = {"&&", "||", "==", "<=", ">=", "!=", "<<", ">>", "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<=", ">>="};

    bool isAtEnd() const;
    char advance();
    char current() const;
    char peek() const;
    void skipWhitespace();

    Token lexNumber(size_t ln, size_t col, size_t idx);
    Token lexString(size_t ln, size_t col, size_t idx);
    Token lexChar(size_t ln, size_t col, size_t idx);
    Token lexOperator(size_t ln, size_t col, size_t idx);
    Token lexIdentifierOrKeyword(size_t ln, size_t col, size_t idx);
};