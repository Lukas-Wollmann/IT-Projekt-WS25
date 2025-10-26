#include "Type.h"

Type::Type(Kind kind)
    : m_Kind(kind)
{}

PrimitiveType::PrimitiveType(PrimitiveKind primitiveKind)
    : Type(Kind::Primitive)
    , m_PrimitiveKind(primitiveKind)
{}

void PrimitiveType::toString(std::ostream &os) const
{
    os << this->m_PrimitiveKind;
}

PointerType::PointerType(TypePtr pointeeType)
    : Type(Kind::Pointer)
    , m_PointeeType(std::move(pointeeType))
{}

void PointerType::toString(std::ostream &os) const
{
    os << "*";
    this->m_PointeeType->toString(os);
}

ArrayType::ArrayType(TypePtr elementType, std::optional<size_t> arraySize)
    : Type(Kind::Array)
    , m_ElementType(std::move(elementType))
    , m_ArraySize(arraySize)
{}

void ArrayType::toString(std::ostream &os) const
{
    os << "[]";
    this->m_ElementType->toString(os);
}

std::ostream &operator<<(std::ostream &os, PrimitiveKind primitiveKind)
{
    switch (primitiveKind)
    {
    case PrimitiveKind::Int: 
        return os << "i32";
    case PrimitiveKind::Double: 
        return os << "f32";
    case PrimitiveKind::Bool: 
        return os << "bool";
    case PrimitiveKind::Char: 
        return os << "char";
    case PrimitiveKind::String: 
        return os << "string";
    }
}

std::ostream &operator<<(std::ostream &os, const Type &type)
{
    type.toString(os);
    
    return os;
}