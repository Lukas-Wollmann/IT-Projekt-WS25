#include "TypeConverter.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace gen {
TypeConverter::TypeConverter(llvm::LLVMContext &ctx)
	: m_Context(ctx) {}

llvm::Type *TypeConverter::convert(Type type) {
	VERIFY(type);

	switch (type->kind) {
		case TypeKind::Primitive:
			return convertPrimitive(static_cast<const PrimitiveType &>(*type));
		case TypeKind::Null: {
			auto *i8 = llvm::Type::getInt8Ty(m_Context);
			return llvm::PointerType::getUnqual(i8);
		}
		case TypeKind::Pointer:	 return convertPointer(static_cast<const PointerType &>(*type));
		case TypeKind::Function: return convertFunction(static_cast<const FunctionType &>(*type));
		case TypeKind::Unit:	 return convertUnit(static_cast<const UnitType &>(*type));
		case TypeKind::Struct:	 return convertStruct(static_cast<const StructType &>(*type));
		case TypeKind::Error:	 UNREACHABLE();
	}

	UNREACHABLE();
}

llvm::Type *TypeConverter::convertPrimitive(const PrimitiveType &t) {
	switch (t.primitiveKind) {
		case PrimitiveKind::I32:  return llvm::Type::getInt32Ty(m_Context);
		case PrimitiveKind::Char: return llvm::Type::getInt32Ty(m_Context);
		case PrimitiveKind::Bool: return llvm::Type::getInt1Ty(m_Context);
	}

	UNREACHABLE();
}

llvm::Type *TypeConverter::convertPointer(const PointerType &t) {
	return llvm::PointerType::getUnqual(convert(t.pointeeType));
}

llvm::Type *TypeConverter::convertFunction(const FunctionType &t) {
	Vec<llvm::Type *> params;

	for (auto param : t.paramTypes) {
		params.push_back(convert(param));
	}

	llvm::Type *ret = convert(t.returnType);
	auto *functionType = llvm::FunctionType::get(ret, params, false);
	return llvm::PointerType::getUnqual(functionType);
}

llvm::Type *TypeConverter::convertUnit(const UnitType &) {
	return llvm::Type::getInt1Ty(m_Context);
}

llvm::Type *TypeConverter::convertStruct(const StructType &t) {
	auto *structType = llvm::StructType::getTypeByName(m_Context, t.name.asAscii());
	VERIFY(structType != nullptr);
	return structType;
}
}