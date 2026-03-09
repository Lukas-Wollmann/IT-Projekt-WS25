#pragma once

#include "core/Macros.h"
#include "core/Typedef.h"
#include "core/U8String.h"

struct TypeBase;

using Type = const TypeBase *;
using TypeList = Vec<Type>;

enum struct TypeKind : u8 { Primitive, Unit, Error, Pointer, Array, Function, Struct };

enum struct PrimitiveKind : u8 { I32, Char, Bool };

struct TypeBase {
	friend struct TypeFactory;

	const TypeKind kind;

protected:
	explicit TypeBase(TypeKind kind)
		: kind(kind) {}

public:
	virtual ~TypeBase() = default;

	bool isTypeKind(TypeKind other) const {
		return kind == other;
	}

	virtual U8String str() const = 0;
	virtual bool equals(Type other) const = 0;
	virtual Box<const TypeBase> clone() const = 0;
};

struct PrimitiveType : public TypeBase {
	friend struct TypeFactory;

	const PrimitiveKind primitiveKind;

private:
	explicit PrimitiveType(PrimitiveKind primitiveKind);

public:
	U8String str() const override;
	bool equals(Type other) const override;
	Box<const TypeBase> clone() const override;
};

struct UnitType : public TypeBase {
	friend struct TypeFactory;

private:
	UnitType();

public:
	U8String str() const override;
	bool equals(Type other) const override;
	Box<const TypeBase> clone() const override;
};

struct ErrorType : public TypeBase {
	friend struct TypeFactory;

private:
	ErrorType();

public:
	U8String str() const override;
	bool equals(Type other) const override;
	Box<const TypeBase> clone() const override;
};

struct PointerType : public TypeBase {
	friend struct TypeFactory;

	const Type pointeeType;

private:
	explicit PointerType(Type pointeeType);

public:
	U8String str() const override;
	bool equals(Type other) const override;
	Box<const TypeBase> clone() const override;
};

struct FunctionType : public TypeBase {
	friend struct TypeFactory;

	const TypeList paramTypes;
	const Type returnType;

private:
	FunctionType(TypeList paramTypes, Type returnType);

public:
	U8String str() const override;
	bool equals(Type other) const override;
	Box<const TypeBase> clone() const override;
};

using StructField = Pair<U8String, Type>;

struct StructType : public TypeBase {
	friend struct TypeFactory;

	const U8String name;
	const Vec<StructField> fields;

private:
	StructType(U8String name, Vec<StructField> fields);

public:
	U8String str() const override;
	bool equals(Type other) const override;
	Box<const TypeBase> clone() const override;
};

template <typename T>
	requires std::derived_from<T, TypeBase>
struct std::formatter<T> {
	constexpr auto parse(const std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const T &t, std::format_context &ctx) const {
		return t.str();
	}
};

template <>
struct std::formatter<TypeList> {
	constexpr auto parse(const std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const TypeList &l, std::format_context &ctx) const {
		for (size_t i = 0; i < l.size(); ++i) {
			if (i)
				std::format_to(ctx.out(), ", ");

			std::format_to(ctx.out(), "{}", *l[i]);
		}

		return ctx.out();
	}
};