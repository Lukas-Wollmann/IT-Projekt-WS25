#include "Printer.h"

#include <sstream>

namespace type {
	Printer::Printer(Iterator out)
		: m_Out(out) {}

	Printer::Iterator Printer::printType(const Type &t) {
		dispatch(t);
		return m_Out;
	}

	void Printer::print(const U8String &text) {
		m_Out = std::format_to(m_Out, "{}\n", text);
	}

	void Printer::visit(const Typename &t) {
		std::format_to(m_Out, "{}", t.typename_);
	}

	void Printer::visit(const PointerType &t) {
		std::format_to(m_Out, "*{}", *t.pointeeType);
	}

	void Printer::visit(const ArrayType &t) {
		std::format_to(m_Out, "[]{}", *t.elementType);
	}

	void Printer::visit(const FunctionType &t) {
		std::format_to(m_Out, "({})->({})", t.paramTypes, *t.returnType);
	}

	void Printer::visit(const ErrorType &) {
		std::format_to(m_Out, "<error-type>");
	}

	void Printer::visit(const UnitType &) {
		std::format_to(m_Out, "()");
	}
}