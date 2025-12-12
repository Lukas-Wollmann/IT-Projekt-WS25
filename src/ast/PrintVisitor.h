#pragma once
#include "Visitor.h"
#include "type/PrintVisitor.h"

namespace ast {
	///
	/// Turn a node into a string representation for debug purposes.
	///
	struct PrintVisitor : public ConstVisitor<void> {
	private:
		std::ostream &m_OStream;

	public:
		PrintVisitor(std::ostream &os);

		void visit(const IntLit &n) override;
		void visit(const FloatLit &n) override;
		void visit(const CharLit &n) override;
		void visit(const BoolLit &n) override;
		void visit(const StringLit &n) override;
		void visit(const ArrayExpr &n) override;
		void visit(const UnaryExpr &n) override;
		void visit(const BinaryExpr &n) override;
		void visit(const Assignment &n) override;
		void visit(const FuncCall &n) override;
		void visit(const VarRef &n) override;
		void visit(const BlockStmt &n) override;
		void visit(const IfStmt &n) override;
		void visit(const WhileStmt &n) override;
		void visit(const ReturnStmt &n) override;
		void visit(const VarDef &n) override;
		void visit(const FuncDecl &n) override;
		void visit(const Module &n) override;
	};
}

std::ostream &operator<<(std::ostream &os, const ast::Node &n);