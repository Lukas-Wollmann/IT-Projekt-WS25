#pragma once
#include "core/Macros.h"
#include "core/SourceLoc.h"
#include "core/U8String.h"

namespace lexer {
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

		Token(TokenType type, U8String = u8"", const SourceLoc &loc = {0, 0, 0, 0});

		bool matches(TokenType otherType, U8String otherLexeme) const;
		bool matches(TokenType otherType) const;
	};

	bool operator==(const Token &left, const Token &right);
	bool operator!=(const Token &left, const Token &right);
}

template <>
struct std::formatter<lexer::TokenType> {
	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(lexer::TokenType type, format_context &ctx) const {
		using enum lexer::TokenType;

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
struct std::formatter<lexer::Token> {
	bool debug = false;

	constexpr auto parse(format_parse_context &ctx) {
		auto it = ctx.begin();
		auto end = ctx.end();

		if (it != end && *it == '?') {
			debug = true;
			++it;
		}

		if (it != end && *it != '}')
			throw std::format_error("Invalid format specifier for lexer::Token");

		return it;
	}

	auto format(const lexer::Token &token, format_context &ctx) const {
		if (!debug) {
			if (token.lexeme == u8"")
				return std::format_to(ctx.out(), "{}", token.type);

			return std::format_to(ctx.out(), "'{}'", token.lexeme);
		}

		return std::format_to(ctx.out(), "Token({}, \"{}\", {})", token.type, token.lexeme,
							  token.loc);
	}
};