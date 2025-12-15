#include "CloneVisitor.h"

namespace type {
	TypePtr CloneVisitor::visit(const Typename &n) {
		return std::make_shared<Typename>(n.typename_);
	}

	TypePtr CloneVisitor::visit(const PointerType &n) {
		return std::make_shared<PointerType>(dispatch(*n.pointeeType));
	}

	TypePtr CloneVisitor::visit(const ArrayType &n) {
		return std::make_shared<ArrayType>(dispatch(*n.elementType), n.arraySize);
	}

	TypePtr CloneVisitor::visit(const FunctionType &n) {
		TypeList paramTypes;

		for (auto &paramType : n.paramTypes)
			paramTypes.push_back(dispatch(*paramType));

		return std::make_shared<FunctionType>(std::move(paramTypes), dispatch(*n.returnType));
	}

	TypePtr CloneVisitor::visit(const ErrorType &) {
		return std::make_shared<ErrorType>();
	}

	TypePtr CloneVisitor::visit(const UnitType &) {
		return std::make_shared<UnitType>();
	}

	TypePtr clone(const Type &type) {
		return CloneVisitor().dispatch(type);
	}
}