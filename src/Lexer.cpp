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
        } else if (isCurrentComment()) {
            tokens.push_back(lexComment(startLoc));
        } else if (isStartBlockComment()) {
            tokens.push_back(lexBlockComment(startLoc));
        } else if (currentChar == '\'') {
            tokens.push_back(lexChar(startLoc));
        } else if (isCurrentSeparator()) {
            tokens.push_back(lexSeparator(startLoc));
        } else if (isCurrentSingleOperator() || isCurrentMultiOperator()) {
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
        m_Loc.index++;
        m_Loc.column = 1;
    }
    else {
        m_Loc.index++;
        m_Loc.column++;
    }
    m_Iter++;
    m_CurentChar = (m_Iter != m_Src.end()) ? *m_Iter : '\0';
}

bool Lexer::isCurrentSingleOperator() const {
    return std::find(s_SingleOps.begin(), s_SingleOps.end(), std::string(1, m_CurentChar)) != s_SingleOps.end();
}

bool Lexer::isCurrentMultiOperator() const {
    return std::find(s_MultiOps.begin(), s_MultiOps.end(), std::string(1, m_CurentChar) + peek()) != s_MultiOps.end();
}

bool Lexer::isCurrentSeparator() const {
    return std::find(s_Separators.begin(), s_Separators.end(), m_CurentChar) != s_Separators.end();
}

bool Lexer::isKeyword(const std::string &lexeme) const {
    return std::find(s_Keywords.begin(), s_Keywords.end(), lexeme) != s_Keywords.end();
}

bool Lexer::isCurrentComment() const {
    return m_CurentChar == '/' && peek() == '/';
}

bool Lexer::isStartBlockComment() const {
    return m_CurentChar == '/' && peek() == '*';
}

bool Lexer::isAtEnd() const {
    return m_CurentChar == '\0';
}

char Lexer::peek() const {
    if (m_Iter == m_Src.end()) 
        return '\0';

    auto nextIt = m_Iter;
    ++nextIt;

    return nextIt != m_Src.end() ? *nextIt : '\0';
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

//unclean have to fix later
Token Lexer::lexChar(SourceLoc startLoc) {
    advance(); // Skip opening quote
    if (isAtEnd()) {
        return Token(TokenType::ILLEGAL, "", startLoc);
    }

    // Helper to skip to closing quote for recovery
    auto skipToClosing = [this]() {
        while (!isAtEnd() && m_CurentChar != '\'') advance();
        if (!isAtEnd() && m_CurentChar == '\'') advance();
    };

    std::string raw; // raw contents inside the quotes (for ILLEGAL tokens)
    // Escaped character
    if (m_CurentChar == '\\') {
        raw += '\\';
        advance();
        if (isAtEnd()) {
            return Token(TokenType::ILLEGAL, raw, startLoc);
        }
        char esc = m_CurentChar;
        raw += esc;

        //mit Optional could be cleaner
        char mapped = '\0';
        bool validEscape = true;
        switch (esc) {
            case '\\': mapped = '\\'; break;
            case '\'': mapped = '\''; break;
            case 'n':  mapped = '\n'; break;
            case 't':  mapped = '\t'; break;
            case 'r':  mapped = '\r'; break;
            case '0':  mapped = '\0'; break;
            default:   validEscape = false; break;
        }

        advance(); // move past escape char
        if (!validEscape) {
            // unknown escape -> illegal, skip to closing for recovery
            skipToClosing();
            return Token(TokenType::ILLEGAL, raw, startLoc);
        }

        // expect closing quote
        if (m_CurentChar != '\'') {
            skipToClosing();
            return Token(TokenType::ILLEGAL, raw, startLoc);
        }
        advance(); // consume closing quote
        return Token(TokenType::CHAR_LITERAL, std::string(1, mapped), startLoc);
    }

    // Non-escaped character
    char c = m_CurentChar;
    raw += c;
    advance(); // move past the character

    // If the character we saw was a closing quote immediately -> empty char literal -> ILLEGAL
    if (c == '\'') {
        // we already consumed that closing quote above
        return Token(TokenType::ILLEGAL, raw, startLoc);
    }

    // Expect closing quote now
    if (m_CurentChar != '\'') {
        // either multi-char char literal or missing closing quote -> ILLEGAL
        skipToClosing();
        return Token(TokenType::ILLEGAL, raw, startLoc);
    }

    advance(); // consume closing quote
    return Token(TokenType::CHAR_LITERAL, std::string(1, c), startLoc);
}

Token Lexer::lexSeparator(SourceLoc startLoc) {
    char sepChar = m_CurentChar;
    advance();
    return Token(TokenType::SEPARATOR, std::string(1, sepChar), startLoc);
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
    if (isKeyword(identLexeme)) {
        return Token(TokenType::KEYWORD, identLexeme, startLoc);
    }
    return Token(TokenType::IDENTIFIER, identLexeme, startLoc);
}

Token Lexer::lexComment(SourceLoc startLoc) {
    // Assuming comments start with '//' and go to the end of the line
    advance(); // Skip first '/'
    advance(); // Skip second '/'

    std::stringstream ss;
    while (m_CurentChar != '\n' && !isAtEnd()) {
        ss << m_CurentChar;
        advance();
    }
    std::string commentLexeme = ss.str();
    return Token(TokenType::COMMENT, commentLexeme, startLoc);
}

Token Lexer::lexBlockComment(SourceLoc startLoc) {
    // Assuming block comments start with '/*' and end with '*/'
    advance(); // Skip first '/'
    advance(); // Skip '*'

    std::stringstream ss;
    while (true) {
        if (isAtEnd() || (m_CurentChar == '\n' && peek() == '\0')) {
            return Token(TokenType::ILLEGAL, ss.str(), startLoc);
        }
        if (m_CurentChar == '*' && peek() == '/') {
            advance(); // Skip '*'
            advance(); // Skip '/'
            break;
        }
        ss << m_CurentChar;
        advance();
    }
    std::string commentLexeme = ss.str();
    return Token(TokenType::COMMENT, commentLexeme, startLoc);
}