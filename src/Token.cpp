#include "Token.h"

#include <format>

std::string tokenTypeToString(const TokenType type) {
	switch (type) {
	case TokenType::IDENTIFIER:
		return "IDENTIFIER";
	case TokenType::STRING_LITERAL:
		return "STRING_LITERAL";
	case TokenType::NUMERIC_LITERAL:
		return "NUMERIC_LITERAL";
	case TokenType::CHAR_LITERAL:
		return "CHAR_LITERAL";
	case TokenType::KEYWORD:
		return "KEYWORD";
	case TokenType::OPERATOR:
		return "OPERATOR";
	case TokenType::SEPERATOR:
		return "SEPERATOR";
	default:
		return "NONE";
	}
}

std::ostream &operator<<(std::ostream &os, const Token &t) {
	return os << std::format("Token({}, {}, {}, {}, {})", tokenTypeToString(t.type), t.lexeme, t.line, t.column, t.index);
}
