#pragma once

#include "Typedef.h"
#include "TypeConverter.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace codegen {
    struct CodeGenContext {
	private:
        llvm::LLVMContext m_LLVMContext;
		Box<llvm::Module> m_LLVMModule;
		llvm::IRBuilder<> m_IRBuilder;
        TypeConverter m_Converter;
		std::unordered_map<U8String, llvm::AllocaInst *> m_Allocas;

    public:
        explicit CodeGenContext(const U8String &moduleName);

        llvm::Type *convertType(type::TypePtr type);

        llvm::AllocaInst *getAlloca(const U8String &ident);
        llvm::AllocaInst *createAlloca(llvm::Type *type, const U8String &ident);
        const std::unordered_map<U8String, llvm::AllocaInst *> &getAllocas() const;
        void setAllocas(std::unordered_map<U8String, llvm::AllocaInst *> allocas);

        llvm::LLVMContext &getLLVMContext();
        llvm::IRBuilder<> &getIRBuilder();
        llvm::Module &getLLVMModule();
    };
}