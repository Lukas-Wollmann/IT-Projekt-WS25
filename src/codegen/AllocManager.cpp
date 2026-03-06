#include "AllocManager.h"

#include <ranges>

namespace gen {
AllocManager::AllocManager(CodeGenContext &ctx)
	: m_Context(ctx) {}

Opt<TrackedValue> AllocManager::getAlloca(const U8String &ident) const {
	for (const auto &scope : std::ranges::reverse_view(m_Allocs)) {
		if (auto it = scope.find(ident); it != scope.end()) {
			return it->second;
		}
	}
	return {};
}

llvm::AllocaInst *AllocManager::createAlloca(type::TypePtr type, const U8String &ident) {
	auto *func = m_Context.irBuilder.GetInsertBlock()->getParent();
	VERIFY(func);

	llvm::IRBuilder<> entryBuilder(&func->getEntryBlock(), func->getEntryBlock().begin());

	auto *llvmType = m_Context.typeConverter.convert(type);
	auto *alloca = entryBuilder.CreateAlloca(llvmType, nullptr, ident.asAscii());

	m_Allocs.back().emplace(ident, TrackedValue{alloca, type});

	return alloca;
}

void AllocManager::clearAllocas() {
	m_Allocs.clear();
}

void AllocManager::openScope() {
	m_Allocs.emplace_back(Map<U8String, TrackedValue>{});
}

void AllocManager::closeScope() {
	m_Allocs.pop_back();
}

void AllocManager::emitCurrentScopeCleanup() {
	emitScopeCleanup(m_Allocs.back());
}

void AllocManager::emitFullScopeCleanup() {
	for (const auto &scope : std::ranges::reverse_view(m_Allocs)) {
		emitScopeCleanup(scope);
	}
}

void AllocManager::emitScopeCleanup(const Scope &scope) {
	// All allocations store a pointer to the value that needs to be dropped
	// so we need to load the value first and then emit the actual cleanup.
	for (const auto &[_, tracked] : scope) {
		auto *llvmType = m_Context.typeConverter.convert(tracked.type);
		auto *value = m_Context.irBuilder.CreateLoad(llvmType, tracked.value);

		m_Context.dropValue(value, tracked.type);
	}
}
}