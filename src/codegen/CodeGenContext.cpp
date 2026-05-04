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
	llvm::FunctionType *arrayCreateTy =
			llvm::FunctionType::get(ptrTy, {sizeTy, sizeTy, ptrTy}, false);
	llvm::FunctionType *arrayCopyTy = spCopyTy;
	llvm::FunctionType *arrayDropTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);

	llvmModule.getOrInsertFunction(sharedPtrCreate, spCreateTy);
	llvmModule.getOrInsertFunction(sharedPtrCopy, spCopyTy);
	llvmModule.getOrInsertFunction(sharedPtrDrop, spDropTy);

	llvmModule.getOrInsertFunction(arrayCreate, arrayCreateTy);
	llvmModule.getOrInsertFunction(arrayCopy, arrayCopyTy);
	llvmModule.getOrInsertFunction(arrayDrop, arrayDropTy);
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

		// FIX: Call the dedicated array copy function directly
		auto *copyArrFunc = llvmModule.getFunction(arrayCopy);
		VERIFY(copyArrFunc);
		auto *copiedDataPtr = irBuilder.CreateCall(copyArrFunc, {dataPtr});

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
		// Extract the raw data pointer from the fat pointer (struct { T* data, size_t len })
		auto *dataPtr = irBuilder.CreateExtractValue(value, 0U);
		auto *dropArrFunc = llvmModule.getFunction(arrayDrop);
		VERIFY(dropArrFunc);
		irBuilder.CreateCall(dropArrFunc, {dataPtr});
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

	if (type->isTypeKind(TypeKind::Array)) {
		constexpr auto arrayDtorName = "__dtor_array";
		auto *dtor = llvmModule.getFunction(arrayDtorName);

		if (!dtor) {
			auto oldIP = irBuilder.saveIP();
			auto *fnType = getDestructorType(); // void(void*)
			dtor = llvm::Function::Create(fnType, llvm::Function::ExternalLinkage, arrayDtorName,
										  llvmModule);

			auto *entry = llvm::BasicBlock::Create(llvmContext, "entry", dtor);
			irBuilder.SetInsertPoint(entry);

			// 'payload' is a pointer to the fat pointer: [T* data, size_t len]*
			auto *payload = dtor->arg_begin();

			// 1. Cast payload to the actual LLVM array struct type pointer
			auto *llvmArrayType = typeConverter.convert(type);
			auto *arrayStructPtr =
					irBuilder.CreateBitCast(payload, llvm::PointerType::getUnqual(llvmArrayType));

			// 2. Extract the data pointer (field 0 of the fat pointer)
			// We GEP to the first field and load it.
			auto *dataPtrAddress = irBuilder.CreateStructGEP(llvmArrayType, arrayStructPtr, 0U);
			auto *sharedPtrToData = irBuilder.CreateLoad(irBuilder.getPtrTy(), dataPtrAddress);

			// 3. Call the array-aware drop function
			auto *dropArr = llvmModule.getFunction(arrayDrop);
			VERIFY(dropArr);
			irBuilder.CreateCall(dropArr, {sharedPtrToData});

			irBuilder.CreateRetVoid();
			irBuilder.restoreIP(oldIP);
		}

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
