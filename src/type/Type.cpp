#include "Type.h"

#include "Macros.h"

namespace type {
	Type::Type(TypeKind kind)
		: kind(kind) {}

	PrimitiveType::PrimitiveType(PrimitiveTypeKind primitiveKind)
		: Type(TypeKind::Primitive)
		, primitiveKind(primitiveKind) {}

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
}

std::ostream &operator<<(std::ostream &os, type::TypeKind kind) {
	using enum type::TypeKind;
	switch (kind) {
		case Primitive: return os << "Primitive";
		case Pointer:	return os << "Pointer";
		case Array:		return os << "Array";
		case Function:	return os << "Function";
		case Error:		return os << "Error";
		case Unit:		return os << "Unit";
		default:		UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, type::PrimitiveTypeKind kind) {
	using enum type::PrimitiveTypeKind;
	switch (kind) {
		case I32:	 return os << "i32";
		case U32:	 return os << "u32";
		case F32:	 return os << "f32";
		case String: return os << "string";
		case Char:	 return os << "char";
		case Bool:	 return os << "bool";
		default:	 UNREACHABLE();
	}
}