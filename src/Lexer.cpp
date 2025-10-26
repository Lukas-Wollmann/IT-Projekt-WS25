#include "Lexer.h"
#include <algorithm>

Lexer::Lexer(const std::string &source)
    : src(source), line(1), column(1), index(0) {
    // Initialize sets for keywords, operators, and separators
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        char current = advance();
        // Tokenize based on the current character
    }
    return tokens;
}

bool Lexer::isAtEnd() const {
    return index >= src.length();
}

char Lexer::advance() {
    index++;
    column++;
    return src[index - 1];
}

char Lexer::current() const {
    if (isAtEnd()) return '\0';
    return src[index];
}

char Lexer::peek() const {
    if (index + 1 >= src.length()) return '\0';
    return src[index + 1];
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char current = this->current();
        if (current == ' ' || current == '\t' || current == '\r') {
            advance();
        } else if (current == '\n') {
            line++;
            column = 1;
            advance();
        }
        return;
    }
}

Token Lexer::lexNumber(size_t ln, size_t col, size_t idx) {
    size_t start = index;
    while(isdigit(current())) {
        advance();
    }
    std::string numberLexeme = src.substr(start, index - start);
    return Token(TokenType::NUMERIC_LITERAL, numberLexeme, ln, col, idx);
}

Token Lexer::lexString(size_t ln, size_t col, size_t idx) {
    advance(); // Skip opening quote
    size_t start = index;
    while (current() != '"' && !isAtEnd()) {
        if(peek() == '\n') {
            line++;
            column = 1;
        }
        advance();
    }
    std::string stringLexeme = src.substr(start, index - start);
    advance(); // Skip closing quote
    return Token(TokenType::STRING_LITERAL, stringLexeme, ln, col, idx);
}

Token Lexer::lexChar(size_t ln, size_t col, size_t idx) {
    advance(); // Skip opening quote
    char charValue = advance(); // Get character
    advance(); // Skip closing quote
    std::string charLexeme(1, charValue);
    return Token(TokenType::CHAR_LITERAL, charLexeme, ln, col, idx);
}

Token Lexer::lexOperator(size_t ln, size_t col, size_t idx) {
    char firstChar = advance();
    char secondChar = peek();

    std::string opLexeme(1, firstChar);
    std::string twoCharOp = opLexeme + secondChar;

    if (std::find(multiOps.begin(), multiOps.end(), twoCharOp) != multiOps.end()) {
        advance(); // Consume second character
        return Token(TokenType::OPERATOR, twoCharOp, ln, col, idx);
    }
    return Token(TokenType::OPERATOR, opLexeme, ln, col, idx);
    
}

Token Lexer::lexIdentifierOrKeyword(size_t ln, size_t col, size_t idx) {
    // Lexing logic for identifiers and keywords
}