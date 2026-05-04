#include "CodeGenContext.h"

#include <llvm/TargetParser/Host.h>

#include <ranges>

#include "type/TypeFactory.h"

namespace gen {
U8String getStructDtorName(const U8String &name) {
	return u8"__dtor_" + name;
}

CodeGenContext::CodeGenContext(const U8String &moduleName)
	: irBuilder(llvmContext)
	, llvmModule(moduleName.asAscii(), llvmContext)
	, typeConverter(llvmContext) {
	llvmModule.setTargetTriple(llvm::sys::getDefaultTargetTriple());
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
	if (type->isTypeKind(TypeKind::Unit) || type->isTypeKind(TypeKind::Primitive) ||
		type->isTypeKind(TypeKind::Null) || type->isTypeKind(TypeKind::Function)) {
		return value;
	}

	if (type->isTypeKind(TypeKind::Struct)) {
		auto *structType = static_cast<StructType *>(type);
		auto *llvmStructType = static_cast<llvm::StructType *>(typeConverter.convert(type));

		llvm::Value *result = llvm::UndefValue::get(llvmStructType);

		for (u32 i = 0; i < structType->orderedFields.size(); ++i) {
			const auto &[_, fieldType] = structType->orderedFields[i];
			auto *fieldValue = irBuilder.CreateExtractValue(value, {i});
			auto *copiedField = copyValue(fieldValue, fieldType);
			result = irBuilder.CreateInsertValue(result, copiedField, {i});
		}

		return result;
	}

	if (type->isTypeKind(TypeKind::Array)) {
		// Fat pointer: extract both data and size, copy the smart pointer, rebuild
		auto *arrayType = static_cast<ArrayType *>(type);
		auto *dataPtr = irBuilder.CreateExtractValue(value, 0U);
		auto *sizeVal = irBuilder.CreateExtractValue(value, 1U);

		auto *elemPtrType = TypeFactory::getPointer(arrayType->elementType);
		auto *copiedDataPtr = copyValue(dataPtr, elemPtrType);

		auto *llvmArrayType = typeConverter.convert(type);
		llvm::Value *result = llvm::UndefValue::get(llvmArrayType);
		result = irBuilder.CreateInsertValue(result, copiedDataPtr, 0U);
		result = irBuilder.CreateInsertValue(result, sizeVal, 1U);

		return result;
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
	if (type->isTypeKind(TypeKind::Unit) || type->isTypeKind(TypeKind::Primitive) ||
		type->isTypeKind(TypeKind::Null) || type->isTypeKind(TypeKind::Error) ||
		type->isTypeKind(TypeKind::Function)) {
		return;
	}

	if (type->isTypeKind(TypeKind::Pointer)) {
		auto *drop = llvmModule.getFunction(sharedPtrDrop);
		VERIFY(drop);
		irBuilder.CreateCall(drop, {value});
		return;
	}

	if (type->isTypeKind(TypeKind::Array)) {
		// Fat pointer: extract the embedded smart pointer (field 0) and drop it
		auto *arrayType = static_cast<ArrayType *>(type);
		auto *dataPtr = irBuilder.CreateExtractValue(value, 0U);
		auto *elemPtrType = TypeFactory::getPointer(arrayType->elementType);
		dropValue(dataPtr, elemPtrType);
		return;
	}

	if (type->isTypeKind(TypeKind::Struct)) {
		auto *func = irBuilder.GetInsertBlock()->getParent();
		VERIFY(func);

		// Materialize the rvalue in a stack slot so the generated struct destructor
		// can recursively drop members through a stable address.
		llvm::IRBuilder<> entryBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());
		auto *llvmStructType = typeConverter.convert(type);
		auto *tmpAlloca = entryBuilder.CreateAlloca(llvmStructType, nullptr, "tmp.struct.drop");
		irBuilder.CreateStore(value, tmpAlloca);

		auto dtor = getDestructor(type);
		VERIFY(dtor.has_value());
		auto *payload = irBuilder.CreateBitCast(tmpAlloca, irBuilder.getPtrTy());
		irBuilder.CreateCall(getDestructorType(), dtor.value(), {payload});
		return;
	}

	UNREACHABLE();
}

Opt<llvm::Value *> CodeGenContext::getDestructor(Type type) {
	if (type->isTypeKind(TypeKind::Unit) || type->isTypeKind(TypeKind::Primitive) ||
		type->isTypeKind(TypeKind::Null) || type->isTypeKind(TypeKind::Error) ||
		type->isTypeKind(TypeKind::Function)) {
		return {};
	}

	if (type->isTypeKind(TypeKind::Struct)) {
		auto *structType = static_cast<StructType *>(type);
		auto dtorName = getStructDtorName(structType->name);
		auto *dtor = llvmModule.getFunction(dtorName.asAscii());
		VERIFY(dtor);
		return dtor;
	}

	if (type->isTypeKind(TypeKind::Pointer)) {
		constexpr auto ptrDtorName = "__dtor_ptr";
		auto *dtor = llvmModule.getFunction(ptrDtorName);

		if (!dtor) {
			auto oldIP = irBuilder.saveIP();
			auto *fnType = getDestructorType();
			dtor = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, ptrDtorName,
										  llvmModule);

			auto *entry = llvm::BasicBlock::Create(llvmContext, "entry", dtor);
			irBuilder.SetInsertPoint(entry);

			auto *payload = dtor->arg_begin();
			payload->setName("payload");

			auto *payloadAsPtrPtr =
					irBuilder.CreateBitCast(payload,
											llvm::PointerType::getUnqual(irBuilder.getPtrTy()));
			auto *sharedPtrValue = irBuilder.CreateLoad(irBuilder.getPtrTy(), payloadAsPtrPtr);

			auto *drop = llvmModule.getFunction(sharedPtrDrop);
			VERIFY(drop);
			irBuilder.CreateCall(drop, {sharedPtrValue});
			irBuilder.CreateRetVoid();
			irBuilder.restoreIP(oldIP);
		}

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
