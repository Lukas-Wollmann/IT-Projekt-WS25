#pragma once
#include "type/Visitor.h"

namespace type {
	struct CompareVisitor : public type::ConstVisitor<bool> {
	private:
		const Type &m_Other;

	public:
		explicit CompareVisitor(const Type &other);

		bool visit(const PrimitiveType &n) override;
		bool visit(const PointerType &n) override;
		bool visit(const ArrayType &n) override;
		bool visit(const FunctionType &n) override;
		bool visit(const ErrorType &n) override;
		bool visit(const UnitType &n) override;
	};
}

bool operator==(const type::Type &left, const type::Type &right);
bool operator!=(const type::Type &left, const type::Type &right);