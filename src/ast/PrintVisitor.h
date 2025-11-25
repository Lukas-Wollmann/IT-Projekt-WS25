#pragma once
#include "Visitor.h"

///
/// Turn a node into a string representation for debug purposes.
///
struct PrintVisitor : public ConstVisitor<void> {
private:
	std::ostream &m_OStream;

public:
	PrintVisitor(std::ostream &os);

	void visit(const ast::IntLit &n) override;
	void visit(const ast::FloatLit &n) override;
	void visit(const ast::CharLit &n) override;
	void visit(const ast::BoolLit &n) override;
	void visit(const ast::StringLit &n) override;
	void visit(const ast::ArrayExpr &n) override;
	void visit(const ast::UnaryExpr &n) override;
	void visit(const ast::BinaryExpr &n) override;
	void visit(const ast::FuncCall &n) override;
	void visit(const ast::VarRef &n) override;
	void visit(const ast::BlockStmt &n) override;
	void visit(const ast::IfStmt &n) override;
	void visit(const ast::WhileStmt &n) override;
	void visit(const ast::ReturnStmt &n) override;
	void visit(const ast::VarDef &n) override;
	void visit(const ast::FuncDecl &n) override;
	void visit(const ast::Module &n) override;
};

std::ostream &operator<<(std::ostream &os, const ast::Node &n);