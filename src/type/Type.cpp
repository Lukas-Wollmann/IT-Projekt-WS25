#include "Type.h"

#include "Macros.h"

namespace type {
	Type::Type(const TypeKind kind)
		: kind(kind) {}

	bool Type::isTypeKind(const TypeKind other) const {
		return kind == other;
	}

	Typename::Typename(U8String typename_)
		: Type(TypeKind::Typename)
		, typename_(std::move(typename_)) {}

	PointerType::PointerType(TypePtr pointeeType)
		: Type(TypeKind::Pointer)
		, pointeeType(std::move(pointeeType)) {}

	ArrayType::ArrayType(TypePtr elementType)
		: Type(TypeKind::Array)
		, elementType(std::move(elementType)) {}

	FunctionType::FunctionType(TypeList paramTypes, TypePtr returnType)
		: Type(TypeKind::Function)
		, paramTypes(std::move(paramTypes))
		, returnType(std::move(returnType)) {}

	ErrorType::ErrorType()
		: Type(TypeKind::Error) {}

	UnitType::UnitType()
		: Type(TypeKind::Unit) {}
}