#include "Type.h"

#include "Macros.h"

namespace type {
	Type::Type(TypeKind kind)
		: kind(kind) {}

    bool Type::isTypeKind(TypeKind other) const {
        return kind == other;
    }

    Typename::Typename(U8String typename_)
		: Type(TypeKind::Typename)
		, typename_(std::move(typename_)) {}

	PointerType::PointerType(Ptr<const Type> pointeeType)
		: Type(TypeKind::Pointer)
		, pointeeType(pointeeType) {}

	ArrayType::ArrayType(Ptr<const Type> elementType, Opt<size_t> arraySize)
		: Type(TypeKind::Array)
		, elementType(elementType)
		, arraySize(arraySize) {}

	FunctionType::FunctionType(TypeList paramTypes, Ptr<const Type> returnType)
		: Type(TypeKind::Function)
		, paramTypes(paramTypes)
		, returnType(returnType) {}

	ErrorType::ErrorType()
		: Type(TypeKind::Error) {}

	UnitType::UnitType()
		: Type(TypeKind::Unit) {}
}

std::ostream &operator<<(std::ostream &os, type::TypeKind kind) {
	using enum type::TypeKind;
	switch (kind) {
		case Typename: return os << "Typename";
		case Pointer:  return os << "Pointer";
		case Array:	   return os << "Array";
		case Function: return os << "Function";
		case Error:	   return os << "Error";
		case Unit:	   return os << "Unit";
		default:	   UNREACHABLE();
	}
}