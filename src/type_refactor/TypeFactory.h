#pragma once
#include <memory>
#include <vector>

#include "Type.h"

struct TypeFactory {
private:
	static Vec<Box<const TypeBase>> s_Registry;

	static Type intern(const TypeBase &newType);

public:
	static const PrimitiveType *getI32();
	static const PrimitiveType *getChar();
	static const PrimitiveType *getBool();

	static const UnitType *getUnit();
	static const ErrorType *getError();

	static const PointerType *getPointer(Type pointeeType);
	static const FunctionType *getFunction(TypeList paramTypes, Type returnType);

	static const StructType *makeStruct(U8String name, Vec<StructField> fields);
	static const StructType *getStruct(const U8String &name);

	static Vec<Type> allTypes();
};
