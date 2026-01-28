#pragma once
#include "core/U8String.h"
#include "type/Type.h"

namespace semantic {
	using Scope = Map<U8String, type::TypePtr>;

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

		void addSymbol(U8String name, type::TypePtr type);
		[[nodiscard]] Opt<type::TypePtr> getSymbol(const U8String &name) const;
		[[nodiscard]] bool isSymbolDefinedInCurrentScope(const U8String &name) const;
	};
}