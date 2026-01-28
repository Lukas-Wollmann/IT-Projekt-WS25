#pragma once
#include "type/Visitor.h"

namespace type {
	struct Compare : ConstVisitor<bool> {
	private:
		const Type &m_Other;

	public:
		explicit Compare(const Type &other);

		bool visit(const Typename &n) override;
		bool visit(const PointerType &n) override;
		bool visit(const ArrayType &n) override;
		bool visit(const FunctionType &n) override;
		bool visit(const ErrorType &n) override;
		bool visit(const UnitType &n) override;
	};

	bool operator==(const TypeList &left, const TypeList &right);
	bool operator!=(const TypeList &left, const TypeList &right);
	bool operator==(const Type &left, const Type &right);
	bool operator!=(const Type &left, const Type &right);
}