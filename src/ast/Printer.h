#pragma once

#include <format>

#include "AST.h"
#include "Visitor.h"

namespace ast {
	struct ASTPrinter : public ConstVisitor<void> {
		using Iterator = std::format_context::iterator;

	private:
		Iterator m_Out;
		U8String m_Prefix;
		bool m_IsLast;

	public:
		ASTPrinter(Iterator out);

		Iterator printNode(const Node &n);

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

template <>
struct std::formatter<ast::Node> {
	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const ast::Node &n, format_context &ctx) const {
		return ast::ASTPrinter(ctx.out()).printNode(n);
	}
};
