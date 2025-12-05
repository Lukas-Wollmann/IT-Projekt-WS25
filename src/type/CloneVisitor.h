#pragma once
#pragma once
#include "Visitor.h"

namespace type {
	///
	/// Deep clone a type with all its nested children
	///
	struct CloneVisitor : public ConstVisitor<Box<Type>> {
	private:
		Box<Type> visit(const PrimitiveType &n) override;
		Box<Type> visit(const PointerType &n) override;
		Box<Type> visit(const ArrayType &n) override;
		Box<Type> visit(const FunctionType &n) override;
		Box<Type> visit(const ErrorType &n) override;
		Box<Type> visit(const UnitType &n) override;
	};

	Box<Type> clone(const Type &type);
}
