#pragma once
#include <deque>
#include <memory>
#include <vector>

#include "Type.h"

struct TypeFactory {
private:
	static Type intern(const TypeBase &newType);
	static std::deque<Box<TypeBase>> &getRegistry();

public:
	static PrimitiveType *getI32();
	static PrimitiveType *getChar();
	static PrimitiveType *getBool();
	static UnitType *getUnit();
	static ErrorType *getError();
	static NullType *getNull();
	static PointerType *getPointer(Type pointeeType);
	static FunctionType *getFunction(TypeList paramTypes, Type returnType);
	static StructType *getStruct(U8String name);

	static void reset();
	static Vec<Type> allTypes();
};
