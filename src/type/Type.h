#pragma once
#include "core/Typedef.h"
#include "core/U8String.h"

namespace type {
	enum struct TypeKind : u8 { Typename, Pointer, Array, Function, Error, Unit };

	struct Type {
		const TypeKind kind;

		virtual ~Type() = default;
		[[nodiscard]] bool isTypeKind(TypeKind other) const;

	protected:
		explicit Type(TypeKind kind);
	};

	using TypePtr = Ptr<const Type>;
	using TypeList = Vec<TypePtr>;

	struct Typename : Type {
		const U8String typename_;

		explicit Typename(U8String typename_);
	};

	struct PointerType : Type {
		const TypePtr pointeeType;

		explicit PointerType(TypePtr pointeeType);
	};

	struct ArrayType : Type {
		const TypePtr elementType;

		explicit ArrayType(TypePtr elementType);
	};

	struct FunctionType : Type {
		const TypeList paramTypes;
		const TypePtr returnType;

		FunctionType(TypeList paramTypes, TypePtr returnType);
	};

	using FunctionTypePtr = Ptr<const FunctionType>;

	struct ErrorType : Type {
		ErrorType();
	};

	struct UnitType : Type {
		UnitType();
	};
}
