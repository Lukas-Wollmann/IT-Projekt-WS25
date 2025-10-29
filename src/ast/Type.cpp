#include "Type.h"


Type::Type(Kind kind)
    : m_Kind(kind)
{}

ValueType::ValueType(std::string typeName)
    : Type(Kind::Value)
    , m_Typename(typeName)
{}

void ValueType::toString(std::ostream &os) const
{
    os << m_Typename;
}

bool ValueType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &valueType = static_cast<const ValueType&>(other);

    return m_Typename == valueType.m_Typename;
}

TypePtr ValueType::copy() const
{
    return std::make_unique<ValueType>(m_Typename);
}

PointerType::PointerType(TypePtr pointeeType)
    : Type(Kind::Pointer)
    , m_PointeeType(std::move(pointeeType))
{}

void PointerType::toString(std::ostream &os) const
{
    os << "*" << *m_PointeeType;
}

bool PointerType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &ptrType = static_cast<const PointerType&>(other);

    return *m_PointeeType == *ptrType.m_PointeeType;
}

TypePtr PointerType::copy() const
{
    return std::make_unique<PointerType>(m_PointeeType->copy());
}

ArrayType::ArrayType(TypePtr elementType, std::optional<size_t> arraySize)
    : Type(Kind::Array)
    , m_ElementType(std::move(elementType))
    , m_ArraySize(arraySize)
{}

void ArrayType::toString(std::ostream &os) const
{
    os << "["; 

    if (m_ArraySize) os << *m_ArraySize;

    os << "]" << *m_ElementType;
}

bool ArrayType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &arrayType = static_cast<const ArrayType&>(other);

    if (m_ArraySize != arrayType.m_ArraySize) 
        return false;

    return *m_ElementType == *arrayType.m_ElementType;
}

TypePtr ArrayType::copy() const
{
    return std::make_unique<ArrayType>(m_ElementType->copy(), m_ArraySize);
}

FunctionType::FunctionType(TypeList parameterTypes, TypePtr returnType)
    : Type(Kind::Function)
    , m_ParameterTypes(std::move(parameterTypes))
    , m_ReturnType(std::move(returnType))
{}

void FunctionType::toString(std::ostream &os) const
{
    os << "(" << m_ParameterTypes << ")->(" << *m_ReturnType << ")";
}

bool FunctionType::equals(const Type &other) const
{
    if (getKind() != other.getKind()) 
        return false;

    auto &funcType = static_cast<const FunctionType&>(other);

    if (!m_ReturnType->equals(*funcType.m_ReturnType))
        return false;

    return m_ParameterTypes == funcType.m_ParameterTypes;
}

TypePtr FunctionType::copy() const 
{
    TypeList copiedParameters;
    copiedParameters.reserve(m_ParameterTypes.size());

    for (const TypePtr &typePtr : m_ParameterTypes) 
        copiedParameters.push_back(typePtr->copy()); 
        
    return std::make_unique<FunctionType>(std::move(copiedParameters), m_ReturnType->copy());
}

std::ostream &operator<<(std::ostream &os, const Type &type)
{
    type.toString(os);
    
    return os;
}

std::ostream &operator<<(std::ostream &os, const TypeList &typeList)
{
    for (size_t i = 0; i < typeList.size(); ++i)
    {
        if (i > 0) os << ",";
        
        os << *typeList[i];
    }

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

bool operator==(const TypeList &left, const TypeList &right)
{
    if (left.size() != right.size())
        return false;

    for (size_t i = 0; i < left.size(); ++i)
    {
        if (!left[i]->equals(*right[i]))
            return false;
    }

    return true;
}

bool operator!=(const TypeList &left, const TypeList &right)
{
    return !(left == right);
}