#pragma once
#include <memory>
#include <optional>

using TypePtr = std::unique_ptr<const struct Type>;


struct Type
{
    enum struct Kind { Primitive, Pointer, Array };

private:
    const Kind m_Kind;

protected:
    explicit Type(const Kind kind);

public:
    virtual ~Type() = default;

    virtual void toString(std::ostream &os) const = 0;
};

enum class PrimitiveKind { Int, Double, Bool, Char, String };

struct PrimitiveType : public Type
{
private:
    const PrimitiveKind m_PrimitiveKind;

public:
    explicit PrimitiveType(PrimitiveKind primitiveKind);

    virtual void toString(std::ostream &os) const override;
};


struct PointerType : public Type
{
private:
    const TypePtr m_PointeeType;

public:
    explicit PointerType(TypePtr pointeeType);

    virtual void toString(std::ostream &os) const override;
};


struct ArrayType : public Type
{
private:
    const TypePtr m_ElementType;
    const std::optional<size_t> m_ArraySize;

public:
    explicit ArrayType(TypePtr elementType, std::optional<size_t> arraySize = std::nullopt);

    virtual void toString(std::ostream &os) const override;
};


std::ostream &operator<<(std::ostream &os, PrimitiveKind primitiveKind);
std::ostream &operator<<(std::ostream &os, const Type &type);