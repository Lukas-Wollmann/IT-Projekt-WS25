#include "TypeFactory.h"

std::deque<Box<TypeBase>> &TypeFactory::getRegistry() {
	static std::deque<Box<TypeBase>> s_Registry;
	return s_Registry;
}

void TypeFactory::reset() {
	auto &registry = getRegistry();
	registry.clear();
}

Type TypeFactory::intern(const TypeBase &newType) {
	auto &registry = getRegistry();

	for (const auto &existing : registry) {
		if (existing->equals(&newType)) {
			return existing.get();
		}
	}

	registry.push_back(newType.clone());

	return registry.back().get();
}

PrimitiveType *TypeFactory::getI32() {
	auto type = intern(PrimitiveType(PrimitiveKind::I32));
	return static_cast<PrimitiveType *>(type);
}

PrimitiveType *TypeFactory::getChar() {
	auto type = intern(PrimitiveType(PrimitiveKind::Char));
	return static_cast<PrimitiveType *>(type);
}

PrimitiveType *TypeFactory::getBool() {
	auto type = intern(PrimitiveType(PrimitiveKind::Bool));
	return static_cast<PrimitiveType *>(type);
}

UnitType *TypeFactory::getUnit() {
	auto type = intern(UnitType());
	return static_cast<UnitType *>(type);
}

ErrorType *TypeFactory::getError() {
	auto type = intern(ErrorType());
	return static_cast<ErrorType *>(type);
}

NullType *TypeFactory::getNull() {
	auto type = intern(NullType());
	return static_cast<NullType *>(type);
}

PointerType *TypeFactory::getPointer(Type pointeeType) {
	auto type = intern(PointerType(pointeeType));
	return static_cast<PointerType *>(type);
}

FunctionType *TypeFactory::getFunction(TypeList paramTypes, Type returnType) {
	auto type = intern(FunctionType(std::move(paramTypes), returnType));
	return static_cast<FunctionType *>(type);
}

StructType *TypeFactory::getStruct(U8String name) {
	auto type = intern(StructType(std::move(name)));
	return static_cast<StructType *>(type);
}

ArrayType *TypeFactory::getArray(Type elementType) {
	auto type = intern(ArrayType(elementType));
	return static_cast<ArrayType *>(type);
}

Vec<Type> TypeFactory::allTypes() {
	Vec<Type> result;

	for (const auto &type : getRegistry()) {
		result.push_back(type.get());
	}

	return result;
}