#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "Typedef.h"
#include "core/U8String.h"

namespace type {
	enum struct TypeKind : u8 { Typename, Pointer, Array, Function, Error, Unit };

	struct Type {
	public:
		const TypeKind kind;

		virtual ~Type() = default;

	protected:
		explicit Type(const TypeKind kind);
	};

	struct Typename : public Type {
	public:
		const U8String typename_;

		explicit Typename(U8String typename_);
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

		explicit ArrayType(Box<const Type> elementType,
						   std::optional<size_t> arraySize = std::nullopt);
	};

	struct FunctionType : public Type {
	public:
		const Vec<Box<const Type>> paramTypes;
		const Box<const Type> returnType;

		FunctionType(Vec<Box<const Type>> paramTypes, Box<const Type> returnType);
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