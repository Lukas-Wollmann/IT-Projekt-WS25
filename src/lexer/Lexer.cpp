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
    while (true) {
        skipWhitespace();
        if (isAtEnd())  break;
        
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
        } else if (isCurrentSingleOperator() || isCurrentDoubleOperator() || isCurrentTripleOperator()) {
            tokens.push_back(lexOperator(startLoc));
        } else if (isalpha(currentChar) || currentChar == U'_') {
            tokens.push_back(lexIdentifierOrKeyword(startLoc));
        } else {
            // Unknown character, skip it for now, implement UTF-8 support later
            tokens.push_back(lexIllegal(startLoc));
        }
    }
    return tokens;
}

void Lexer::advance() {
    if (isAtEnd()) throw std::out_of_range("Cannot advance past end of source");
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

bool Lexer::isCurrentDoubleOperator() const {
    return std::find(s_DoubleOps.begin(), s_DoubleOps.end(), U8String(m_CurentChar) + peek(1)) != s_DoubleOps.end();
}

bool Lexer::isCurrentTripleOperator() const {
    return std::find(s_TripleOps.begin(), s_TripleOps.end(), U8String(m_CurentChar) + peek(1) + peek(2)) != s_TripleOps.end();
}

bool Lexer::isCurrentSeparator() const {
    return std::find(s_Separators.begin(), s_Separators.end(), m_CurentChar) != s_Separators.end();
}

bool Lexer::isKeyword(const U8String &lexeme) const {
    return std::find(s_Keywords.begin(), s_Keywords.end(), lexeme) != s_Keywords.end();
}

bool Lexer::isCurrentComment() const {
    return m_CurentChar == U'/' && peek(1) == U'/';
}

bool Lexer::isStartBlockComment() const {
    return m_CurentChar == U'/' && peek(1) == U'*';
}

bool Lexer::isAtEnd() const {
    return m_CurentChar == U'\0';
}

char32_t Lexer::peek(size_t distance) const {
    if (distance == 0) return m_CurentChar;

    auto it = m_Iter;
    for (size_t i = 0; i < distance; ++i) {
        if (it == m_Src.end()) return U'\0';
        ++it;
    }
    return it != m_Src.end() ? *it : U'\0';
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

U8String Lexer::skipToClosing() {
        std::stringstream illegalSs;
        while (!isAtEnd() && m_CurentChar != U'\'' && m_CurentChar != U'\n') {

            illegalSs << m_CurentChar;
            advance();
        }
        if (!isAtEnd() && m_CurentChar == U'\'') advance();
        return U8String(illegalSs.str());
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
    char32_t prevChar = U'\0';
    char32_t prevprevChar = U'\0';

    while ((m_CurentChar != U'"') || (m_CurentChar == U'"' && prevChar == U'\\' && prevprevChar != U'\\')) {
        if (isAtEnd()) {
            // Unterminated string literal
            return Token(TokenType::ILLEGAL, U8String(ss.str()), startLoc, ErrorTypeToken::UNTERMINATED_STRING);
        }

        prevprevChar = prevChar;
        prevChar = m_CurentChar;
        ss << m_CurentChar;
        advance();
    }
    U8String stringLexeme(ss.str());
    advance(); // Skip closing quote
    return Token(TokenType::STRING_LITERAL, stringLexeme, startLoc);
}

Token Lexer::lexEscapedChar(SourceLoc startLoc) {
    std::stringstream rawSs;   // raw contents inside the quotes (for ILLEGAL tokens)
    std::stringstream valueSs; // actual value for CHAR_LITERAL
    rawSs << U'\\';
    advance();
    if (isAtEnd()) {
        return Token(TokenType::ILLEGAL, U8String(rawSs.str()), startLoc, ErrorTypeToken::UNTERMINATED_CHAR_LITERAL);
    }

    if(m_CurentChar == U'\'' && peek(1) != U'\'') {
        advance();
        return Token(TokenType::ILLEGAL, U8String(rawSs.str()), startLoc, ErrorTypeToken::SOLO_BACKSLASH_IN_CHAR_LITERAL);
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
        case U'"':  mapped = U'"'; break;
        default:   validEscape = false; break;
    }

    advance(); // move past escape char
    if (!validEscape) {
        U8String illegalChars = skipToClosing();
        return Token(TokenType::ILLEGAL, U8String(rawSs.str()) + illegalChars, startLoc, ErrorTypeToken::INVALID_ESCAPE_SEQUENCE);
    }

    // expect closing quote
    if (m_CurentChar != U'\'') {
        U8String illegalChars = skipToClosing();
        return Token(TokenType::ILLEGAL, U8String(rawSs.str()) + illegalChars, startLoc, ErrorTypeToken::MULTIPLE_CHAR_IN_CHAR_LITERAL);
    }
    advance(); // consume closing quote

    // return mapped char as string
    valueSs << mapped;
    return Token(TokenType::CHAR_LITERAL, U8String(valueSs.str()), startLoc);
}

//unclean have to fix later
Token Lexer::lexChar(SourceLoc startLoc) {
    advance(); // Skip opening quote
    if (isAtEnd()) {
        return Token(TokenType::ILLEGAL, U8String(u8""), startLoc, ErrorTypeToken::UNTERMINATED_CHAR_LITERAL);
    }

    std::stringstream rawSs;   // raw contents inside the quotes (for ILLEGAL tokens)
    std::stringstream valueSs; // actual value for CHAR_LITERAL

    // Escaped character
    if (m_CurentChar == U'\\') {
        return lexEscapedChar(startLoc);
    }

    // Non-escaped character
    char32_t c = m_CurentChar;

    // If the character we saw was a closing quote immediately -> empty char literal -> ILLEGAL
    if (c == U'\'') {
        advance();
        return Token(TokenType::ILLEGAL, U8String(""), startLoc, ErrorTypeToken::EMPTY_CHAR_LITERAL);
    }

    rawSs << c;
    valueSs << c;
    advance(); // move past the character

    // Expect closing quote now
    if (m_CurentChar != U'\'') {
        U8String illegalChars = skipToClosing();
        return Token(TokenType::ILLEGAL, U8String(rawSs.str()) + illegalChars, startLoc, ErrorTypeToken::MULTIPLE_CHAR_IN_CHAR_LITERAL);
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
    char32_t secondChar = peek(1);
    char32_t thirdChar = peek(2);
    advance();

    U8String opLexeme(firstChar);
    U8String twoCharOp = opLexeme + secondChar;
    U8String threeCharOp = twoCharOp + thirdChar;

    if (std::find(s_TripleOps.begin(), s_TripleOps.end(), threeCharOp) != s_TripleOps.end()) {
        advance(); // Consume second character
        advance(); // Consume third character
        return Token(TokenType::OPERATOR, threeCharOp, startLoc);
    } else if (std::find(s_DoubleOps.begin(), s_DoubleOps.end(), twoCharOp) != s_DoubleOps.end()) {
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
        if (isAtEnd() || (m_CurentChar == U'\n' && peek(1) == U'\0')) {
            return Token(TokenType::ILLEGAL, U8String(ss.str()), startLoc, ErrorTypeToken::UNTERMINATED_BLOCK_COMMENT);
        }
        if (m_CurentChar == U'*' && peek(1) == U'/') {
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

Token Lexer::lexIllegal(SourceLoc startLoc) {
    std::stringstream ss;
    ss << m_CurentChar;
    advance();
    U8String illegalLexeme(ss.str());
    return Token(TokenType::ILLEGAL, illegalLexeme, startLoc, ErrorTypeToken::ILLEGAL_IDENTIFIER);
}