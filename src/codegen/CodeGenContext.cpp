#include "CodeGenContext.h"

namespace gen {
CodeGenContext::CodeGenContext(const U8String &moduleName)
	: m_LLVMModule(std::make_unique<llvm::Module>(moduleName.asAscii(), m_LLVMContext))
	, m_IRBuilder(m_LLVMContext)
	, m_Converter(m_LLVMContext) {}

llvm::Type *CodeGenContext::convertType(type::TypePtr type) {
	return m_Converter.dispatch(*type);
}

llvm::AllocaInst *CodeGenContext::getAlloca(const U8String &ident) {
	auto it = m_Allocas.find(ident);

	if (it == m_Allocas.end())
		return nullptr;

	return it->second;
}

llvm::AllocaInst *CodeGenContext::createAlloca(llvm::Type *type, const U8String &ident) {
	auto func = m_IRBuilder.GetInsertBlock()->getParent();

	VERIFY(func);

	llvm::IRBuilder<> tmp(&func->getEntryBlock(), func->getEntryBlock().begin());

	auto alloca = tmp.CreateAlloca(type, nullptr, ident.asAscii());
	m_Allocas[ident] = alloca;

	return alloca;
}

const std::unordered_map<U8String, llvm::AllocaInst *> &CodeGenContext::getAllocas() const {
	return m_Allocas;
}

void CodeGenContext::setAllocas(std::unordered_map<U8String, llvm::AllocaInst *> allocas) {
	m_Allocas = std::move(allocas);
}

llvm::LLVMContext &CodeGenContext::getLLVMContext() {
	return m_LLVMContext;
}

llvm::IRBuilder<> &CodeGenContext::getIRBuilder() {
	return m_IRBuilder;
}

llvm::Module &CodeGenContext::getLLVMModule() {
	return *m_LLVMModule;
}
}