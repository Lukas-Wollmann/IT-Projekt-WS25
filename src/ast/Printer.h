#pragma once

#include <format>

#include "Visitor.h"

namespace ast {
///
/// Turn an abstract syntax tree node into a tree like representation for debug purposes.
///
struct Printer : ConstVisitor<void> {
	using Iterator = std::format_context::iterator;

private:
	Iterator m_Out;
	U8String m_Prefix;
	bool m_IsLast;

public:
	explicit Printer(Iterator out);

	Iterator printNode(const Node &n);

private:
	void printLine(const U8String &text);
	[[nodiscard]] Printer child(bool isLast = false) const;
	void printLabeledChild(const U8String &label, const Node &node, bool isLast = false) const;

	void visit(const IntLit &n) override;
	void visit(const CharLit &n) override;
	void visit(const BoolLit &n) override;
	void visit(const UnitLit &n) override;
	void visit(const HeapAlloc &n) override;
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
	void visit(const StructDecl &n) override;
	void visit(const Module &n) override;
};
}

template <typename T>
	requires std::derived_from<T, ast::Node>
struct std::formatter<T> {
	constexpr auto parse(const format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const T &n, format_context &ctx) const {
		return ast::Printer(ctx.out()).printNode(n);
	}
};