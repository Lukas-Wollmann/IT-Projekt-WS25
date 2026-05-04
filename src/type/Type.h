#pragma once

#include <format>

#include "core/Macros.h"
#include "core/Typedef.h"
#include "core/U8String.h"

enum struct TypeKind : u8 { Primitive, Unit, Error, Null, Pointer, Function, Struct, Array };

enum struct PrimitiveKind : u8 { I32, Char, Bool };

struct TypeBase {
	const TypeKind kind;

	explicit TypeBase(TypeKind kind)
		: kind(kind) {}

	virtual ~TypeBase() = default;

	bool isTypeKind(TypeKind other) const {
		return kind == other;
	}

	virtual U8String str() const = 0;
	virtual bool equals(const TypeBase *other) const = 0;
	virtual Box<TypeBase> clone() const = 0;
};

using Type = TypeBase *;
using TypeList = Vec<Type>;

struct PrimitiveType : public TypeBase {
	const PrimitiveKind primitiveKind;

	explicit PrimitiveType(PrimitiveKind primitiveKind);

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;
};

struct UnitType : public TypeBase {
	UnitType();

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;
};

struct ErrorType : public TypeBase {
	ErrorType();

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;
};

struct NullType : public TypeBase {
	NullType();

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;
};

struct PointerType : public TypeBase {
	const Type pointeeType;

	explicit PointerType(Type pointeeType);

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;
};

struct FunctionType : public TypeBase {
	const TypeList paramTypes;
	const Type returnType;

	FunctionType(TypeList paramTypes, Type returnType);

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;
};

using StructField = Pair<U8String, Type>;

struct StructType : public TypeBase {
	const U8String name;
	Map<U8String, Type> fields;
	Vec<StructField> orderedFields;
	bool isDeclared;

	StructType(U8String name);

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;
};

struct ArrayType : public TypeBase {
	const Type elementType;
	const Opt<i32> size; // None = dynamic ([]T), Some(N) = fixed ([N]T)

	ArrayType(Type elementType, Opt<i32> size);

	U8String str() const override;
	bool equals(const TypeBase *other) const override;
	Box<TypeBase> clone() const override;

	bool isDynamic() const {
		return !size.has_value();
	}

	bool isFixed() const {
		return size.has_value();
	}
};

template <typename T>
	requires std::derived_from<T, TypeBase>
struct std::formatter<T> {
	constexpr auto parse(const std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const T &t, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "{}", t.str());
	}
};

template <>
struct std::formatter<Type> {
	constexpr auto parse(const std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const Type &t, std::format_context &ctx) const {
		return std::format_to(ctx.out(), "{}", t->str());
	}
};

template <>
struct std::formatter<TypeList> {
	constexpr auto parse(const std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const TypeList &list, std::format_context &ctx) const {
		auto out = ctx.out();
		for (size_t i = 0; i < list.size(); ++i) {
			if (i > 0) {
				out = std::format_to(out, ", ");
			}

			out = std::format_to(out, "{}", list[i]);
		}
		return out;
	}
};