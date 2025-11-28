#include "Namespace.h"
#include "type/PrintVisitor.h"
#include <cassert>

Namespace::Namespace(std::string name)
    : m_Name(name)
{}

void Namespace::addFunction(FunctionDeclaration func)
{   
    assert(m_Functions.find(func.getName()) == m_Functions.end());
    
    m_Functions.emplace(func.getName(), std::move(func));
}

std::optional<Ref<const FunctionDeclaration>> Namespace::getFunction(const std::string &name) const
{
    auto it = m_Functions.find(name);

    if (it != m_Functions.end())
        return it->second;
    
    return std::nullopt;  
}

std::ostream &operator<<(std::ostream &os, const Namespace &ns)
{
    os << "Namespace \"" << ns.m_Name << "\" {\n";

    for (auto &fn : ns.m_Functions)
        os << "    " << fn.first << ": " << fn.second.getType() << "\n";

    return os << "}\n";
}