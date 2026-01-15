#pragma once
#include <iostream>
#include <unordered_map>

#include "core/U8String.h"
#include "type/Type.h"

namespace semantic {
	struct SymbolTable;
}

std::ostream &operator<<(std::ostream &os, const semantic::SymbolTable &table);

namespace semantic {
	using Scope = std::unordered_map<U8String, type::TypePtr>;

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

		friend std::ostream & ::operator<<(std::ostream &os, const SymbolTable &table);
	};

	std::ostream &operator<<(std::ostream &os, const Scope &scope);
}