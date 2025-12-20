#include "OperatorTable.h"

#include <sstream>

#include "Macros.h"
#include "ast/AST.h"
#include "type/CompareVisitor.h"
#include "type/Type.h"

namespace semantic {
	using namespace ast;
	using namespace type;

	OperatorTable::OperatorTable() {
		using enum UnaryOpKind;
		using enum BinaryOpKind;

		// ARITHMETIC BINARY OPERATORS
		addBinaryOperator(u8"i32", Addition, u8"i32", u8"i32");
		addBinaryOperator(u8"u32", Addition, u8"u32", u8"u32");
		addBinaryOperator(u8"f32", Addition, u8"f32", u8"f32");
		addBinaryOperator(u8"string", Addition, u8"string", u8"string");

		addBinaryOperator(u8"i32", Subtraction, u8"i32", u8"i32");
		addBinaryOperator(u8"u32", Subtraction, u8"u32", u8"u32");
		addBinaryOperator(u8"f32", Subtraction, u8"f32", u8"f32");

		addBinaryOperator(u8"i32", Multiplication, u8"i32", u8"i32");
		addBinaryOperator(u8"u32", Multiplication, u8"u32", u8"u32");
		addBinaryOperator(u8"f32", Multiplication, u8"f32", u8"f32");

		addBinaryOperator(u8"i32", Division, u8"i32", u8"i32");
		addBinaryOperator(u8"u32", Division, u8"u32", u8"u32");
		addBinaryOperator(u8"f32", Division, u8"f32", u8"f32");

		addBinaryOperator(u8"i32", Modulo, u8"i32", u8"i32");
		addBinaryOperator(u8"u32", Modulo, u8"u32", u8"u32");

		// COMPARISON BINARY OPERATORS
		addBinaryOperator(u8"i32", Equality, u8"i32", u8"bool");
		addBinaryOperator(u8"i32", Inequality, u8"i32", u8"bool");
		addBinaryOperator(u8"i32", LessThan, u8"i32", u8"bool");
		addBinaryOperator(u8"i32", GreaterThan, u8"i32", u8"bool");
		addBinaryOperator(u8"i32", GreaterThanOrEqual, u8"i32", u8"bool");

		// u32
		addBinaryOperator(u8"u32", Equality, u8"u32", u8"bool");
		addBinaryOperator(u8"u32", Inequality, u8"u32", u8"bool");
		addBinaryOperator(u8"u32", LessThan, u8"u32", u8"bool");
		addBinaryOperator(u8"u32", GreaterThan, u8"u32", u8"bool");
		addBinaryOperator(u8"u32", GreaterThanOrEqual, u8"u32", u8"bool");

		// f32
		addBinaryOperator(u8"f32", Equality, u8"f32", u8"bool");
		addBinaryOperator(u8"f32", Inequality, u8"f32", u8"bool");
		addBinaryOperator(u8"f32", LessThan, u8"f32", u8"bool");
		addBinaryOperator(u8"f32", GreaterThan, u8"f32", u8"bool");
		addBinaryOperator(u8"f32", GreaterThanOrEqual, u8"f32", u8"bool");

		// bool
		addBinaryOperator(u8"bool", Equality, u8"bool", u8"bool");
		addBinaryOperator(u8"bool", Inequality, u8"bool", u8"bool");

		// char
		addBinaryOperator(u8"char", Equality, u8"char", u8"bool");
		addBinaryOperator(u8"char", Inequality, u8"char", u8"bool");
		addBinaryOperator(u8"char", LessThan, u8"char", u8"bool");
		addBinaryOperator(u8"char", GreaterThan, u8"char", u8"bool");
		addBinaryOperator(u8"char", GreaterThanOrEqual, u8"char", u8"bool");

		// string
		addBinaryOperator(u8"string", Equality, u8"string", u8"bool");
		addBinaryOperator(u8"string", Inequality, u8"string", u8"bool");
		addBinaryOperator(u8"string", LessThan, u8"string", u8"bool");
		addBinaryOperator(u8"string", GreaterThan, u8"string", u8"bool");
		addBinaryOperator(u8"string", GreaterThanOrEqual, u8"string", u8"bool");

		// LOGICAL BINARY OPERATORS
		addBinaryOperator(u8"bool", LogicalAnd, u8"bool", u8"bool");
		addBinaryOperator(u8"bool", LogicalOr, u8"bool", u8"bool");

		// BITWISE BINARY OPERATORS (i32 and u32)
		addBinaryOperator(u8"i32", BitwiseAnd, u8"i32", u8"i32");
		addBinaryOperator(u8"i32", BitwiseOr, u8"i32", u8"i32");
		addBinaryOperator(u8"i32", BitwiseXor, u8"i32", u8"i32");
		addBinaryOperator(u8"i32", LeftShift, u8"i32", u8"i32");
		addBinaryOperator(u8"i32", RightShift, u8"i32", u8"i32");

		addBinaryOperator(u8"u32", BitwiseAnd, u8"u32", u8"u32");
		addBinaryOperator(u8"u32", BitwiseOr, u8"u32", u8"u32");
		addBinaryOperator(u8"u32", BitwiseXor, u8"u32", u8"u32");
		addBinaryOperator(u8"u32", LeftShift, u8"u32", u8"u32");
		addBinaryOperator(u8"u32", RightShift, u8"u32", u8"u32");

		// UNARY ARITHMETIC
		addUnaryOperator(Positive, u8"i32", u8"i32");
		addUnaryOperator(Positive, u8"u32", u8"u32");
		addUnaryOperator(Positive, u8"f32", u8"f32");

		addUnaryOperator(Negative, u8"i32", u8"i32");
		addUnaryOperator(Negative, u8"f32", u8"f32");

		// UNARY LOGICAL
		addUnaryOperator(Not, u8"bool", u8"bool");

		// UNARY BITWISE
		addUnaryOperator(Not, u8"i32", u8"i32");
		addUnaryOperator(Not, u8"u32", u8"u32");

		// Dereference handled in type-checker explicitly
	}

	Opt<FunctionType> OperatorTable::getUnaryOperator(UnaryOpKind op, const TypePtr &t) const {
		for (auto &[unaryOp, funcType] : m_UnaryOps) {
			if (unaryOp != op)
				continue;

			auto &params = funcType.paramTypes;
			VERIFY(params.size() == 1);

			if (*params[0] == *t)
				return funcType;
		}

		return std::nullopt;
	}

	Opt<FunctionType> OperatorTable::getBinaryOperator(BinaryOpKind op, const TypePtr &t1,
													   const TypePtr &t2) const {
		for (auto &[binaryOp, funcType] : m_BinaryOps) {
			if (binaryOp != op)
				continue;

			auto &params = funcType.paramTypes;
			VERIFY(params.size() == 2);

			if (*params[0] == *t1 && *params[1] == *t2)
				return funcType;
		}

		return std::nullopt;
	}

	void OperatorTable::addBinaryOperator(U8String left, BinaryOpKind op, U8String right,
										  U8String ret) {
		TypePtr leftType = std::make_shared<Typename>(left);
		TypePtr rightType = std::make_shared<Typename>(right);
		TypePtr retType = std::make_shared<Typename>(ret);

		m_BinaryOps.push_back({op, FunctionType({leftType, rightType}, retType)});
	}

	void OperatorTable::addUnaryOperator(ast::UnaryOpKind op, U8String operand, U8String ret) {
		TypePtr operandType = std::make_shared<Typename>(operand);
		TypePtr retType = std::make_shared<Typename>(ret);

		m_UnaryOps.push_back({op, FunctionType({operandType}, retType)});
	}
}