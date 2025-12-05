#include "PrintVisitor.h"

namespace type {
	PrintVisitor::PrintVisitor(std::ostream &os)
		: m_OStream(os) {}

	void PrintVisitor::visit(const PrimitiveType &n) {
		m_OStream << n.primitiveKind;
	}

	void PrintVisitor::visit(const PointerType &n) {
		m_OStream << "*" << *n.pointeeType;
	}

	void PrintVisitor::visit(const ArrayType &n) {
		m_OStream << "[";

		if (n.arraySize)
			m_OStream << *n.arraySize;

		m_OStream << "]" << *n.elementType;
	}

	void PrintVisitor::visit(const FunctionType &n) {
		m_OStream << "(";

		for (size_t i = 0; i < n.paramTypes.size(); ++i)
			m_OStream << (i ? ", " : "") << *n.paramTypes[i];

		m_OStream << ")->(" << *n.returnType << ")";
	}

	void PrintVisitor::visit(const ErrorType &) {
		m_OStream << "<error-type>";
	}

	void PrintVisitor::visit(const UnitType &) {
		m_OStream << "()";
	}
}

std::ostream &operator<<(std::ostream &os, const type::Type &n) {
	type::PrintVisitor(os).dispatch(n);
	return os;
}