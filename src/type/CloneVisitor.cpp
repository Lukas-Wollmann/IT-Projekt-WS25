#include "CloneVisitor.h"

namespace type {
	Box<Type> CloneVisitor::visit(const PrimitiveType &n) {
		return std::make_unique<PrimitiveType>(n.primitiveKind);
	}

	Box<Type> CloneVisitor::visit(const PointerType &n) {
		return std::make_unique<PointerType>(dispatch(*n.pointeeType));
	}

	Box<Type> CloneVisitor::visit(const ArrayType &n) {
		return std::make_unique<ArrayType>(dispatch(*n.elementType), n.arraySize);
	}

	Box<Type> CloneVisitor::visit(const FunctionType &n) {
		Vec<Box<const Type>> paramTypes;

		for (auto &paramType : n.paramTypes)
			paramTypes.push_back(dispatch(*paramType));

		return std::make_unique<FunctionType>(std::move(paramTypes), dispatch(*n.returnType));
	}

	Box<Type> CloneVisitor::visit(const ErrorType &) {
		return std::make_unique<ErrorType>();
	}

	Box<Type> CloneVisitor::visit(const UnitType &) {
		return std::make_unique<UnitType>();
	}

	Box<Type> clone(const Type &type) {
		return CloneVisitor().dispatch(type);
	}
}