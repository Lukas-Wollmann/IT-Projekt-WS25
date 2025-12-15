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

		// Arithmetic
		addBinaryOperator(Addition, u8"i32");
		addBinaryOperator(Addition, u8"u32");
		addBinaryOperator(Addition, u8"f32");
		addBinaryOperator(Addition, u8"string");

		addBinaryOperator(Subtraction, u8"i32");
		addBinaryOperator(Subtraction, u8"u32");
		addBinaryOperator(Subtraction, u8"f32");

		addBinaryOperator(Multiplication, u8"i32");
		addBinaryOperator(Multiplication, u8"u32");
		addBinaryOperator(Multiplication, u8"f32");

		addBinaryOperator(Division, u8"i32");
		addBinaryOperator(Division, u8"u32");
		addBinaryOperator(Division, u8"f32");

		addBinaryOperator(Modulo, u8"i32");
		addBinaryOperator(Modulo, u8"u32");

		// Comparison
		addBinaryOperator(Equality, u8"i32");
		addBinaryOperator(Equality, u8"u32");
		addBinaryOperator(Equality, u8"f32");
		addBinaryOperator(Equality, u8"bool");
		addBinaryOperator(Equality, u8"char");
		addBinaryOperator(Equality, u8"string");

		addBinaryOperator(Inequality, u8"i32");
		addBinaryOperator(Inequality, u8"u32");
		addBinaryOperator(Inequality, u8"f32");
		addBinaryOperator(Inequality, u8"bool");
		addBinaryOperator(Inequality, u8"char");
		addBinaryOperator(Inequality, u8"string");

		addBinaryOperator(LessThan, u8"i32");
		addBinaryOperator(LessThan, u8"u32");
		addBinaryOperator(LessThan, u8"f32");
		addBinaryOperator(LessThan, u8"char");
		addBinaryOperator(LessThan, u8"string");

		addBinaryOperator(GreaterThan, u8"i32");
		addBinaryOperator(GreaterThan, u8"u32");
		addBinaryOperator(GreaterThan, u8"f32");
		addBinaryOperator(GreaterThan, u8"char");
		addBinaryOperator(GreaterThan, u8"string");

		addBinaryOperator(GreaterThanOrEqual, u8"i32");
		addBinaryOperator(GreaterThanOrEqual, u8"u32");
		addBinaryOperator(GreaterThanOrEqual, u8"f32");
		addBinaryOperator(GreaterThanOrEqual, u8"char");
		addBinaryOperator(GreaterThanOrEqual, u8"string");

		// Logical
		addBinaryOperator(LogicalAnd, u8"bool");
		addBinaryOperator(LogicalOr, u8"bool");

		// Bitwise
		addBinaryOperator(BitwiseAnd, u8"i32");
		addBinaryOperator(BitwiseAnd, u8"u32");

		addBinaryOperator(BitwiseOr, u8"i32");
		addBinaryOperator(BitwiseOr, u8"u32");

		addBinaryOperator(BitwiseXor, u8"i32");
		addBinaryOperator(BitwiseXor, u8"u32");

		addBinaryOperator(LeftShift, u8"i32");
		addBinaryOperator(LeftShift, u8"u32");

		addBinaryOperator(RightShift, u8"i32");
		addBinaryOperator(RightShift, u8"u32");

		// Arithmetic
		addUnaryOperator(Positive, u8"i32");
		addUnaryOperator(Positive, u8"u32");
		addUnaryOperator(Positive, u8"f32");

		addUnaryOperator(Negative, u8"i32");
		addUnaryOperator(Negative, u8"f32");

		// Logical
		addUnaryOperator(Not, u8"bool");

		// Bitwise
		addUnaryOperator(Not, u8"i32");
		addUnaryOperator(Not, u8"u32");

		// Dereference is handled separately in the typechecker
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

	void OperatorTable::addBinaryOperator(BinaryOpKind op, const U8String &typename_) {
		TypePtr type = std::make_shared<Typename>(typename_);

		m_BinaryOps.push_back({op, FunctionType({type, type}, type)});
	}

	void OperatorTable::addUnaryOperator(UnaryOpKind op, const U8String &typename_) {
		TypePtr type = std::make_shared<Typename>(typename_);

		m_UnaryOps.push_back({op, FunctionType({type}, type)});
	}
}