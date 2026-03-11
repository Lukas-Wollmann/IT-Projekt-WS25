#pragma once
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "TypeConverter.h"

namespace gen {
struct TrackedValue {
	llvm::Value *value;
	Type type;
};

struct CodeGenContext {
public:
	constexpr static auto sharedPtrCreate = "__sp_create";
	constexpr static auto sharedPtrCopy = "__sp_copy";
	constexpr static auto sharedPtrDrop = "__sp_drop";

	llvm::LLVMContext llvmContext;
	llvm::IRBuilder<> irBuilder;
	llvm::Module llvmModule;
	gen::TypeConverter typeConverter;

	explicit CodeGenContext(const U8String &moduleName);

	void registerRuntimeFunctions();

	llvm::Value *copyValue(llvm::Value *value, Type type);
	void dropValue(llvm::Value *value, Type type);

	[[nodiscard]] Opt<llvm::Value *> getDestructor(Type type);
	[[nodiscard]] llvm::Value *getNullDestructor();
	[[nodiscard]] llvm::FunctionType *getDestructorType();
	[[nodiscard]] llvm::Value *sizeOf(Type type);
};
}