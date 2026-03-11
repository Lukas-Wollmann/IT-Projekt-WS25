#include "Type.h"

PrimitiveType::PrimitiveType(PrimitiveKind primitiveKind)
	: TypeBase(TypeKind::Primitive)
	, primitiveKind(primitiveKind) {}

U8String PrimitiveType::str() const {
	switch (primitiveKind) {
		case PrimitiveKind::I32:  return u8"i32";
		case PrimitiveKind::Char: return u8"char";
		case PrimitiveKind::Bool: return u8"bool";
		default:				  return u8"unknown_primitive";
	}
}

bool PrimitiveType::equals(const TypeBase *other) const {
	if (!other || other->kind != TypeKind::Primitive)
		return false;
	auto *otherPrim = static_cast<const PrimitiveType *>(other);
	return primitiveKind == otherPrim->primitiveKind;
}

Box<TypeBase> PrimitiveType::clone() const {
	return std::make_unique<PrimitiveType>(primitiveKind);
}

UnitType::UnitType()
	: TypeBase(TypeKind::Unit) {}

U8String UnitType::str() const {
	return u8"()";
}

bool UnitType::equals(const TypeBase *other) const {
	return other && other->kind == TypeKind::Unit;
}

Box<TypeBase> UnitType::clone() const {
	return std::make_unique<UnitType>();
}

ErrorType::ErrorType()
	: TypeBase(TypeKind::Error) {}

U8String ErrorType::str() const {
	return u8"<error-type>";
}

bool ErrorType::equals(const TypeBase *other) const {
	return other && other->kind == TypeKind::Error;
}

Box<TypeBase> ErrorType::clone() const {
	return std::make_unique<ErrorType>();
}

PointerType::PointerType(Type pointeeType)
	: TypeBase(TypeKind::Pointer)
	, pointeeType(pointeeType) {}

U8String PointerType::str() const {
	return std::format("*{}", pointeeType);
}

bool PointerType::equals(const TypeBase *other) const {
	if (!other || other->kind != TypeKind::Pointer)
		return false;
	auto *otherPtr = static_cast<const PointerType *>(other);

	return pointeeType == otherPtr->pointeeType;
}

Box<TypeBase> PointerType::clone() const {
	return std::make_unique<PointerType>(pointeeType);
}

FunctionType::FunctionType(TypeList paramTypes, Type returnType)
	: TypeBase(TypeKind::Function)
	, paramTypes(std::move(paramTypes))
	, returnType(returnType) {}

U8String FunctionType::str() const {
	return std::format("({}) -> {}", paramTypes, returnType);
}

bool FunctionType::equals(const TypeBase *other) const {
	if (!other || other->kind != TypeKind::Function) {
		return false;
	}

	auto *otherFn = static_cast<const FunctionType *>(other);

	if (returnType != otherFn->returnType) {
		return false;
	}

	if (paramTypes.size() != otherFn->paramTypes.size()) {
		return false;
	}

	for (size_t i = 0; i < paramTypes.size(); ++i) {
		if (paramTypes[i] != otherFn->paramTypes[i]) {
			return false;
		}
	}
	return true;
}

Box<TypeBase> FunctionType::clone() const {
	return std::make_unique<FunctionType>(paramTypes, returnType);
}

StructType::StructType(U8String name, Vec<StructField> fields)
	: TypeBase(TypeKind::Struct)
	, name(std::move(name))
	, fields(std::move(fields)) {}

U8String StructType::str() const {
	return name;
}

bool StructType::equals(const TypeBase *other) const {
	if (!other || other->kind != TypeKind::Struct)
		return false;
	auto *otherStruct = static_cast<const StructType *>(other);

	return name == otherStruct->name;
}

Box<TypeBase> StructType::clone() const {
	return std::make_unique<StructType>(name, fields);
}