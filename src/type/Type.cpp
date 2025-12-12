#include "Type.h"

#include "Macros.h"

namespace type {
	Type::Type(TypeKind kind)
		: kind(kind) {}

	PointerType::PointerType(Box<const Type> pointeeType)
		: Type(TypeKind::Pointer)
		, pointeeType(std::move(pointeeType)) {}

	ArrayType::ArrayType(Box<const Type> elementType, Opt<size_t> arraySize)
		: Type(TypeKind::Array)
		, elementType(std::move(elementType))
		, arraySize(arraySize) {}

	FunctionType::FunctionType(Params paramTypes, Box<const Type> returnType)
		: Type(TypeKind::Function)
		, paramTypes(std::move(paramTypes))
		, returnType(std::move(returnType)) {}

	ErrorType::ErrorType()
		: Type(TypeKind::Error) {}

	UnitType::UnitType()
		: Type(TypeKind::Unit) {}

	Typename::Typename(U8String typename_)
		: Type(TypeKind::Typename)
		, typename_(std::move(typename_)) {}
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