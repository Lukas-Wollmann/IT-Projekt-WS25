#include "CompareVisitor.h"

namespace type {
	CompareVisitor::CompareVisitor(const Type &other)
		: m_Other(other) {}

	bool CompareVisitor::visit(const Typename &n) {
		if (m_Other.kind != TypeKind::Typename)
			return false;

		auto &other = static_cast<const Typename &>(m_Other);

		return n.typename_ == other.typename_;
	}

	bool CompareVisitor::visit(const PointerType &n) {
		if (m_Other.kind != TypeKind::Pointer)
			return false;

		const auto &other = static_cast<const PointerType &>(m_Other);

		return *n.pointeeType == *other.pointeeType;
	}

	bool CompareVisitor::visit(const ArrayType &n) {
		if (m_Other.kind != TypeKind::Array)
			return false;

		const auto &other = static_cast<const ArrayType &>(m_Other);

		if (n.arraySize != other.arraySize)
			return false;

		return *n.elementType == *other.elementType;
	}

	bool CompareVisitor::visit(const FunctionType &n) {
		if (m_Other.kind != TypeKind::Function)
			return false;

		const auto &other = static_cast<const FunctionType &>(m_Other);

		if (n.paramTypes.size() != other.paramTypes.size())
			return false;

		for (size_t i = 0; i < n.paramTypes.size(); ++i) {
			if (*n.paramTypes[i] != *other.paramTypes[i])
				return false;
		}

		return *n.returnType == *other.returnType;
	}

	bool CompareVisitor::visit(const ErrorType &) {
		return m_Other.kind == TypeKind::Error;
	}

	bool CompareVisitor::visit(const UnitType &) {
		return m_Other.kind == TypeKind::Unit;
	}
}

bool operator==(const type::Params &left, const type::Params &right) {
	if (left.size() != right.size())
		return false;

	for (size_t i = 0; i < left.size(); ++i) {
		if (*left[i] != *right[i])
			return false;
	}

	return true;
}

bool operator!=(const type::Params &left, const type::Params &right) {
	return !(left == right);
}

bool operator==(const type::Type &left, const type::Type &right) {
	return type::CompareVisitor(right).dispatch(left);
}

bool operator!=(const type::Type &left, const type::Type &right) {
	return !(left == right);
}