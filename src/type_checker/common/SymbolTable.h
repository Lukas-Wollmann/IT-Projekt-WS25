#pragma once
#include <functional>
#include <iostream>
#include <unordered_map>

#include "core/U8String.h"
#include "type/Type.h"

struct SymbolInfo {
private:
	Box<const type::Type> m_Type;

public:
	SymbolInfo(Box<const type::Type> type);
	SymbolInfo(const SymbolInfo &) = delete;
	SymbolInfo(SymbolInfo &&) = default;

	const type::Type &getType() const {
		return *m_Type;
	}
};

using Scope = std::unordered_map<U8String, SymbolInfo>;

std::ostream &operator<<(std::ostream &os, const Scope &scope);

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
	std::optional<Ref<const SymbolInfo>> getSymbol(const U8String &name) const;
	bool isSymbolDefinedInCurrentScope(const U8String &name) const;

	friend std::ostream &operator<<(std::ostream &os, const SymbolTable &table);
};