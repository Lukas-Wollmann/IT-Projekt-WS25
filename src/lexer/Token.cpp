#include "Token.h"

#include "Macros.h"

namespace lexer {
	Token::Token(TokenType type, U8String lexeme, SourceLoc loc, TokenError error)
		: type(type)
		, lexeme(std::move(lexeme))
		, loc(loc)
		, error(error) {}

	bool operator==(const Token &left, const Token &right) {
		return left.type == right.type && left.lexeme == right.lexeme;
	}

	bool operator!=(const Token &left, const Token &right) {
		return !(left == right);
	}

	std::ostream &operator<<(std::ostream &os, TokenType type) {
		using enum TokenType;

		switch (type) {
			case Identifier:	return os << "Identifier";
			case StringLiteral: return os << "StringLiteral";
			case IntLiteral:	return os << "IntLiteral";
			case CharLiteral:	return os << "CharLiteral";
			case BoolLiteral:	return os << "BoolLiteral";
			case Keyword:		return os << "Keyword";
			case Operator:		return os << "Operator";
			case Separator:		return os << "Separator";
			case Comment:		return os << "Comment";
			case Illegal:		return os << "Illegal";
			case EndOfFile:		return os << "EndOfFile";
			default:			UNREACHABLE();
		}
	}

	std::ostream &operator<<(std::ostream &os, TokenError error) {
		using enum TokenError;

		switch (error) {
			case None:						 return os << "None";
			case UnterminatedStringLiteral:	 return os << "UnterminatedStringLiteral";
			case UnterminatedBlockComment:	 return os << "UnterminatedBlockComment";
			case UnterminatedCharLiteral:	 return os << "UnterminatedCharLiteral";
			case EmptyCharLiteral:			 return os << "EmptyCharLiteral";
			case MultipleCharsInCharLiteral: return os << "MultipleCharsInCharLiteral";
			case InvalidEscapeSequence:		 return os << "InvalidEscapeSequence";
			case IllegalIdentifier:			 return os << "IllegalIdentifier";
			default:						 UNREACHABLE();
		}
	}

	std::ostream &operator<<(std::ostream &os, const Token &token) {
		os << "Token(" << token.type << ", " << token.lexeme << ", ";
		os << token.loc << ", " << token.error << ")";
		return os;
	}
}