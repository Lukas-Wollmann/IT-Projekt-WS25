#include "SymbolTable.h"

#include <ranges>

#include "type/Printer.h"

namespace semantic {
	using namespace type;

	SymbolTable::SymbolTable() {
		enterScope();
	}

	Scope &SymbolTable::enterScope() {
		m_Scopes.emplace_back();

		return m_Scopes.back();
	}

	void SymbolTable::exitScope() {
		VERIFY(!m_Scopes.empty());

		m_Scopes.pop_back();
	}

	void SymbolTable::addSymbol(U8String name, TypePtr type) {
		m_Scopes.back().emplace(std::move(name), std::move(type));
	}

	Opt<TypePtr> SymbolTable::getSymbol(const U8String &name) const {
		for (const auto &m_Scope : std::ranges::reverse_view(m_Scopes)) {
			if (const auto it = m_Scope.find(name); it != m_Scope.end())
				return it->second;
		}

		return {};
	}

	bool SymbolTable::isSymbolDefinedInCurrentScope(const U8String &name) const {
		VERIFY(!m_Scopes.empty());

		return m_Scopes.back().contains(name);
	}

	std::ostream &operator<<(std::ostream &os, const Scope &scope) {
		os << "====== Scope ======\n";

		for (auto &[name, type] : scope)
			os << name << ": " << *type << "\n";

		return os << "===================\n";
	}
}