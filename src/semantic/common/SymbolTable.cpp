#include "SymbolTable.h"

#include "type/PrintVisitor.h"

namespace semantic {
    using namespace type;

	SymbolInfo::SymbolInfo(TypePtr type)
		: m_Type(std::move(type)) {}

    TypePtr SymbolInfo::getType() const {
		return m_Type;
	}

    SymbolTable::SymbolTable() {
        enterScope();
    }

	Scope &SymbolTable::enterScope() {
		m_Scopes.push_back({});

		return m_Scopes.back();
	}

	void SymbolTable::exitScope() {
		if (m_Scopes.empty())
			throw std::runtime_error("Cannot exit the global scope");

		m_Scopes.pop_back();
	}

	void SymbolTable::addSymbol(U8String name, SymbolInfo symbol) {
		m_Scopes.back().emplace(std::move(name), std::move(symbol));
	}

	Opt<Ref<const SymbolInfo>> SymbolTable::getSymbol(const U8String &name) const {
		for (auto scope = m_Scopes.rbegin(); scope != m_Scopes.rend(); ++scope) {
			auto it = scope->find(name);

			if (it != scope->end())
				return it->second;
		}

		return std::nullopt;
	}

	bool SymbolTable::isSymbolDefinedInCurrentScope(const U8String &name) const {
		const Scope &scope = m_Scopes.back();

		return scope.find(name) != scope.end();
	}
}

std::ostream &operator<<(std::ostream &os, const semantic::Scope &scope) {
	os << "====== Scope ======\n";

	for (auto &[name, info] : scope)
		os << name << ": " << info.getType() << "\n";

	return os << "===================\n";
}

std::ostream &operator<<(std::ostream &os, const semantic::SymbolTable &table) {
	for (auto &scope : table.m_Scopes)
		os << scope;

	return os;
}