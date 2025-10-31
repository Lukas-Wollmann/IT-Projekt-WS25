#include "Lexer.h"
#include <algorithm>
#include <cctype>
#include <sstream>

Lexer::Lexer(const std::string &source)
    : m_Src(source)
    , m_Iter(m_Src.begin())
    , m_CurentChar(source.empty() ? '\0' : source[0])
    , m_Loc{1, 1, 0} {
    // Initialize sets for keywords, operators, and separators
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        char currentChar = m_CurentChar;
        SourceLoc startLoc = m_Loc;  // Save the starting location
        
        if (isdigit(currentChar)) {
            tokens.push_back(lexNumber(startLoc));
        } else if (currentChar == '"') {
            tokens.push_back(lexString(startLoc));
        } else if (currentChar == '\'') {
            tokens.push_back(lexChar(startLoc));
        } else if (isSeparator(currentChar)) {
            advance();
            std::string sepLexeme(1, currentChar);
            tokens.push_back(Token(TokenType::SEPERATOR, sepLexeme, startLoc));
        } else if (isOperator(currentChar)) {
            tokens.push_back(lexOperator(startLoc));
        } else if (isalpha(currentChar) || currentChar == '_') {
            tokens.push_back(lexIdentifierOrKeyword(startLoc));
        } else {
            // Unknown character, skip it for now, implement UTF-8 support later
            advance();
        }
    }
    return tokens;
}

void Lexer::advance() {
    if (m_CurentChar == '\n') {
        m_Loc.line++;
        m_Loc.column = 1;
    }
    else {
        m_Loc.index++;
        m_Loc.column++;
    }
    m_Iter++;
    m_CurentChar = (m_Iter != m_Src.end()) ? *m_Iter : '\0';
}

bool Lexer::isOperator(char c) const {
    return (std::find(s_SingleOps.begin(), s_SingleOps.end(), std::string(1, c)) != s_SingleOps.end() || 
            std::find(s_MultiOps.begin(), s_MultiOps.end(), std::string(1, c) + peek()) != s_MultiOps.end());
}

bool Lexer::isSeparator(char c) const {
    return std::find(s_Separators.begin(), s_Separators.end(), c) != s_Separators.end();
}

bool Lexer::isAtEnd() const {
    return m_CurentChar == '\0';
}

char Lexer::peek() const {
    if (m_Loc.index + 1 >= m_Src.length()) return '\0';
    return m_Src[m_Loc.index + 1];
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        if (m_CurentChar == ' ' || m_CurentChar == '\t' || m_CurentChar == '\r' || m_CurentChar == '\n') {
            advance();
        }
        else {
            return;
        }
    }
}

Token Lexer::lexNumber(SourceLoc startLoc) {
    std::stringstream ss;
    while (isdigit(m_CurentChar)) {
        ss << m_CurentChar;
        advance();
    }
    std::string numberLexeme = ss.str();
    return Token(TokenType::NUMERIC_LITERAL, numberLexeme, startLoc);
}

Token Lexer::lexString(SourceLoc startLoc) {
    advance(); // Skip opening quote

    std::stringstream ss;

    while (m_CurentChar != '"' && !isAtEnd()) {
        ss << m_CurentChar;
        advance();
    }
    std::string stringLexeme = ss.str();
    advance(); // Skip closing quote
    return Token(TokenType::STRING_LITERAL, stringLexeme, startLoc);
}

//TODO: Handle escape sequences -> TokenType::ILLEGAL
Token Lexer::lexChar(SourceLoc startLoc) {
    advance(); // Skip opening quote
    advance();
    std::string charLexeme(1, m_CurentChar); // Get character
    advance(); // Skip closing quote
    return Token(TokenType::CHAR_LITERAL, charLexeme, startLoc);
}

Token Lexer::lexOperator(SourceLoc startLoc) {
    char firstChar = m_CurentChar;
    char secondChar = peek();
    advance();

    std::string opLexeme(1, firstChar);
    std::string twoCharOp = opLexeme + secondChar;

    if (std::find(s_MultiOps.begin(), s_MultiOps.end(), twoCharOp) != s_MultiOps.end()) {
        advance(); // Consume second character
        return Token(TokenType::OPERATOR, twoCharOp, startLoc);
    }
    return Token(TokenType::OPERATOR, opLexeme, startLoc);
    
}

Token Lexer::lexIdentifierOrKeyword(SourceLoc startLoc) {
    std::stringstream ss;
    while (isalnum(m_CurentChar) || m_CurentChar == '_') {
        ss << m_CurentChar;
        advance();
    }
    std::string identLexeme = ss.str();
    if (std::find(s_Keywords.begin(), s_Keywords.end(), identLexeme) != s_Keywords.end()) {
        return Token(TokenType::KEYWORD, identLexeme, startLoc);
    }
    return Token(TokenType::IDENTIFIER, identLexeme, startLoc);
}