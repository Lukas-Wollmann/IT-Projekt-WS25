#include "OperatorTable.h"

#include <sstream>

#include "Macros.h"
#include "ast/AST.h"
#include "type/Compare.h"
#include "type/Type.h"

namespace semantic {
	using namespace ast;
	using namespace type;

	OperatorTable::OperatorTable() {
		using enum UnaryOpKind;
		using enum BinaryOpKind;

		// Arithmetic Binary Operators
		addBinaryOperator(u8"i32", u8"i32", Addition, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", Subtraction, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", Multiplication, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", Division, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", Modulo, u8"i32");

		// Comparison Binary Operators
		addBinaryOperator(u8"bool", u8"bool", Equality, u8"bool");
		addBinaryOperator(u8"bool", u8"bool", Inequality, u8"bool");
		addBinaryOperator(u8"char", u8"char", Equality, u8"bool");
		addBinaryOperator(u8"char", u8"char", Inequality, u8"bool");

		addBinaryOperator(u8"i32", u8"i32", Equality, u8"bool");
		addBinaryOperator(u8"i32", u8"i32", Inequality, u8"bool");
		addBinaryOperator(u8"i32", u8"i32", LessThan, u8"bool");
		addBinaryOperator(u8"i32", u8"i32", LessThanOrEqual, u8"bool");
		addBinaryOperator(u8"i32", u8"i32", GreaterThan, u8"bool");
		addBinaryOperator(u8"i32", u8"i32", GreaterThanOrEqual, u8"bool");

		// Logical Binary Operators
		addBinaryOperator(u8"bool", u8"bool", LogicalAnd, u8"bool");
		addBinaryOperator(u8"bool", u8"bool", LogicalOr, u8"bool");

		// Bitwise Binary Operators
		addBinaryOperator(u8"i32", u8"i32", BitwiseAnd, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", BitwiseOr, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", BitwiseXor, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", LeftShift, u8"i32");
		addBinaryOperator(u8"i32", u8"i32", RightShift, u8"i32");

		// Unary Arithmetic Operators
		addUnaryOperator(u8"i32", Positive, u8"i32");
		addUnaryOperator(u8"i32", Negative, u8"i32");

		// Unary Logical Operators
		addUnaryOperator(u8"bool", LogicalNot, u8"bool");

		// Unary Bitwise Operators
		addUnaryOperator(u8"i32", BitwiseNot, u8"i32");

		// Dereference handled in type-checker explicitly, it is not a normal
		// operator as it works on all operand types that are a PointerType.
	}

	Opt<FunctionType> OperatorTable::getUnaryOperator(const UnaryOpKind op,
													  const TypePtr &t) const {
		for (auto &[unaryOp, funcType] : m_UnaryOps) {
			if (unaryOp != op)
				continue;

			auto &params = funcType.paramTypes;
			VERIFY(params.size() == 1);

			if (*params[0] == *t)
				return funcType;
		}

		return {};
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

		return {};
	}

	void OperatorTable::addBinaryOperator(const U8String &left, const U8String &right,
										  const BinaryOpKind op, const U8String &ret) {
		const auto leftType = std::make_shared<Typename>(left);
		const auto rightType = std::make_shared<Typename>(right);
		const auto retType = std::make_shared<Typename>(ret);

		m_BinaryOps.emplace_back(op, FunctionType({leftType, rightType}, retType));
	}

	void OperatorTable::addUnaryOperator(const U8String &operand, const UnaryOpKind op,
										 const U8String &ret) {
		const auto operandType = std::make_shared<Typename>(operand);
		const auto retType = std::make_shared<Typename>(ret);

		m_UnaryOps.emplace_back(op, FunctionType({operandType}, retType));
	}
}