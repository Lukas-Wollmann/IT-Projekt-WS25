#include "Type.h"

Type::Type(Kind kind)
    : m_Kind(kind)
{}

ValueType::ValueType(std::string typeName)
    : Type(Kind::Primitive)
    , m_Typename(typeName)
{}

void ValueType::toString(std::ostream &os) const
{
    os << this->m_Typename;
}

bool ValueType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &valueType = static_cast<const ValueType&>(other);

    return m_Typename == valueType.m_Typename;
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

bool PointerType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &ptrType = static_cast<const PointerType&>(other);

    return m_PointeeType->equals(*ptrType.m_PointeeType);
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

bool ArrayType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &arrayType = static_cast<const ArrayType&>(other);

    if (m_ArraySize != arrayType.m_ArraySize) 
        return false;

    return m_ElementType->equals(*arrayType.m_ElementType);
}

FunctionType::FunctionType(TypeList parameterTypes, TypePtr returnType)
    : Type(Kind::Function)
    , m_ParameterTypes(std::move(parameterTypes))
    , m_ReturnType(std::move(returnType))
{}

void FunctionType::toString(std::ostream &os) const
{
    os << "(";

    for (size_t i = 0; i < m_ParameterTypes.size(); ++i)
    {
        if (i > 0) os << ",";
        
        m_ParameterTypes[i]->toString(os);
    }

    os << ")->(";
    m_ReturnType->toString(os);    
    os << ")";
}

bool FunctionType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &funcType = static_cast<const FunctionType&>(other);

    if (!m_ReturnType->equals(*funcType.m_ReturnType))
        return false;

    if (m_ParameterTypes.size() != funcType.m_ParameterTypes.size())
        return false;

    for (size_t i = 0; i < m_ParameterTypes.size(); ++i)
    {
        if (!m_ParameterTypes[i]->equals(*funcType.m_ParameterTypes[i]))
            return false;
    }

    return true;
}

std::ostream &operator<<(std::ostream &os, const Type &type)
{
    type.toString(os);
    
    return os;
}

bool operator==(const Type &left, const Type &right)
{
    return left.equals(right);
}

bool operator!=(const Type &left, const Type &right)
{
    return !(left == right);  
}