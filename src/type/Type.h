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
        bool isTypeKind(TypeKind other) const;

	protected:
		explicit Type(const TypeKind kind);
	};

	using TypePtr = Ptr<const Type>;
    using TypeList = Vec<TypePtr>;

	struct Typename : public Type {
	public:
		const U8String typename_;

		explicit Typename(U8String typename_);
	};

	struct PointerType : public Type {
	public:
		const Ptr<const Type> pointeeType;

		explicit PointerType(Ptr<const Type> pointeeType);
	};

	struct ArrayType : public Type {
	public:
		const Ptr<const Type> elementType;
		const Opt<size_t> arraySize;

		explicit ArrayType(Ptr<const Type> elementType, Opt<size_t> arraySize = std::nullopt);
	};

	struct FunctionType : public Type {
	public:
		const TypeList paramTypes;
		const Ptr<const Type> returnType;

		FunctionType(TypeList paramTypes, Ptr<const Type> returnType);
	};

    using FunctionTypePtr = Ptr<const FunctionType>;

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