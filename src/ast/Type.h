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
    virtual TypePtr copy() const = 0;

    Kind getKind() const { return m_Kind; }
};


struct ValueType : public Type
{
private:
    const std::string m_Typename;

public:
    explicit ValueType(std::string typeName);

    void toString(std::ostream &os) const override;
    bool equals(const Type &other) const override;
    TypePtr copy() const override;

    const std::string &getTypename() const { return m_Typename; }
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