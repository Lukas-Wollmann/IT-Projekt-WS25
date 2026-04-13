#pragma once
#include "Namespace.h"
#include "core/Operators.h"
#include "type/Type.h"

namespace sem {
struct OperatorTable {
private:
	Vec<Pair<UnaryOpKind, const FunctionType *>> m_UnaryOps;
	Vec<Pair<BinaryOpKind, const FunctionType *>> m_BinaryOps;

public:
	OperatorTable();
	OperatorTable(const OperatorTable &) = delete;
	OperatorTable(OperatorTable &&) = delete;

	OperatorTable &operator=(const OperatorTable &) = delete;
	OperatorTable &operator=(OperatorTable &&) = delete;

	[[nodiscard]] Opt<const FunctionType *> getUnaryOperator(UnaryOpKind op, Type t) const;
	[[nodiscard]] Opt<const FunctionType *> getBinaryOperator(BinaryOpKind op, Type t1,
															  Type t2) const;

private:
	void addBinaryOperator(const U8String &left, const U8String &right, BinaryOpKind op,
						   const U8String &ret);
	void addUnaryOperator(const U8String &operand, UnaryOpKind op, const U8String &ret);
	Type getTypeFromName(const U8String &name);
};
}