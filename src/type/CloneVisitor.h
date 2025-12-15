#pragma once
#pragma once
#include "Visitor.h"

namespace type {
	///
	/// Deep clone a type with all its nested children
	///
	struct CloneVisitor : public ConstVisitor<TypePtr> {
	private:
		TypePtr visit(const Typename &n) override;
		TypePtr visit(const PointerType &n) override;
		TypePtr visit(const ArrayType &n) override;
		TypePtr visit(const FunctionType &n) override;
		TypePtr visit(const ErrorType &n) override;
		TypePtr visit(const UnitType &n) override;
	};

	TypePtr clone(const Type &type);
}
