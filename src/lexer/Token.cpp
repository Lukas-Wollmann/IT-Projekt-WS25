#include "Token.h"

#include <format>

std::ostream &operator<<(std::ostream &os, TokenType type) {
	switch (type) {
		case TokenType::IDENTIFIER:		 return os << "IDENTIFIER";
		case TokenType::STRING_LITERAL:	 return os << "STRING_LITERAL";
		case TokenType::NUMERIC_LITERAL: return os << "NUMERIC_LITERAL";
		case TokenType::CHAR_LITERAL:	 return os << "CHAR_LITERAL";
		case TokenType::KEYWORD:		 return os << "KEYWORD";
		case TokenType::OPERATOR:		 return os << "OPERATOR";
		case TokenType::SEPARATOR:		 return os << "SEPARATOR";
		case TokenType::COMMENT:		 return os << "COMMENT";
		case TokenType::ILLEGAL:		 return os << "ILLEGAL";
	}

	return os << "<UnknownTokenType>";
}

std::ostream &operator<<(std::ostream &os, ErrorTypeToken type) {
	switch (type) {
		case ErrorTypeToken::NOT_ILLEGAL:		  return os << "NOT_ILLEGAL";
		case ErrorTypeToken::UNTERMINATED_STRING: return os << "UNTERMINATED_STRING";
		case ErrorTypeToken::SOLO_BACKSLASH_IN_CHAR_LITERAL:
			return os << "SOLO_BACKSLASH_IN_CHAR_LITERAL";
		case ErrorTypeToken::INVALID_ESCAPE_SEQUENCE: return os << "INVALID_ESCAPE_SEQUENCE";
		case ErrorTypeToken::MULTIPLE_CHAR_IN_CHAR_LITERAL:
			return os << "MULTIPLE_CHAR_IN_CHAR_LITERAL";
		case ErrorTypeToken::UNTERMINATED_CHAR_LITERAL:	 return os << "UNTERMINATED_CHAR_LITERAL";
		case ErrorTypeToken::EMPTY_CHAR_LITERAL:		 return os << "EMPTY_CHAR_LITERAL";
		case ErrorTypeToken::UNTERMINATED_BLOCK_COMMENT: return os << "UNTERMINATED_BLOCK_COMMENT";
		case ErrorTypeToken::ILLEGAL_IDENTIFIER:		 return os << "ILLEGAL_IDENTIFIER";
	}
	return os << "<UnknownIllegalType>";
}

std::ostream &operator<<(std::ostream &os, const Token &t) {
	return os << "Token(" << t.type << ", " << t.lexeme << ", " << t.loc.line << ", "
			  << t.loc.column << ", " << t.loc.index << ", " << t.errorType << ")";
}

bool operator==(const Token &lhs, const Token &rhs) {
	// Compare type, lexeme, and all fields inside loc
	return lhs.type == rhs.type && lhs.lexeme == rhs.lexeme && lhs.loc.line == rhs.loc.line &&
		   lhs.loc.column == rhs.loc.column && lhs.loc.index == rhs.loc.index;
}

bool operator!=(const Token &lhs, const Token &rhs) {
	return !(lhs == rhs);
}