#pragma once
#include <string>
#include "Token.h"
#include <vector>
#include <unordered_set>

class Lexer {
public:
    explicit Lexer(const std::string &source);
    std::vector<Token> createToken();

private:
    std::string src;
    size_t line, column, index;

    std::unordered_set<std::string> identifiers;
    std::unordered_set<char> separators;
    std::unordered_set<std::string> keywords;
    std::unordered_set<std::string> Operators;

    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();

    Token makeToken(TokenType type, const std::string &lexeme, size_t ln, size_t col, size_t idx);
    Token lexNumber(size_t ln, size_t col, size_t idx);
    Token lexString(size_t ln, size_t col, size_t idx);
    Token lexChar(size_t ln, size_t col, size_t idx);
    Token lexoperator(size_t ln, size_t col, size_t idx);
    Token lexIdentifierorKeyword(size_t ln, size_t col, size_t idx);
}