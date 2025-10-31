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
	case TokenType::SEPERATOR:
		return os << "SEPERATOR";
	}

    return os << "<UnknownTokenType>";
}

std::ostream &operator<<(std::ostream &os, const Token &t) 
{
    return os << "Token(" << t.type << ", " << t.lexeme << ", " << t.loc.line << ", " << t.loc.column << ", " << t.loc.index << ")";
}
