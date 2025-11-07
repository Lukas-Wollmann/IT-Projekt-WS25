#include "Lexer.h"
#include <algorithm>
#include <cctype>
#include <sstream>

Lexer::Lexer(const U8String &source)
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
        char32_t currentChar = m_CurentChar;
        SourceLoc startLoc = m_Loc;  // Save the starting location
        
        if (isdigit(currentChar)) {
            tokens.push_back(lexNumber(startLoc));
        } else if (currentChar == U'"') {
            tokens.push_back(lexString(startLoc));
        } else if (isCurrentComment()) {
            tokens.push_back(lexComment(startLoc));
        } else if (isStartBlockComment()) {
            tokens.push_back(lexBlockComment(startLoc));
        } else if (currentChar == U'\'') {
            tokens.push_back(lexChar(startLoc));
        } else if (isCurrentSeparator()) {
            tokens.push_back(lexSeparator(startLoc));
        } else if (isCurrentSingleOperator() || isCurrentMultiOperator()) {
            tokens.push_back(lexOperator(startLoc));
        } else if (isalpha(currentChar) || currentChar == U'_') {
            tokens.push_back(lexIdentifierOrKeyword(startLoc));
        } else {
            // Unknown character, skip it for now, implement UTF-8 support later
            advance();
        }
    }
    return tokens;
}

void Lexer::advance() {
    if (m_CurentChar == U'\n') {
        m_Loc.line++;
        m_Loc.index++;
        m_Loc.column = 1;
    }
    else {
        m_Loc.index++;
        m_Loc.column++;
    }
    m_Iter++;
    m_CurentChar = (m_Iter != m_Src.end()) ? *m_Iter : U'\0';
}

bool Lexer::isCurrentSingleOperator() const {
    return std::find(s_SingleOps.begin(), s_SingleOps.end(), U8String(m_CurentChar)) != s_SingleOps.end();
}

bool Lexer::isCurrentMultiOperator() const {
    return std::find(s_MultiOps.begin(), s_MultiOps.end(), U8String(m_CurentChar) + peek()) != s_MultiOps.end();
}

bool Lexer::isCurrentSeparator() const {
    return std::find(s_Separators.begin(), s_Separators.end(), m_CurentChar) != s_Separators.end();
}

bool Lexer::isKeyword(const U8String &lexeme) const {
    return std::find(s_Keywords.begin(), s_Keywords.end(), lexeme) != s_Keywords.end();
}

bool Lexer::isCurrentComment() const {
    return m_CurentChar == U'/' && peek() == U'/';
}

bool Lexer::isStartBlockComment() const {
    return m_CurentChar == U'/' && peek() == U'*';
}

bool Lexer::isAtEnd() const {
    return m_CurentChar == U'\0';
}

char32_t Lexer::peek() const {
    if (m_Iter == m_Src.end()) 
        return U'\0';

    auto nextIt = m_Iter;
    ++nextIt;

    return nextIt != m_Src.end() ? *nextIt : U'\0';
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        if (m_CurentChar == U' ' || m_CurentChar == U'\t' || m_CurentChar == U'\r' || m_CurentChar == U'\n') {
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
    U8String numberLexeme(ss.str());
    return Token(TokenType::NUMERIC_LITERAL, numberLexeme, startLoc);
}

Token Lexer::lexString(SourceLoc startLoc) {
    advance(); // Skip opening quote

    std::stringstream ss;

    while (m_CurentChar != U'"' && !isAtEnd()) {
        ss << m_CurentChar;
        advance();
    }
    U8String stringLexeme(ss.str());
    advance(); // Skip closing quote
    return Token(TokenType::STRING_LITERAL, stringLexeme, startLoc);
}

//unclean have to fix later
Token Lexer::lexChar(SourceLoc startLoc) {
    advance(); // Skip opening quote
    if (isAtEnd()) {
        return Token(TokenType::ILLEGAL, U8String(u8""), startLoc);
    }

    // Helper to skip to closing quote for recovery
    auto skipToClosing = [this]() {
        while (!isAtEnd() && m_CurentChar != U'\'') advance();
        if (!isAtEnd() && m_CurentChar == U'\'') advance();
    };

    std::stringstream rawSs;   // raw contents inside the quotes (for ILLEGAL tokens)
    std::stringstream valueSs; // actual value for CHAR_LITERAL

    // Escaped character
    if (m_CurentChar == U'\\') {
        rawSs << U'\\';
        advance();
        if (isAtEnd()) {
            return Token(TokenType::ILLEGAL, U8String(rawSs.str()), startLoc);
        }
        char32_t esc = m_CurentChar;
        rawSs << esc;

        char mapped = U'\0';
        bool validEscape = true;
        switch (esc) {
            case U'\\': mapped = U'\\'; break;
            case U'\'': mapped = U'\''; break;
            case U'n':  mapped = U'\n'; break;
            case U't':  mapped = U'\t'; break;
            case U'r':  mapped = U'\r'; break;
            case U'0':  mapped = U'\0'; break;
            default:   validEscape = false; break;
        }

        advance(); // move past escape char
        if (!validEscape) {
            skipToClosing();
            return Token(TokenType::ILLEGAL, U8String(rawSs.str()), startLoc);
        }

        // expect closing quote
        if (m_CurentChar != U'\'') {
            skipToClosing();
            return Token(TokenType::ILLEGAL, U8String(rawSs.str()), startLoc);
        }
        advance(); // consume closing quote

        // return mapped char as string
        valueSs << mapped;
        return Token(TokenType::CHAR_LITERAL, U8String(valueSs.str()), startLoc);
    }

    // Non-escaped character
    char32_t c = m_CurentChar;
    rawSs << c;
    valueSs << c;
    advance(); // move past the character

    // If the character we saw was a closing quote immediately -> empty char literal -> ILLEGAL
    if (c == U'\'') {
        return Token(TokenType::ILLEGAL, U8String(rawSs.str()), startLoc);
    }

    // Expect closing quote now
    if (m_CurentChar != U'\'') {
        skipToClosing();
        return Token(TokenType::ILLEGAL, U8String(rawSs.str()), startLoc);
    }

    advance(); // consume closing quote
    return Token(TokenType::CHAR_LITERAL, U8String(valueSs.str()), startLoc);
}

Token Lexer::lexSeparator(SourceLoc startLoc) {
    char32_t sepChar = m_CurentChar;
    advance();
    return Token(TokenType::SEPARATOR, U8String(sepChar), startLoc);
}

Token Lexer::lexOperator(SourceLoc startLoc) {
    char32_t firstChar = m_CurentChar;
    char32_t secondChar = peek();
    advance();

    U8String opLexeme(firstChar);
    U8String twoCharOp = opLexeme + secondChar;

    if (std::find(s_MultiOps.begin(), s_MultiOps.end(), twoCharOp) != s_MultiOps.end()) {
        advance(); // Consume second character
        return Token(TokenType::OPERATOR, twoCharOp, startLoc);
    }
    return Token(TokenType::OPERATOR, opLexeme, startLoc);
    
}

Token Lexer::lexIdentifierOrKeyword(SourceLoc startLoc) {
    std::stringstream ss;
    while (isalnum(m_CurentChar) || m_CurentChar == U'_') {
        ss << m_CurentChar;
        advance();
    }
    U8String identLexeme(ss.str());
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
    while (m_CurentChar != U'\n' && !isAtEnd()) {
        ss << m_CurentChar;
        advance();
    }
    U8String commentLexeme(ss.str());
    return Token(TokenType::COMMENT, commentLexeme, startLoc);
}

Token Lexer::lexBlockComment(SourceLoc startLoc) {
    // Assuming block comments start with '/*' and end with '*/'
    advance(); // Skip first '/'
    advance(); // Skip '*'

    std::stringstream ss;
    while (true) {
        if (isAtEnd() || (m_CurentChar == U'\n' && peek() == U'\0')) {
            return Token(TokenType::ILLEGAL, U8String(ss.str()), startLoc);
        }
        if (m_CurentChar == U'*' && peek() == U'/') {
            advance(); // Skip '*'
            advance(); // Skip '/'
            break;
        }
        ss << m_CurentChar;
        advance();
    }
    U8String commentLexeme(ss.str());
    return Token(TokenType::COMMENT, commentLexeme, startLoc);
}