#include "Compare.h"

namespace type {
	Compare::Compare(const Type &other)
		: m_Other(other) {}

	bool Compare::visit(const Typename &n) {
		if (m_Other.kind != TypeKind::Typename)
			return false;

		const auto &other = dynamic_cast<const Typename &>(m_Other);

		return n.typename_ == other.typename_;
	}

	bool Compare::visit(const PointerType &n) {
		if (m_Other.kind != TypeKind::Pointer)
			return false;

		const auto &other = dynamic_cast<const PointerType &>(m_Other);

		return *n.pointeeType == *other.pointeeType;
	}

	bool Compare::visit(const ArrayType &n) {
		if (m_Other.kind != TypeKind::Array)
			return false;

		const auto &other = dynamic_cast<const ArrayType &>(m_Other);

		return *n.elementType == *other.elementType;
	}

	bool Compare::visit(const FunctionType &n) {
		if (m_Other.kind != TypeKind::Function)
			return false;

		const auto &other = dynamic_cast<const FunctionType &>(m_Other);

		if (n.paramTypes.size() != other.paramTypes.size())
			return false;

		for (size_t i = 0; i < n.paramTypes.size(); ++i) {
			if (*n.paramTypes[i] != *other.paramTypes[i])
				return false;
		}

		return *n.returnType == *other.returnType;
	}

	bool Compare::visit(const ErrorType &) {
		return m_Other.kind == TypeKind::Error;
	}

	bool Compare::visit(const UnitType &) {
		return m_Other.kind == TypeKind::Unit;
	}

	bool operator==(const TypeList &left, const TypeList &right) {
		if (left.size() != right.size())
			return false;

		for (size_t i = 0; i < left.size(); ++i) {
			if (*left[i] != *right[i])
				return false;
		}

		return true;
	}

	bool operator!=(const TypeList &left, const TypeList &right) {
		return !(left == right);
	}

	bool operator==(const Type &left, const Type &right) {
		return Compare(right).dispatch(left);
	}

	bool operator!=(const Type &left, const Type &right) {
		return !(left == right);
	}
}