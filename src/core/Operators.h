#pragma once

#include "Macros.h"
#include "U8String.h"

enum struct BinaryOpKind {
	Addition,
	Subtraction,
	Multiplication,
	Division,
	Modulo,
	Equality,
	Inequality,
	LessThan,
	GreaterThan,
	LessThanOrEqual,
	GreaterThanOrEqual,
	LogicalAnd,
	LogicalOr,
	BitwiseAnd,
	BitwiseOr,
	BitwiseXor,
	LeftShift,
	RightShift
};

enum struct UnaryOpKind { LogicalNot, BitwiseNot, Positive, Negative, Dereference };

enum struct AssignmentKind {
	Simple,
	Addition,
	Subtraction,
	Multiplication,
	Division,
	Modulo,
	BitwiseAnd,
	BitwiseOr,
	BitwiseXor,
	LeftShift,
	RightShift,
};

template <>
struct std::formatter<BinaryOpKind> {
	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(BinaryOpKind kind, format_context &ctx) const {
		using enum BinaryOpKind;

		std::u8string_view out;

		switch (kind) {
			case Addition:			 out = u8"+"; break;
			case Subtraction:		 out = u8"-"; break;
			case Multiplication:	 out = u8"*"; break;
			case Division:			 out = u8"/"; break;
			case Modulo:			 out = u8"%"; break;
			case Equality:			 out = u8"=="; break;
			case Inequality:		 out = u8"!="; break;
			case LessThan:			 out = u8"<"; break;
			case GreaterThan:		 out = u8">"; break;
			case LessThanOrEqual:	 out = u8"<="; break;
			case GreaterThanOrEqual: out = u8">="; break;
			case LogicalAnd:		 out = u8"&&"; break;
			case LogicalOr:			 out = u8"||"; break;
			case BitwiseAnd:		 out = u8"&"; break;
			case BitwiseOr:			 out = u8"|"; break;
			case BitwiseXor:		 out = u8"^"; break;
			case LeftShift:			 out = u8"<<"; break;
			case RightShift:		 out = u8">>"; break;
			default:				 UNREACHABLE();
		}

		return std::copy(out.begin(), out.end(), ctx.out());
	}
};

template <>
struct std::formatter<UnaryOpKind> {
	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(UnaryOpKind kind, format_context &ctx) const {
		using enum UnaryOpKind;

		std::u8string_view out;

		switch (kind) {
			case LogicalNot:  out = u8"!"; break;
			case BitwiseNot:  out = u8"~"; break;
			case Positive:	  out = u8"+"; break;
			case Negative:	  out = u8"-"; break;
			case Dereference: out = u8"*"; break;
			default:		  UNREACHABLE();
		}

		return std::copy(out.begin(), out.end(), ctx.out());
	}
};

template <>
struct std::formatter<AssignmentKind> {
	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(AssignmentKind kind, format_context &ctx) const {
		using enum AssignmentKind;

		std::u8string_view out;

		switch (kind) {
			case Simple:		 out = u8"=";
			case Addition:		 out = u8"+=";
			case Subtraction:	 out = u8"-=";
			case Multiplication: out = u8"*=";
			case Division:		 out = u8"/=";
			case Modulo:		 out = u8"%=";
			case BitwiseAnd:	 out = u8"&=";
			case BitwiseOr:		 out = u8"|=";
			case BitwiseXor:	 out = u8"^=";
			case LeftShift:		 out = u8"<<=";
			case RightShift:	 out = u8">>=";
			default:			 UNREACHABLE();
		}

		return std::copy(out.begin(), out.end(), ctx.out());
	}
};

inline std::ostream &operator<<(std::ostream &os, BinaryOpKind op) {
	return os << std::format("{}", op);
}

inline std::ostream &operator<<(std::ostream &os, UnaryOpKind op) {
	return os << std::format("{}", op);
}

inline std::ostream &operator<<(std::ostream &os, AssignmentKind op) {
	return os << std::format("{}", op);
}