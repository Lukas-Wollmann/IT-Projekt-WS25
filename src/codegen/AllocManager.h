#pragma once
#include "CodeGenContext.h"

namespace gen {
using Scope = Map<U8String, TrackedValue>;

struct AllocManager {
private:
	CodeGenContext &m_Context;
	Vec<Scope> m_Allocs;

public:
	explicit AllocManager(CodeGenContext &ctx);

public:
	Opt<TrackedValue> getAlloca(const U8String &ident) const;
	llvm::AllocaInst *createAlloca(Type type, const U8String &ident);
	void clearAllocas();
	void openScope();
	void closeScope();
	void emitCurrentScopeCleanup();
	void emitFullScopeCleanup();

private:
	void emitScopeCleanup(const Scope &scope);
};
}