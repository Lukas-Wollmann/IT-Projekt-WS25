#pragma once
#include "Macros.h"
#include "Type.h"
#include "Typedef.h"

#include <iostream>

namespace type {
	///
	/// An interface for all traversing passes over types. The visitor
	/// uses a dispatch method to call the correct visit method for the
	/// corresponding type kind. The visited note can be modified.
	///
	template <typename T, bool IsConst = false>
	struct Visitor {
	public:
		virtual ~Visitor() = default;

		template <typename U>
		using Constness = std::conditional_t<IsConst, const U, U>;

		T dispatch(Constness<Type> &type) {
			switch (type.kind) {
				case TypeKind::Primitive:
					return visit(static_cast<Constness<PrimitiveType> &>(type));
				case TypeKind::Pointer:	 return visit(static_cast<Constness<PointerType> &>(type));
				case TypeKind::Array:	 return visit(static_cast<Constness<ArrayType> &>(type));
				case TypeKind::Function: return visit(static_cast<Constness<FunctionType> &>(type));
				case TypeKind::Error:	 return visit(static_cast<Constness<ErrorType> &>(type));
				case TypeKind::Unit:	 return visit(static_cast<Constness<UnitType> &>(type));
				default:                 UNREACHABLE();
			}
		}

		virtual T visit(Constness<PrimitiveType> &) { UNREACHABLE(); }

		virtual T visit(Constness<PointerType> &) { UNREACHABLE(); }

		virtual T visit(Constness<ArrayType> &) { UNREACHABLE(); }

		virtual T visit(Constness<FunctionType> &) { UNREACHABLE(); }

		virtual T visit(Constness<ErrorType> &) { UNREACHABLE(); }

		virtual T visit(Constness<UnitType> &) { UNREACHABLE(); }
	};

	///
	/// An interface for all traversing passes over types. The visitor
	/// uses a dispatch method to call the correct visit method for the
	/// corresponding type kind. The visited note can not be modified.
	///
	template <typename T>
	using ConstVisitor = Visitor<T, true>;
}