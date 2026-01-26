#include "Printer.h"

#include <sstream>

namespace type {
	Printer::Printer(std::ostream &os)
		: m_OStream(os) {}

	void Printer::visit(const Typename &n) {
		m_OStream << n.typename_;
	}

	void Printer::visit(const PointerType &n) {
		m_OStream << "*" << *n.pointeeType;
	}

	void Printer::visit(const ArrayType &n) {
		m_OStream << "[]" << *n.elementType;
	}

	void Printer::visit(const FunctionType &n) {
		m_OStream << "(" << n.paramTypes << ")->(" << *n.returnType << ")";
	}

	void Printer::visit(const ErrorType &) {
		m_OStream << "<error-type>";
	}

	void Printer::visit(const UnitType &) {
		m_OStream << "()";
	}

	std::ostream &operator<<(std::ostream &os, const TypeList &p) {
		for (size_t i = 0; i < p.size(); ++i)
			os << (i ? ", " : "") << *p[i];

		return os;
	}

	std::ostream &operator<<(std::ostream &os, const Type &n) {
		Printer(os).dispatch(n);
		return os;
	}

	U8String str(const Type &n) {
		std::ostringstream oss;
		Printer(oss).dispatch(n);
		return oss.str();
	}
}