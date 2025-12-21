#pragma once

#include "type/Visitor.h"

namespace llvm {
    class Type;
    class LLVMContext;
}

namespace codegen {
	struct TypeConverter : public type::ConstVisitor<llvm::Type *> {
	public:
		explicit TypeConverter(llvm::LLVMContext &ctx);

	private:
		llvm::LLVMContext &m_Context;

	public:
		llvm::Type *visit(const type::Typename &t) override;
		llvm::Type *visit(const type::PointerType &t) override;
		llvm::Type *visit(const type::ArrayType &t) override;
		llvm::Type *visit(const type::FunctionType &t) override;
		llvm::Type *visit(const type::UnitType &t) override;
		llvm::Type *visit(const type::ErrorType &t) override;
	};
}
