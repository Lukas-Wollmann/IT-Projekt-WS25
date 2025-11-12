#include "Token.h"
#include <format>

std::ostream &operator<<(std::ostream &os, TokenType type)
{
	switch (type) 
    {
	case TokenType::IDENTIFIER:
        return os << "IDENTIFIER";
	case TokenType::STRING_LITERAL:
		return os << "STRING_LITERAL";
	case TokenType::NUMERIC_LITERAL:
		return os << "NUMERIC_LITERAL";
	case TokenType::CHAR_LITERAL:
		return os << "CHAR_LITERAL";
	case TokenType::KEYWORD:
		return os << "KEYWORD";
	case TokenType::OPERATOR:
		return os << "OPERATOR";
	case TokenType::SEPARATOR:
		return os << "SEPARATOR";
	case TokenType::COMMENT:
		return os << "COMMENT";
	case TokenType::ILLEGAL:
		return os << "ILLEGAL";
	}

    return os << "<UnknownTokenType>";
}

std::ostream &operator<<(std::ostream &os, const Token &t) 
{
    return os << "Token(" << t.type << ", " << t.lexeme << ", " << t.loc.line << ", " << t.loc.column << ", " << t.loc.index << ")";
}

bool operator==(const Token& lhs, const Token& rhs)
{
    // Compare type, lexeme, and all fields inside loc
    return lhs.type == rhs.type &&
           lhs.lexeme == rhs.lexeme &&
           lhs.loc.line == rhs.loc.line &&
           lhs.loc.column == rhs.loc.column &&
           lhs.loc.index == rhs.loc.index;
}

bool operator!=(const Token& lhs, const Token& rhs)
{
	return !(lhs == rhs);
}