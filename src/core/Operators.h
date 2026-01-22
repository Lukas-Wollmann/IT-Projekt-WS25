#pragma once

#include "Macros.h"

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

std::ostream &operator<<(std::ostream &os, UnaryOpKind kind);
std::ostream &operator<<(std::ostream &os, BinaryOpKind kind);