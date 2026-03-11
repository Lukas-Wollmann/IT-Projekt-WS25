#include "CodeGenContext.h"

#include <ranges>

namespace gen {
CodeGenContext::CodeGenContext(const U8String &moduleName)
	: irBuilder(llvmContext)
	, llvmModule(moduleName.asAscii(), llvmContext)
	, typeConverter(llvmContext) {
	registerRuntimeFunctions();
}

void CodeGenContext::registerRuntimeFunctions() {
	const auto &dataLayout = llvmModule.getDataLayout();
	auto *sizeTy = irBuilder.getIntPtrTy(dataLayout);
	auto *ptrTy = irBuilder.getPtrTy();
	auto *voidTy = irBuilder.getVoidTy();

	llvm::FunctionType *spCreateTy = llvm::FunctionType::get(ptrTy, {sizeTy, ptrTy}, false);
	llvm::FunctionType *spCopyTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
	llvm::FunctionType *spDropTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);

	llvmModule.getOrInsertFunction(sharedPtrCreate, spCreateTy);
	llvmModule.getOrInsertFunction(sharedPtrCopy, spCopyTy);
	llvmModule.getOrInsertFunction(sharedPtrDrop, spDropTy);
}

llvm::Value *CodeGenContext::copyValue(llvm::Value *value, Type type) {
	if (type->isTypeKind(TypeKind::Unit) || type->isTypeKind(TypeKind::Primitive)) {
		return value;
	}

	if (type->isTypeKind(TypeKind::Struct)) {
		// TODO: For struct types emit a copy constructor here
		return value;
	}

	if (type->isTypeKind(TypeKind::Pointer)) {
		// For pointers, we need to increment the reference count
		auto *const func = llvmModule.getFunction(sharedPtrCopy);
		VERIFY(func);
		auto *const result = irBuilder.CreateCall(func, {value});

		return result;
	}

	UNREACHABLE();
}

void CodeGenContext::dropValue(llvm::Value *value, Type type) {
	if (auto dtor = getDestructor(type)) {
		irBuilder.CreateCall(getDestructorType(), dtor.value(), {value});
	}
}

Opt<llvm::Value *> CodeGenContext::getDestructor(Type type) {
	if (type->isTypeKind(TypeKind::Unit) || type->isTypeKind(TypeKind::Primitive)) {
		return {};
	}

	if (type->isTypeKind(TypeKind::Struct)) {
		return {}; // TODO recursive for structs / emit dtor
	}

	if (type->isTypeKind(TypeKind::Pointer)) {
		auto *const dtor = llvmModule.getFunction(sharedPtrDrop);
		VERIFY(dtor);
		return dtor;
	}

	UNREACHABLE();
}

llvm::Value *CodeGenContext::getNullDestructor() {
	auto *functionType = getDestructorType();
	auto *functionPtrType = llvm::PointerType::getUnqual(functionType);
	return llvm::ConstantPointerNull::get(functionPtrType);
}

[[nodiscard]] llvm::FunctionType *CodeGenContext::getDestructorType() {
	llvm::Type *voidType = llvm::Type::getVoidTy(llvmContext);
	auto *const i8Type = llvm::Type::getInt8Ty(llvmContext);
	auto *const voidPtrType = llvm::PointerType::getUnqual(i8Type);
	return llvm::FunctionType::get(voidType, {voidPtrType}, false);
}

[[nodiscard]] llvm::Value *CodeGenContext::sizeOf(Type type) {
	auto *const llvmType = typeConverter.convert(type);
	const auto &layout = llvmModule.getDataLayout();
	auto *const sizeType = layout.getIntPtrType(llvmContext);
	const auto size = layout.getTypeAllocSize(llvmType);

	return llvm::ConstantInt::get(sizeType, size);
}
}
