#include "PrintVisitor.h"

PrintVisitor::PrintVisitor(std::ostream &os)
	: m_OStream(os) {}

std::ostream &operator<<(std::ostream &os, const ast::Node &n) {
	PrintVisitor(os).dispatch(n);
	return os;
}

void PrintVisitor::visit(const ast::IntLit &n) {}

void PrintVisitor::visit(const ast::FloatLit &n) {}

void PrintVisitor::visit(const ast::CharLit &n) {}

void PrintVisitor::visit(const ast::BoolLit &n) {}

void PrintVisitor::visit(const ast::StringLit &n) {}

void PrintVisitor::visit(const ast::ArrayExpr &n) {}

void PrintVisitor::visit(const ast::UnaryExpr &n) {}

void PrintVisitor::visit(const ast::BinaryExpr &n) {}

void PrintVisitor::visit(const ast::FuncCall &n) {}

void PrintVisitor::visit(const ast::VarRef &n) {}

void PrintVisitor::visit(const ast::BlockStmt &n) {}

void PrintVisitor::visit(const ast::IfStmt &n) {}

void PrintVisitor::visit(const ast::WhileStmt &n) {}

void PrintVisitor::visit(const ast::ReturnStmt &n) {}

void PrintVisitor::visit(const ast::VarDef &n) {}

void PrintVisitor::visit(const ast::FuncDecl &n) {}

void PrintVisitor::visit(const ast::Module &n) {}