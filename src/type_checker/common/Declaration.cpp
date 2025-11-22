#include "Declaration.h"
#include <cassert>

FunctionDeclaration::FunctionDeclaration(std::string name, FunctionTypePtr type)
    : m_Name(std::move(name))
    , m_Type(std::move(type))
{
    assert(m_Type);
}

FunctionType &FunctionDeclaration::getType() const
{
    return *m_Type;
}

const std::string &FunctionDeclaration::getName() const
{
    return m_Name;
}