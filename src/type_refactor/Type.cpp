#include "Type.h"

PrimitiveType::PrimitiveType(PrimitiveKind primitiveKind)
	: TypeBase(TypeKind::Primitive)
	, primitiveKind(primitiveKind) {}

U8String PrimitiveType::str() const {
	switch (primitiveKind) {
		case PrimitiveKind::I32:  return u8"i32";
		case PrimitiveKind::Char: return u8"char";
		case PrimitiveKind::Bool: return u8"bool";
		default:				  UNREACHABLE();
	}
}

bool PrimitiveType::equals(Type other) const {
	if (!other || other->kind != TypeKind::Primitive)
		return false;
	auto *otherPrim = dynamic_cast<const PrimitiveType *>(other);
	return otherPrim && otherPrim->primitiveKind == primitiveKind;
}

Box<const TypeBase> PrimitiveType::clone() const {
	return std::make_unique<const PrimitiveType>(*this);
}

UnitType::UnitType()
	: TypeBase(TypeKind::Unit) {}

U8String UnitType::str() const {
	return u8"()";
}

bool UnitType::equals(Type other) const {
	if (!other)
		return false;
	return other->kind == TypeKind::Unit;
}

Box<const TypeBase> UnitType::clone() const {
	return std::make_unique<const UnitType>(*this);
}

ErrorType::ErrorType()
	: TypeBase(TypeKind::Error) {}

U8String ErrorType::str() const {
	return u8"error";
}

bool ErrorType::equals(Type other) const {
	if (!other)
		return false;
	return other->kind == TypeKind::Error;
}

Box<const TypeBase> ErrorType::clone() const {
	return std::make_unique<const ErrorType>(*this);
}

PointerType::PointerType(Type pointeeType)
	: TypeBase(TypeKind::Pointer)
	, pointeeType(pointeeType) {}

U8String PointerType::str() const {
	return pointeeType->str() + U8String("*");
}

bool PointerType::equals(Type other) const {
	if (!other || other->kind != TypeKind::Pointer)
		return false;
	auto *otherPtr = dynamic_cast<const PointerType *>(other);
	return otherPtr && otherPtr->pointeeType == pointeeType;
}

Box<const TypeBase> PointerType::clone() const {
	return std::make_unique<const PointerType>(*this);
}

FunctionType::FunctionType(Vec<Type> paramTypes, Type returnType)
	: TypeBase(TypeKind::Function)
	, paramTypes(std::move(paramTypes))
	, returnType(returnType) {}

U8String FunctionType::str() const {
	return std::format("({}) -> {}", paramTypes, returnType);
}

bool FunctionType::equals(Type other) const {
	if (!other || other->kind != TypeKind::Function)
		return false;
	auto *otherFunc = dynamic_cast<const FunctionType *>(other);
	if (!otherFunc)
		return false;
	if (otherFunc->returnType != returnType)
		return false;
	if (otherFunc->paramTypes.size() != paramTypes.size())
		return false;
	for (size_t i = 0; i < paramTypes.size(); ++i) {
		if (otherFunc->paramTypes[i] != paramTypes[i])
			return false;
	}
	return true;
}

Box<const TypeBase> FunctionType::clone() const {
	return std::make_unique<const FunctionType>(*this);
}

StructType::StructType(U8String name, Vec<StructField> fields)
	: TypeBase(TypeKind::Struct)
	, name(std::move(name))
	, fields(std::move(fields)) {}

U8String StructType::str() const {
	return name;
}

bool StructType::equals(Type other) const {
	if (!other || other->kind != TypeKind::Struct)
		return false;
	auto *otherStruct = dynamic_cast<const StructType *>(other);
	return otherStruct && otherStruct->name == name;
}

Box<const TypeBase> StructType::clone() const {
	return std::make_unique<const StructType>(*this);
}
