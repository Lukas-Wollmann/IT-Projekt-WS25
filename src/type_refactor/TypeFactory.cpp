#include "TypeFactory.h"

Vec<Box<const TypeBase>> TypeFactory::s_Registry;

Type TypeFactory::intern(const TypeBase &newType) {
	for (const auto &existing : s_Registry) {
		if (existing->equals(&newType)) {
			return existing.get();
		}
	}

	s_Registry.push_back(newType.clone());

	return s_Registry.back().get();
}

Vec<Type> TypeFactory::allTypes() {
	Vec<Type> result;

	for (const auto &t : s_Registry) {
		result.push_back(t.get());
	}

	return result;
}

const PrimitiveType *TypeFactory::getI32() {
	const auto type = intern(PrimitiveType(PrimitiveKind::I32));
	return reinterpret_cast<const PrimitiveType *>(type);
}

const PrimitiveType *TypeFactory::getChar() {
	const auto type = intern(PrimitiveType(PrimitiveKind::Char));
	return reinterpret_cast<const PrimitiveType *>(type);
}

const PrimitiveType *TypeFactory::getBool() {
	const auto type = intern(PrimitiveType(PrimitiveKind::Bool));
	return reinterpret_cast<const PrimitiveType *>(type);
}

const UnitType *TypeFactory::getUnit() {
	const auto type = intern(UnitType());
	return reinterpret_cast<const UnitType *>(type);
}

const ErrorType *TypeFactory::getError() {
	const auto type = intern(ErrorType());
	return reinterpret_cast<const ErrorType *>(type);
}

const PointerType *TypeFactory::getPointer(Type pointeeType) {
	const auto type = intern(PointerType(pointeeType));
	return reinterpret_cast<const PointerType *>(type);
}

const FunctionType *TypeFactory::getFunction(TypeList paramTypes, Type returnType) {
	const auto type = intern(FunctionType(std::move(paramTypes), returnType));
	return reinterpret_cast<const FunctionType *>(type);
}

const StructType *TypeFactory::makeStruct(U8String name, Vec<StructField> fields) {
	const auto type = intern(StructType(name, std::move(fields)));
	return reinterpret_cast<const StructType *>(type);
}

const StructType *TypeFactory::getStruct(const U8String &name) {
	for (const auto &t : s_Registry) {
		if (auto *s = dynamic_cast<const StructType *>(t.get())) {
			if (s->name == name) {
				return s;
			}
		}
	}
	return nullptr;
}
