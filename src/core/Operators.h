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

U8String str(UnaryOpKind kind);
U8String str(BinaryOpKind kind);
