#pragma once
#include "Namespace.h"
#include "type/Type.h"

namespace semantic {
	struct OperatorTable {
	private:
		Vec<Pair<ast::UnaryOpKind, type::FunctionType>> m_UnaryOps;
		Vec<Pair<ast::BinaryOpKind, type::FunctionType>> m_BinaryOps;

	public:
		OperatorTable();
		OperatorTable(const OperatorTable &) = delete;
		OperatorTable(OperatorTable &&) = delete;

		OperatorTable &operator=(const OperatorTable &) = delete;
		OperatorTable &operator=(OperatorTable &&) = delete;

		[[nodiscard]] Opt<type::FunctionType> getUnaryOperator(ast::UnaryOpKind op,
															   const type::TypePtr &t) const;
		[[nodiscard]] Opt<type::FunctionType> getBinaryOperator(ast::BinaryOpKind op,
																const type::TypePtr &t1,
																const type::TypePtr &t2) const;

	private:
		void addBinaryOperator(const U8String &left, const U8String &right, ast::BinaryOpKind op,
							   const U8String &ret);
		void addUnaryOperator(const U8String &operand, ast::UnaryOpKind op, const U8String &ret);
	};
}