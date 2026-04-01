#include "OperatorTable.h"

#include "ast/AST.h"
#include "core/Macros.h"
#include "type/TypeFactory.h"

namespace sem {
using namespace ast;

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

Opt<const FunctionType *> OperatorTable::getUnaryOperator(UnaryOpKind op, Type t) const {
	for (auto &[unaryOp, funcType] : m_UnaryOps) {
		if (unaryOp != op)
			continue;

		auto &params = funcType->paramTypes;
		VERIFY(params.size() == 1);

		if (params[0]->equals(t))
			return funcType;
	}

	return {};
}

Opt<const FunctionType *> OperatorTable::getBinaryOperator(BinaryOpKind op, Type t1,
														   Type t2) const {
	if (op == BinaryOpKind::Equality || op == BinaryOpKind::Inequality) {
		const bool leftPtr = t1->isTypeKind(TypeKind::Pointer);
		const bool rightPtr = t2->isTypeKind(TypeKind::Pointer);
		const bool leftNull = t1->isTypeKind(TypeKind::Null);
		const bool rightNull = t2->isTypeKind(TypeKind::Null);

		if ((leftPtr && rightPtr && t1->equals(t2)) || (leftPtr && rightNull) ||
			(leftNull && rightPtr)) {
			return TypeFactory::getFunction(TypeList{t1, t2}, TypeFactory::getBool());
		}
	}

	for (auto &[binaryOp, funcType] : m_BinaryOps) {
		if (binaryOp != op)
			continue;

		auto &params = funcType->paramTypes;
		VERIFY(params.size() == 2);

		if (params[0]->equals(t1) && params[1]->equals(t2))
			return funcType;
	}

	return {};
}

void OperatorTable::addBinaryOperator(const U8String &left, const U8String &right, BinaryOpKind op,
									  const U8String &ret) {
	Type leftType = getTypeFromName(left);
	Type rightType = getTypeFromName(right);
	Type retType = getTypeFromName(ret);

	auto funcType = TypeFactory::getFunction(TypeList{leftType, rightType}, retType);
	m_BinaryOps.emplace_back(op, funcType);
}

void OperatorTable::addUnaryOperator(const U8String &operand, UnaryOpKind op, const U8String &ret) {
	Type operandType = getTypeFromName(operand);
	Type retType = getTypeFromName(ret);

	auto funcType = TypeFactory::getFunction(TypeList{operandType}, retType);
	m_UnaryOps.emplace_back(op, funcType);
}

Type OperatorTable::getTypeFromName(const U8String &name) {
	if (name == u8"i32")
		return TypeFactory::getI32();
	if (name == u8"char")
		return TypeFactory::getChar();
	if (name == u8"bool")
		return TypeFactory::getBool();

	UNREACHABLE();
}
}