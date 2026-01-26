#pragma once

#include "AST.h"
#include "Visitor.h"

namespace ast {
	struct ASTPrinter : public ConstVisitor<void> {
	private:
		std::ostream &m_OStream;
		U8String m_Prefix;
		bool m_IsLast;

	public:
		ASTPrinter(std::ostream &os);

		void print(const Node &node);

	private:
		void printLine(const U8String &text);
		ASTPrinter child(bool isLast = false) const;
		void printLabeledChild(const U8String &label, const Node &node, bool isLast = false) const;

		void visit(const IntLit &n) override;
		void visit(const CharLit &n) override;
		void visit(const BoolLit &n) override;
		void visit(const UnitLit &n) override;
		void visit(const UnaryExpr &n) override;
		void visit(const BinaryExpr &n) override;
		void visit(const Assignment &n) override;
		void visit(const VarRef &n) override;
		void visit(const FuncCall &n) override;
		void visit(const BlockStmt &n) override;
		void visit(const IfStmt &n) override;
		void visit(const WhileStmt &n) override;
		void visit(const ReturnStmt &n) override;
		void visit(const VarDef &n) override;
		void visit(const FuncDecl &n) override;
		void visit(const Module &n) override;
	};

	std::ostream &operator<<(std::ostream &os, const Node &n);
}
