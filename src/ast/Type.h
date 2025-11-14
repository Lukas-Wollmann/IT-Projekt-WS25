#pragma once
#include <memory>
#include <optional>
#include <vector>
#include "Typedef.h"

using TypePtr = std::unique_ptr<const struct Type>;
using TypeList = std::vector<TypePtr>;

enum struct TypeKind : u8
{ 
    Primitive, 
    Pointer, 
    Array, 
    Function, 
    Error 
};

struct Type 
{
private:
    const TypeKind m_Kind;

protected:
    explicit Type(const TypeKind kind);

public:
    virtual ~Type() = default;

    virtual void toString(std::ostream &os) const = 0;
    virtual bool equals(const Type &other) const = 0;
    virtual TypePtr copy() const = 0;

    TypeKind getKind() const { return m_Kind; }
};

enum struct PrimitiveTypeKind : u8 
{ 
    I32, 
    U32, 
    F32, 
    String, 
    Char, 
    Bool 
};

struct PrimitiveType : public Type
{
private:
    const PrimitiveTypeKind m_Primitive;

public:
    explicit PrimitiveType(PrimitiveTypeKind primitive);

    void toString(std::ostream &os) const override;
    bool equals(const Type &other) const override;
    TypePtr copy() const override;

    PrimitiveTypeKind getPrimitive() const { return m_Primitive; }
};

struct PointerType : public Type
{
private:
    const TypePtr m_PointeeType;

public:
    explicit PointerType(TypePtr pointeeType);

    void toString(std::ostream &os) const override;
    bool equals(const Type &other) const override;
    TypePtr copy() const override;

    const Type &getPointeeType() const { return *m_PointeeType; }
};

struct ArrayType : public Type
{
private:
    const TypePtr m_ElementType;
    const std::optional<size_t> m_ArraySize;

public:
    explicit ArrayType(TypePtr elementType, std::optional<size_t> arraySize = std::nullopt);

    void toString(std::ostream &os) const override;
    bool equals(const Type &other) const override;
    TypePtr copy() const override;

    const Type &getElementType() const { return *m_ElementType; }
    std::optional<size_t> getArraySize() const { return m_ArraySize; }
};

struct FunctionType : public Type
{
private:
    const TypeList m_ParameterTypes;
    const TypePtr m_ReturnType;

public:
    explicit FunctionType(TypeList parameterTypes, TypePtr returnType);

    void toString(std::ostream &os) const override;
    bool equals(const Type &other) const override;
    TypePtr copy() const override;

    const TypeList &getParameterTypes() const { return m_ParameterTypes; }
    const Type &getReturnType() const { return *m_ReturnType; }
};

struct ErrorType : public Type
{
public:
    explicit ErrorType();

    void toString(std::ostream &os) const override;
    bool equals(const Type &other) const override;
    TypePtr copy() const override; 
};

std::ostream &operator<<(std::ostream &os, const Type &type);
std::ostream &operator<<(std::ostream &os, const TypeList &typeList);
std::ostream &operator<<(std::ostream &os, PrimitiveTypeKind primitive);

bool operator==(const Type &left, const Type &right);
bool operator!=(const Type &left, const Type &right);
bool operator==(const TypeList &left, const TypeList &right);
bool operator!=(const TypeList &left, const TypeList &right);