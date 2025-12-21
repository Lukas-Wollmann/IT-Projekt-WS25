#include "TypeConverter.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

namespace codegen {
	TypeConverter::TypeConverter(llvm::LLVMContext &ctx)
		: m_Context(ctx) {}

	llvm::Type *TypeConverter::visit(const type::Typename &t) {
		if (t.typename_ == u8"i32")
			return llvm::Type::getInt32Ty(m_Context);
		if (t.typename_ == u8"f32")
			return llvm::Type::getInt32Ty(m_Context);
		if (t.typename_ == u8"char")
			return llvm::Type::getInt32Ty(m_Context);
		if (t.typename_ == u8"bool")
			return llvm::Type::getInt32Ty(m_Context);

		UNREACHABLE();
	}

	llvm::Type *TypeConverter::visit(const type::PointerType &t) {
		return llvm::PointerType::getUnqual(dispatch(*t.pointeeType));
	}

	llvm::Type *TypeConverter::visit(const type::ArrayType &t) {
		llvm::Type *element = dispatch(*t.elementType);
		
        if (t.arraySize.has_value())
			return llvm::ArrayType::get(element, t.arraySize.value());
		
		return llvm::PointerType::getUnqual(element);
	}

	llvm::Type *TypeConverter::visit(const type::FunctionType &t) {
		Vec<llvm::Type *> params;
		
        for (auto &p : t.paramTypes)
			params.push_back(dispatch(*p));

		llvm::Type *ret = dispatch(*t.returnType);
		
        return llvm::FunctionType::get(ret, params, false);
	}

	llvm::Type *TypeConverter::visit(const type::UnitType &t) {
		return llvm::Type::getInt1Ty(m_Context);
	}

	llvm::Type * TypeConverter::visit(const type::ErrorType &) {
		UNREACHABLE();
	}
}