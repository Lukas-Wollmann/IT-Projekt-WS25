#pragma once
#include "core/U8String.h"
#include "type/Type.h"

namespace sem {
using Scope = Map<U8String, Type>;

struct SymbolTable {
private:
	Vec<Scope> m_Scopes;

public:
	SymbolTable();
	SymbolTable(const SymbolTable &) = delete;
	SymbolTable(SymbolTable &&) = delete;

	SymbolTable &operator=(const SymbolTable &) = delete;
	SymbolTable &operator=(SymbolTable &&) = delete;

	Scope &enterScope();
	void exitScope();

	void addSymbol(U8String name, Type type);
	[[nodiscard]] Opt<Type> getSymbol(const U8String &name) const;
	[[nodiscard]] bool isSymbolDefinedInCurrentScope(const U8String &name) const;
};
}