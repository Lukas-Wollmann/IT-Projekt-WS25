#include "Operator.h"

#include <sstream>

#include "ast/AST.h"
#include "type/Type.h"

namespace semantic {
	using namespace ast;
	using namespace type;

	void addInternalBinaryOperator(Namespace &ns, BinaryOpKind op, const U8String &typename_) {
		Params params;
		params.push_back(std::make_unique<Typename>(typename_));
		params.push_back(std::make_unique<Typename>(typename_));

		auto retType = std::make_unique<Typename>(typename_);
		auto func = std::make_unique<FunctionType>(std::move(params), std::move(retType));

		std::stringstream ss;
		ss << "operator" << op << "<" << typename_ << "," << typename_ << ">";

		ns.addFunction(U8String(ss.str()), std::move(func));
	}

	void addInternalUnaryOperator(Namespace &ns, UnaryOpKind op, const U8String &typename_) {
		Params params;
		params.push_back(std::make_unique<Typename>(typename_));

		auto retType = std::make_unique<Typename>(typename_);
		auto func = std::make_unique<FunctionType>(std::move(params), std::move(retType));

		std::stringstream ss;
		ss << "operator" << op << "<" << typename_ << ">";

		ns.addFunction(U8String(ss.str()), std::move(func));
	}

	void addInternalOperatorDecls(Namespace &ns) {
		using enum BinaryOpKind;
		using enum UnaryOpKind;

		// Arithmetic (Binary)
		addInternalBinaryOperator(ns, Addition, u8"i32");
		addInternalBinaryOperator(ns, Addition, u8"u32");
		addInternalBinaryOperator(ns, Addition, u8"f32");
		addInternalBinaryOperator(ns, Addition, u8"string");

		addInternalBinaryOperator(ns, Subtraction, u8"i32");
		addInternalBinaryOperator(ns, Subtraction, u8"u32");
		addInternalBinaryOperator(ns, Subtraction, u8"f32");

		addInternalBinaryOperator(ns, Multiplication, u8"i32");
		addInternalBinaryOperator(ns, Multiplication, u8"u32");
		addInternalBinaryOperator(ns, Multiplication, u8"f32");

		addInternalBinaryOperator(ns, Division, u8"i32");
		addInternalBinaryOperator(ns, Division, u8"u32");
		addInternalBinaryOperator(ns, Division, u8"f32");

		addInternalBinaryOperator(ns, Modulo, u8"i32");
		addInternalBinaryOperator(ns, Modulo, u8"u32");

		// Comparison (Binary)
		addInternalBinaryOperator(ns, Equality, u8"i32");
		addInternalBinaryOperator(ns, Equality, u8"u32");
		addInternalBinaryOperator(ns, Equality, u8"f32");
		addInternalBinaryOperator(ns, Equality, u8"bool");
		addInternalBinaryOperator(ns, Equality, u8"char");
		addInternalBinaryOperator(ns, Equality, u8"string");

		addInternalBinaryOperator(ns, Inequality, u8"i32");
		addInternalBinaryOperator(ns, Inequality, u8"u32");
		addInternalBinaryOperator(ns, Inequality, u8"f32");
		addInternalBinaryOperator(ns, Inequality, u8"bool");
		addInternalBinaryOperator(ns, Inequality, u8"char");
		addInternalBinaryOperator(ns, Inequality, u8"string");

		addInternalBinaryOperator(ns, LessThan, u8"i32");
		addInternalBinaryOperator(ns, LessThan, u8"u32");
		addInternalBinaryOperator(ns, LessThan, u8"f32");
		addInternalBinaryOperator(ns, LessThan, u8"char");
		addInternalBinaryOperator(ns, LessThan, u8"string");

		addInternalBinaryOperator(ns, GreaterThan, u8"i32");
		addInternalBinaryOperator(ns, GreaterThan, u8"u32");
		addInternalBinaryOperator(ns, GreaterThan, u8"f32");
		addInternalBinaryOperator(ns, GreaterThan, u8"char");
		addInternalBinaryOperator(ns, GreaterThan, u8"string");

		addInternalBinaryOperator(ns, GreaterThanOrEqual, u8"i32");
		addInternalBinaryOperator(ns, GreaterThanOrEqual, u8"u32");
		addInternalBinaryOperator(ns, GreaterThanOrEqual, u8"f32");
		addInternalBinaryOperator(ns, GreaterThanOrEqual, u8"char");
		addInternalBinaryOperator(ns, GreaterThanOrEqual, u8"string");

		// Logical (Binary)
		addInternalBinaryOperator(ns, LogicalAnd, u8"bool");
		addInternalBinaryOperator(ns, LogicalOr, u8"bool");

		// Bitwise (Binary)
		addInternalBinaryOperator(ns, BitwiseAnd, u8"i32");
		addInternalBinaryOperator(ns, BitwiseAnd, u8"u32");

		addInternalBinaryOperator(ns, BitwiseOr, u8"i32");
		addInternalBinaryOperator(ns, BitwiseOr, u8"u32");

		addInternalBinaryOperator(ns, BitwiseXor, u8"i32");
		addInternalBinaryOperator(ns, BitwiseXor, u8"u32");

		addInternalBinaryOperator(ns, LeftShift, u8"i32");
		addInternalBinaryOperator(ns, LeftShift, u8"u32");

		addInternalBinaryOperator(ns, RightShift, u8"i32");
		addInternalBinaryOperator(ns, RightShift, u8"u32");

		// Arithmetic (Unary)
		addInternalUnaryOperator(ns, Positive, u8"i32");
		addInternalUnaryOperator(ns, Positive, u8"u32");
		addInternalUnaryOperator(ns, Positive, u8"f32");

		addInternalUnaryOperator(ns, Negative, u8"i32");
		addInternalUnaryOperator(ns, Negative, u8"f32");

		// Logical (Unary)
		addInternalUnaryOperator(ns, Not, u8"bool");

		// Bitwise (Unary)
		addInternalUnaryOperator(ns, Not, u8"i32");
		addInternalUnaryOperator(ns, Not, u8"u32");

		// Dereference is implemented separatly in the typechecker.
    }
}