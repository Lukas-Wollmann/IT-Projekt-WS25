#include "Lexer.h"
#include <algorithm>

Lexer::Lexer(const std::string &source)
    : src(source), loc{1, 1, 0} {
    // Initialize sets for keywords, operators, and separators
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        char currentChar = this->current();
        SourceLoc tokenLoc = loc;

        if (isdigit(currentChar)) {
            tokens.push_back(lexNumber(tokenLoc));
        } else if (currentChar == '"') {
            tokens.push_back(lexString(tokenLoc));
        } else if (currentChar == '\'') {
            tokens.push_back(lexChar(tokenLoc));
        } else if (isSeparator(currentChar)) {
            std::string sepLexeme(1, advance());
            tokens.push_back(Token(TokenType::SEPERATOR, sepLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index));
        } else if (isOperator(currentChar)) {
            tokens.push_back(lexOperator(tokenLoc));
        } else if (isalpha(currentChar) || currentChar == '_') {
            tokens.push_back(lexIdentifierOrKeyword(tokenLoc));
        } else {
            // Unknown character, skip it for now, implement UTF-8 support later
            advance();
        }
    }
    return tokens;
}

bool Lexer::isOperator(char c) const {
    return (std::find(singleOps.begin(), singleOps.end(), std::string(1, c)) != singleOps.end() || 
            std::find(multiOps.begin(), multiOps.end(), std::string(1, c) + peek()) != multiOps.end());
}

bool Lexer::isSeparator(char c) const {
    return std::find(separators.begin(), separators.end(), c) != separators.end();
}

bool Lexer::isAtEnd() const {
    return loc.index >= src.length();
}

char Lexer::advance() {
    loc.index++;
    loc.column++;
    return src[loc.index - 1];
}

char Lexer::current() const {
    if (isAtEnd()) return '\0';
    return src[loc.index];
}

char Lexer::peek() const {
    if (loc.index + 1 >= src.length()) return '\0';
    return src[loc.index + 1];
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char currentChar = this->current();
        if (currentChar == ' ' || currentChar == '\t' || currentChar == '\r') {
            advance();
        } else if (currentChar == '\n') {
            loc.line++;
            loc.column = 1;
            advance();
        }
        return;
    }
}

Token Lexer::lexNumber(SourceLoc tokenLoc) {
    size_t start = loc.index;
    while(isdigit(current())) {
        advance();
    }
    std::string numberLexeme = src.substr(start, loc.index - start);
    return Token(TokenType::NUMERIC_LITERAL, numberLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
}

Token Lexer::lexString(SourceLoc tokenLoc) {
    advance(); // Skip opening quote
    size_t start = loc.index;
    while (current() != '"' && !isAtEnd()) {
        advance();
    }
    std::string stringLexeme = src.substr(start, loc.index - start);
    advance(); // Skip closing quote
    return Token(TokenType::STRING_LITERAL, stringLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
}

Token Lexer::lexChar(SourceLoc tokenLoc) {
    advance(); // Skip opening quote
    char charValue = advance(); // Get character
    advance(); // Skip closing quote
    std::string charLexeme(1, charValue);
    return Token(TokenType::CHAR_LITERAL, charLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
}

Token Lexer::lexOperator(SourceLoc tokenLoc) {
    char firstChar = advance();
    char secondChar = current();

    std::string opLexeme(1, firstChar);
    std::string twoCharOp = opLexeme + secondChar;

    if (std::find(multiOps.begin(), multiOps.end(), twoCharOp) != multiOps.end()) {
        advance(); // Consume second character
        return Token(TokenType::OPERATOR, twoCharOp, tokenLoc.line, tokenLoc.column, tokenLoc.index);
    }
    return Token(TokenType::OPERATOR, opLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
    
}

Token Lexer::lexIdentifierOrKeyword(SourceLoc tokenLoc) {
    size_t start = loc.index;
    while (isalnum(current()) || current() == '_') {
        advance();
    }
    std::string identLexeme = src.substr(start, loc.index - start);
    if (std::find(keywords.begin(), keywords.end(), identLexeme) != keywords.end()) {
        return Token(TokenType::KEYWORD, identLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
    }
    return Token(TokenType::IDENTIFIER, identLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
}