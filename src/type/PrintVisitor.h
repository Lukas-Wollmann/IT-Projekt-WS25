#pragma once
#include "Visitor.h"

namespace type {
	///
	/// Turn a type into a string representation for debug purposes.
	///
	struct PrintVisitor : public ConstVisitor<void> {
	private:
		std::ostream &m_OStream;

	public:
		PrintVisitor(std::ostream &os);

		void visit(const Typename &n) override;
		void visit(const PointerType &n) override;
		void visit(const ArrayType &n) override;
		void visit(const FunctionType &n) override;
		void visit(const ErrorType &n) override;
		void visit(const UnitType &n) override;
	};
}

std::ostream &operator<<(std::ostream &os, const type::TypeList &p);
std::ostream &operator<<(std::ostream &os, const type::Type &n);