#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "Typedef.h"

namespace type {
	enum struct TypeKind : u8 { Primitive, Pointer, Array, Function, Error, Unit };

	struct Type {
	public:
		const TypeKind kind;

		virtual ~Type() = default;

	protected:
		explicit Type(const TypeKind kind);
	};

	using Params = std::vector<Box<const Type>>;

	enum struct PrimitiveTypeKind : u8 { I32, U32, F32, String, Char, Bool };

	struct PrimitiveType : public Type {
	public:
		const PrimitiveTypeKind primitiveKind;

		explicit PrimitiveType(PrimitiveTypeKind primitiveKind);
	};

	struct PointerType : public Type {
	public:
		const Box<const Type> pointeeType;

		explicit PointerType(Box<const Type> pointeeType);
	};

	struct ArrayType : public Type {
	public:
		const Box<const Type> elementType;
		const std::optional<size_t> arraySize;

		explicit ArrayType(Box<const Type> elementType, Opt<size_t> arraySize = std::nullopt);
	};

	struct FunctionType : public Type {
	public:
		const Vec<Box<const Type>> paramTypes;
		const Box<const Type> returnType;

		FunctionType(Params param, Box<const Type> returnType);
	};

	struct ErrorType : public Type {
	public:
		ErrorType();
	};

	struct UnitType : public Type {
	public:
		UnitType();
	};
}

std::ostream &operator<<(std::ostream &os, type::TypeKind kind);
std::ostream &operator<<(std::ostream &os, type::PrimitiveTypeKind kind);