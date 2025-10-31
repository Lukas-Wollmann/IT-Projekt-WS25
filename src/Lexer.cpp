#include "Lexer.h"
#include <algorithm>

Lexer::Lexer(const std::string &source)
    : m_Src(source), m_Loc{1, 1, 0} {
    // Initialize sets for keywords, operators, and separators
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        char currentChar = this->current();
        SourceLoc tokenLoc = m_Loc;

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
    return (std::find(s_SingleOps.begin(), s_SingleOps.end(), std::string(1, c)) != s_SingleOps.end() || 
            std::find(s_MultiOps.begin(), s_MultiOps.end(), std::string(1, c) + peek()) != s_MultiOps.end());
}

bool Lexer::isSeparator(char c) const {
    return std::find(s_Separators.begin(), s_Separators.end(), c) != s_Separators.end();
}

bool Lexer::isAtEnd() const {
    return m_Loc.index >= m_Src.length();
}

char Lexer::advance() {
    m_Loc.index++;
    m_Loc.column++;
    return m_Src[m_Loc.index - 1];
}

char Lexer::current() const {
    if (isAtEnd()) return '\0';
    return m_Src[m_Loc.index];
}

char Lexer::peek() const {
    if (m_Loc.index + 1 >= m_Src.length()) return '\0';
    return m_Src[m_Loc.index + 1];
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char currentChar = this->current();
        if (currentChar == ' ' || currentChar == '\t' || currentChar == '\r') {
            advance();
        } else if (currentChar == '\n') {
            m_Loc.line++;
            m_Loc.column = 1;
            advance();
        }
        return;
    }
}

Token Lexer::lexNumber(SourceLoc tokenLoc) {
    size_t start = m_Loc.index;
    while(isdigit(current())) {
        advance();
    }
    std::string numberLexeme = m_Src.substr(start, m_Loc.index - start);
    return Token(TokenType::NUMERIC_LITERAL, numberLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
}

Token Lexer::lexString(SourceLoc tokenLoc) {
    advance(); // Skip opening quote
    size_t start = m_Loc.index;
    while (current() != '"' && !isAtEnd()) {
        advance();
    }
    std::string stringLexeme = m_Src.substr(start, m_Loc.index - start);
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

    if (std::find(s_MultiOps.begin(), s_MultiOps.end(), twoCharOp) != s_MultiOps.end()) {
        advance(); // Consume second character
        return Token(TokenType::OPERATOR, twoCharOp, tokenLoc.line, tokenLoc.column, tokenLoc.index);
    }
    return Token(TokenType::OPERATOR, opLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
    
}

Token Lexer::lexIdentifierOrKeyword(SourceLoc tokenLoc) {
    size_t start = m_Loc.index;
    while (isalnum(current()) || current() == '_') {
        advance();
    }
    std::string identLexeme = m_Src.substr(start, m_Loc.index - start);
    if (std::find(s_Keywords.begin(), s_Keywords.end(), identLexeme) != s_Keywords.end()) {
        return Token(TokenType::KEYWORD, identLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
    }
    return Token(TokenType::IDENTIFIER, identLexeme, tokenLoc.line, tokenLoc.column, tokenLoc.index);
}