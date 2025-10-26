#pragma once
#include <memory>
#include <optional>
#include <vector>

using TypePtr = std::unique_ptr<const struct Type>;
using TypeList = std::vector<TypePtr>;

struct Type 
{
    friend std::ostream &operator<<(std::ostream &os, const Type &type);
    friend bool operator==(const Type &left, const Type &right);
    friend bool operator!=(const Type &left, const Type &right);
    
    enum struct Kind { Primitive, Pointer, Array, Function };

private:
    const Kind m_Kind;

protected:
    explicit Type(const Kind kind);

public:
    virtual ~Type() = default;

    virtual void toString(std::ostream &os) const = 0;
    virtual bool equals(const Type &other) const = 0;

    Kind getKind() const { return m_Kind; }
};


struct ValueType : public Type
{
private:
    const std::string m_Typename;

public:
    explicit ValueType(std::string typeName);

    virtual void toString(std::ostream &os) const override;
    virtual bool equals(const Type &other) const override;
};


struct PointerType : public Type
{
private:
    const TypePtr m_PointeeType;

public:
    explicit PointerType(TypePtr pointeeType);

    virtual void toString(std::ostream &os) const override;
    virtual bool equals(const Type &other) const override;
};


struct ArrayType : public Type
{
private:
    const TypePtr m_ElementType;
    const std::optional<size_t> m_ArraySize;

public:
    explicit ArrayType(TypePtr elementType, std::optional<size_t> arraySize = std::nullopt);

    virtual void toString(std::ostream &os) const override;
    virtual bool equals(const Type &other) const override;
};


struct FunctionType : public Type
{
private:
    const TypeList m_ParameterTypes;
    const TypePtr m_ReturnType;

public:
    explicit FunctionType(TypeList parameterTypes, TypePtr returnType);

    virtual void toString(std::ostream &os) const override;
    virtual bool equals(const Type &other) const override;
};