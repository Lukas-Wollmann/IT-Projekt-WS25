#include "type_checker/SymbolTable.h"

SymbolInfo::SymbolInfo(TypePtr type)
    : m_Type(std::move(type))
{}

Scope &SymbolTable::pushScope()
{
    m_Scopes.push_back(Scope());

    return m_Scopes.back();
}

void SymbolTable::popScope()
{
    if (m_Scopes.empty())
        throw std::runtime_error("Called popScope on empty SymbolTable");

    m_Scopes.pop_back();
}

std::optional<Ref<const SymbolInfo>> SymbolTable::getSymbol(const std::string &name) const
{
    for (auto scope = m_Scopes.rbegin(); scope != m_Scopes.rend(); ++scope) 
    {
        auto it = scope->find(name);

        if (it != scope->end()) return it->second;
    }

    return std::nullopt;
}

void SymbolTable::addSymbol(std::string name, SymbolInfo symbol)
{
    if (m_Scopes.empty())
        throw std::runtime_error("Called addSymbol on empty SymbolTable");

    m_Scopes.back().emplace(std::move(name), std::move(symbol));
}


bool SymbolTable::hasSymbolInCurrentScope(const std::string name)
{
    if (m_Scopes.empty())
        throw std::runtime_error("Called hasSymbolInCurrentScope on empty SymbolTable");

    const Scope &current = m_Scopes.back();
    
    return current.find(name) != current.end();
}