#include "type_checker/SymbolTable.h"

SymbolInfo::SymbolInfo(TypePtr type)
    : m_Type(std::move(type))
{}

Scope::Scope(WeakScopePtr parent) 
    : m_Parent(std::move(parent)) 
{}

ScopePtr Scope::enterScope()
{
    m_Children.emplace_back(std::make_shared<Scope>(shared_from_this()));
    
    return m_Children.back();
}

ScopePtr Scope::getParent() const 
{
    return m_Parent.lock();
}

void Scope::addSymbol(std::string name, SymbolInfo symbol)
{
    m_Symbols.emplace(std::move(name), std::move(symbol));
}

std::optional<Ref<const SymbolInfo>> Scope::getSymbol(const std::string &name) const
{
    auto it = m_Symbols.find(name);

    if (it != m_Symbols.end())
        return it->second;

    if (auto parent = m_Parent.lock()) 
        return parent->getSymbol(name);
    
    return std::nullopt;
}

bool Scope::isSymbolDefinedInThisScope(const std::string name)
{   
    return m_Symbols.find(name) != m_Symbols.end();
}

void Scope::toString(std::ostream &os, size_t indent) const
{
    std::string prefix(indent, ' ');
    
    os << prefix << "Scope {\n";

    for (const auto &[name, symbol] : m_Symbols)
        os << prefix << "  Symbol(" << name << ", " << symbol.getType() << ")\n";
    
    for (const ScopePtr &child : m_Children)
        child->toString(os, indent + 4);

    os << prefix << "}\n";
}

SymbolTable::SymbolTable()
    : m_GlobalScope(std::make_shared<Scope>())
    , m_Current(m_GlobalScope)
{}

ScopePtr SymbolTable::enterScope()
{
    return m_Current = m_Current->enterScope();    
}

void SymbolTable::exitScope()
{
    ScopePtr parent = m_Current->getParent();
    
    if (!parent)
        throw std::runtime_error("Cannot exit the global scope");

    m_Current = parent;
}

void SymbolTable::addSymbol(std::string name, SymbolInfo symbol)
{
    m_Current->addSymbol(std::move(name), std::move(symbol));
}

std::optional<Ref<const SymbolInfo>> SymbolTable::getSymbol(const std::string &name) const
{
    return m_Current->getSymbol(name);
}

bool SymbolTable::isSymbolDefinedInCurrentScope(const std::string name)
{
    return m_Current->isSymbolDefinedInThisScope(name);
}

ScopePtr SymbolTable::getGlobalScope() const
{
    return m_GlobalScope;
}