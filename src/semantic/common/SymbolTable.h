#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>

#include "core/U8String.h"
#include "type/Type.h"

namespace semantic {
	struct SymbolTable;
}

std::ostream &operator<<(std::ostream &os, const semantic::SymbolTable &table);

namespace semantic {
	struct SymbolInfo {
	private:
		type::TypePtr m_Type;

	public:
		SymbolInfo(type::TypePtr type);
		SymbolInfo(const SymbolInfo &) = delete;
		SymbolInfo(SymbolInfo &&) = default;

		type::TypePtr getType() const;
	};

	using Scope = std::unordered_map<U8String, SymbolInfo>;

	struct SymbolTable {
	private:
		std::vector<Scope> m_Scopes;

	public:
		SymbolTable() = default;
		SymbolTable(const SymbolTable &) = delete;
		SymbolTable(SymbolTable &&) = delete;

		SymbolTable &operator=(const SymbolTable &) = delete;
		SymbolTable &operator=(SymbolTable &&) = delete;

		Scope &enterScope();
		void exitScope();

		void addSymbol(U8String name, SymbolInfo symbol);
		Opt<Ref<const SymbolInfo>> getSymbol(const U8String &name) const;
		bool isSymbolDefinedInCurrentScope(const U8String &name) const;

		friend std::ostream & ::operator<<(std::ostream &os, const SymbolTable &table);
	};
}

std::ostream &operator<<(std::ostream &os, const semantic::Scope &scope);