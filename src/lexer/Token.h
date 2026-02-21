#pragma once
#include "core/Macros.h"
#include "core/SourceLoc.h"
#include "core/U8String.h"

namespace lex {
enum struct TokenType {
	Identifier,
	StringLiteral,
	IntLiteral,
	CharLiteral,
	BoolLiteral,
	Keyword,
	Operator,
	Separator,
	Comment,
	Illegal,
	EndOfFile
};

struct Token {
	TokenType type;
	U8String lexeme;
	SourceLoc loc;

	explicit Token(TokenType type, U8String = u8"", const SourceLoc &loc = {});

	[[nodiscard]] bool matches(TokenType otherType, const U8String &otherLexeme) const;
	[[nodiscard]] bool matches(TokenType otherType) const;

	bool operator==(const Token &other) const;
	bool operator!=(const Token &other) const;
};
}

template <>
struct std::formatter<lex::TokenType> {
	constexpr auto parse(const format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const lex::TokenType type, format_context &ctx) const {
		using enum lex::TokenType;

		std::u8string_view out;

		switch (type) {
			case Identifier:	out = u8"Identifier"; break;
			case StringLiteral: out = u8"StringLiteral"; break;
			case IntLiteral:	out = u8"IntLiteral"; break;
			case CharLiteral:	out = u8"CharLiteral"; break;
			case BoolLiteral:	out = u8"BoolLiteral"; break;
			case Keyword:		out = u8"Keyword"; break;
			case Operator:		out = u8"Operator"; break;
			case Separator:		out = u8"Separator"; break;
			case Comment:		out = u8"Comment"; break;
			case Illegal:		out = u8"Illegal"; break;
			case EndOfFile:		out = u8"EndOfFile"; break;
			default:			UNREACHABLE();
		}

		return std::copy(out.begin(), out.end(), ctx.out());
	}
};

template <>
struct std::formatter<lex::Token> {
	bool debug = false;

	constexpr auto parse(const format_parse_context &ctx) {
		auto it = ctx.begin();
		const auto end = ctx.end();

		if (it != end && *it == '?') {
			debug = true;
			++it;
		}

		if (it != end && *it != '}')
			throw std::format_error("Invalid format specifier for lexer::Token");

		return it;
	}

	auto format(const lex::Token &token, format_context &ctx) const {
		if (!debug) {
			if (token.lexeme.empty())
				return std::format_to(ctx.out(), "{}", token.type);

			return std::format_to(ctx.out(), "'{}'", token.lexeme);
		}

		return std::format_to(ctx.out(), "Token({}, \"{}\", {})", token.type, token.lexeme,
							  token.loc);
	}
};