#pragma once

#include "type/Type.h"

namespace llvm {
class Type;
class LLVMContext;
}

namespace gen {
struct TypeConverter {
public:
	explicit TypeConverter(llvm::LLVMContext &ctx);

	llvm::Type *convert(Type type);

private:
	llvm::LLVMContext &m_Context;

	llvm::Type *convertPrimitive(const PrimitiveType &t);
	llvm::Type *convertPointer(const PointerType &t);
	llvm::Type *convertFunction(const FunctionType &t);
	llvm::Type *convertUnit(const UnitType &t);
	llvm::Type *convertStruct(const StructType &t);
	llvm::Type *convertArray(const ArrayType &t);
};
}
