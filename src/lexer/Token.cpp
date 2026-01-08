#include "Token.h"

#include "Macros.h"

namespace lexer {
	Token::Token(TokenType type, U8String lexeme, SourceLoc loc, TokenError error)
		: type(type)
		, lexeme(std::move(lexeme))
		, loc(loc)
		, error(error) {}
}

std::ostream &operator<<(std::ostream &os, lexer::TokenType type) {
	using enum lexer::TokenType;

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

std::ostream &operator<<(std::ostream &os, lexer::TokenError error) {
	using enum lexer::TokenError;

	switch (error) {
		case None:						 return os << "None";
		case UnterminatedString:		 return os << "UnterminatedString";
		case UnterminatedBlockComment:	 return os << "UnterminatedBlockComment";
		case UnterminatedCharLiteral:	 return os << "UnterminatedCharLiteral";
		case EmptyCharLiteral:			 return os << "EmptyCharLiteral";
		case MultipleCharsInCharLiteral: return os << "MultipleCharsInCharLiteral";
		case InvalidEscapeSequence:		 return os << "InvalidEscapeSequence";
		case IllegalIdentifier:			 return os << "IllegalIdentifier";
		default:						 UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, const lexer::Token &t) {
	os << "Token(" << t.type << ", " << t.lexeme << ", " << t.loc.line;
	os << ", " << t.loc.column << ", " << t.loc.index << ", " << t.error;
	return os << ")";
}

bool operator==(const lexer::Token &left, const lexer::Token &right) {
	return left.type == right.type && left.lexeme == right.lexeme;
}

bool operator!=(const lexer::Token &left, const lexer::Token &right) {
	return !(left == right);
}