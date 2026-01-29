#pragma once
#include "Namespace.h"
#include "core/Operators.h"
#include "type/Type.h"

namespace sem {
struct OperatorTable {
private:
	Vec<Pair<UnaryOpKind, type::FunctionType>> m_UnaryOps;
	Vec<Pair<BinaryOpKind, type::FunctionType>> m_BinaryOps;

public:
	OperatorTable();
	OperatorTable(const OperatorTable &) = delete;
	OperatorTable(OperatorTable &&) = delete;

	OperatorTable &operator=(const OperatorTable &) = delete;
	OperatorTable &operator=(OperatorTable &&) = delete;

	[[nodiscard]] Opt<type::FunctionType> getUnaryOperator(UnaryOpKind op,
														   const type::TypePtr &t) const;
	[[nodiscard]] Opt<type::FunctionType>
	getBinaryOperator(BinaryOpKind op, const type::TypePtr &t1, const type::TypePtr &t2) const;

private:
	void addBinaryOperator(const U8String &left, const U8String &right, BinaryOpKind op,
						   const U8String &ret);
	void addUnaryOperator(const U8String &operand, UnaryOpKind op, const U8String &ret);
};
}