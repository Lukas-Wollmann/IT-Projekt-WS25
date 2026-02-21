#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "ast/Visitor.h"
#include "codegen/TypeConverter.h"
#include "type/Compare.h"

struct TrackedValue {
	llvm::Value *value;
	type::TypePtr type;
};

struct LoweringContext {
private:
	llvm::LLVMContext m_LLVMContext;
	Box<llvm::Module> m_LLVMModule;
	llvm::IRBuilder<> m_IRBuilder;
	gen::TypeConverter m_Converter;
	Vec<Pair<Map<U8String, TrackedValue>, bool>> m_Allocas;

public:
	explicit LoweringContext(const U8String &moduleName)
		: m_LLVMContext()
		, m_LLVMModule(std::make_unique<llvm::Module>(moduleName.asAscii(), m_LLVMContext))
		, m_IRBuilder(m_LLVMContext)
		, m_Converter(m_LLVMContext) {}

	llvm::IRBuilder<> &getIRBuilder() {
		return m_IRBuilder;
	}

	llvm::LLVMContext &getLLVMContext() {
		return m_LLVMContext;
	}

	llvm::Module &getLLVMModule() const {
		return *m_LLVMModule;
	}

	gen::TypeConverter &getTypeConverter() {
		return m_Converter;
	}

	Opt<TrackedValue> getAlloca(const U8String &ident) {
		for (auto it = m_Allocas.rbegin(); it != m_Allocas.rend(); ++it) {
			auto &currentScope = *it;
			auto found = currentScope.first.find(ident);

			if (found != currentScope.first.end()) {
				return found->second;
			}
		}

		return {};
	}

	llvm::AllocaInst *createAlloca(type::TypePtr type, const U8String &ident) {
		auto func = m_IRBuilder.GetInsertBlock()->getParent();
		VERIFY(func);

		llvm::IRBuilder<> tmp(&func->getEntryBlock(), func->getEntryBlock().begin());

		const auto llvmType = m_Converter.convertType(type);
		auto alloca = tmp.CreateAlloca(llvmType, nullptr, ident.asAscii());

		m_Allocas.back().first.emplace(ident, TrackedValue{alloca, type});

		return alloca;
	}

	void clearAllocas() {
		m_Allocas.clear();
	}

	void openScope(bool cleanup = true) {
		m_Allocas.emplace_back(Pair<Map<U8String, TrackedValue>, bool>{{}, cleanup});
	}

	void closeScope() {
		m_Allocas.pop_back();
	}

	void emitScopeCleanup() {
		const auto &scope = m_Allocas.back();

		if (!scope.second) {
			return;
		}

		for (const auto &[_, tracked] : scope.first) {
			auto ptrTy = m_IRBuilder.getPtrTy();
			auto value = m_IRBuilder.CreateLoad(ptrTy, tracked.value);
			dropValue(value, tracked.type);
		}
	}

	void emitFullScopeCleanup() {
		for (const auto &[scope, cleanup] : m_Allocas) {
			if (!cleanup) {
				continue;
			}

			for (const auto &[_, tracked] : scope) {
				auto ptrTy = m_IRBuilder.getPtrTy();
				auto value = m_IRBuilder.CreateLoad(ptrTy, tracked.value);
				dropValue(value, tracked.type);
			}
		}
	}

	llvm::Value *copyValue(llvm::Value *value, const type::TypePtr &type) {
		if (type->isTypeKind(type::TypeKind::Unit)) {
			return value;
		}

		if (type->isTypeKind(type::TypeKind::Typename)) {
			return value; // TODO emit copy ctor
		}

		if (type->isTypeKind(type::TypeKind::Pointer)) {
			// For pointers, we need to increment the reference count
			auto *const func = m_LLVMModule->getFunction("__sp_copy");
			VERIFY(func);
			auto *const result = m_IRBuilder.CreateCall(func, {value});

			return result;
		}

		UNREACHABLE();
	}

	[[nodiscard]] constexpr llvm::FunctionType *getDtorType() {
		llvm::Type *voidType = llvm::Type::getVoidTy(m_LLVMContext);
		auto *const i8Type = llvm::Type::getInt8Ty(m_LLVMContext);
		auto *const voidPtrType = llvm::PointerType::getUnqual(i8Type); // i8*
		return llvm::FunctionType::get(voidType, {voidPtrType}, false);
	}

	[[nodiscard]] constexpr llvm::Value *getNullDtor() {
		auto *functionType = getDtorType();
		auto *functionPtrType = llvm::PointerType::getUnqual(functionType);
		return llvm::ConstantPointerNull::get(functionPtrType);
	}

	[[nodiscard]] Opt<llvm::Value *> getDtor(const type::TypePtr &type) {
		if (type->isTypeKind(type::TypeKind::Unit)) {
			return {};
		}

		if (type->isTypeKind(type::TypeKind::Typename)) {
			return {}; // TODO recursive for structs / emit dtor
		}

		if (type->isTypeKind(type::TypeKind::Pointer)) {
			auto *const dtor = m_LLVMModule->getFunction("__sp_drop");
			VERIFY(dtor);
			return dtor;
		}

		UNREACHABLE();
	}

	void dropValue(llvm::Value *value, const type::TypePtr &type) {
		if (auto dtor = getDtor(type)) {
			m_IRBuilder.CreateCall(getDtorType(), dtor.value(), {value});
		}
	}

	[[nodiscard]] llvm::Value *sizeOf(const type::TypePtr &type) {
		auto *const llvmType = m_Converter.convertType(type);
		const auto &layout = m_LLVMModule->getDataLayout();
		auto *const sizeType = layout.getIntPtrType(m_LLVMContext);
		const auto size = layout.getTypeAllocSize(llvmType);

		return llvm::ConstantInt::get(sizeType, size);
	}
};