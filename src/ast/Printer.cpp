#include "Printer.h"

#include "type/Printer.h"

namespace ast {
	Printer::Printer(std::ostream &os)
		: m_OStream(os) {}

	void Printer::visit(const IntLit &n) {
		m_OStream << "IntLit(" << n.value << ")";
	}

	void Printer::visit(const FloatLit &n) {
		m_OStream << "FloatLit(" << n.value << ")";
	}

	void Printer::visit(const CharLit &n) {
		m_OStream << "CharLit('" << U8String(n.value) << "')";
	}

	void Printer::visit(const BoolLit &n) {
		m_OStream << "BoolLit(" << (n.value ? "true" : "false") << ")";
	}

	void Printer::visit(const StringLit &n) {
		m_OStream << "StringLit(\"" << n.value << "\")";
	}

	void Printer::visit(const UnitLit &) {
		m_OStream << "UnitLit()";
	}

	void Printer::visit(const ArrayExpr &n) {
		m_OStream << "ArrayExpr(" << *n.elementType << ", {";

		for (size_t i = 0; i < n.values.size(); ++i)
			m_OStream << (i ? ", " : "") << *n.values[i];

		m_OStream << "})";
	}

	void Printer::visit(const UnaryExpr &n) {
		m_OStream << "UnaryExpr(" << n.op << ", ";
		dispatch(*n.operand);
		m_OStream << ")";
	}

	void Printer::visit(const BinaryExpr &n) {
		m_OStream << "BinaryExpr(";
		dispatch(*n.left);
		m_OStream << ", " << n.op << ", ";
		dispatch(*n.right);
		m_OStream << ")";
	}

	void Printer::visit(const Assignment &n) {
		m_OStream << "Assignment(";
		dispatch(*n.left);
		m_OStream << ", ";
		m_OStream << n.assignmentKind << ", ";
		dispatch(*n.right);
		m_OStream << ")";
	}

	void Printer::visit(const HeapAlloc &n) {
		m_OStream << "HeapAlloc(" << *n.type << ")";
	}

	void Printer::visit(const FuncCall &n) {
		m_OStream << "FuncCall(";
		dispatch(*n.expr);
		m_OStream << ", {";

		for (size_t i = 0; i < n.args.size(); ++i)
			m_OStream << (i ? ", " : "") << *n.args[i];

		m_OStream << "})";
	}

	void Printer::visit(const VarRef &n) {
		m_OStream << "VarRef(" << n.ident << ")";
	}

	void Printer::visit(const BlockStmt &n) {
		m_OStream << "BlockStmt({";

		for (size_t i = 0; i < n.stmts.size(); ++i)
			m_OStream << (i ? ", " : "") << *n.stmts[i];

		m_OStream << "})";
	}

	void Printer::visit(const IfStmt &n) {
		m_OStream << "IfStmt(";
		dispatch(*n.cond);
		m_OStream << ", ";
		dispatch(*n.then);
		m_OStream << ", ";
		dispatch(*n.else_);
		m_OStream << ")";
	}

	void Printer::visit(const WhileStmt &n) {
		m_OStream << "WhileStmt(";
		dispatch(*n.cond);
		m_OStream << ", ";
		dispatch(*n.body);
		m_OStream << ")";
	}

	void Printer::visit(const ReturnStmt &n) {
		m_OStream << "ReturnStmt(";

		if (n.expr)
			dispatch(**n.expr);

		m_OStream << ")";
	}

	void Printer::visit(const VarDef &n) {
		m_OStream << "VarDef(" << n.ident << ", ";
		m_OStream << *n.type << ", ";
		dispatch(*n.value);
		m_OStream << ")";
	}

	void Printer::visit(const FuncDecl &n) {
		m_OStream << "FuncDecl(" << n.ident << ", {";

		for (size_t i = 0; i < n.params.size(); ++i)
			m_OStream << (i ? ", " : "") << n.params[i].first << ": " << *n.params[i].second;

		m_OStream << "}, " << *n.returnType;
		m_OStream << ", ";
		dispatch(*n.body);
		m_OStream << ")";
	}

	void Printer::visit(const Module &n) {
		m_OStream << "Module(" << n.name << ", {";

		for (size_t i = 0; i < n.decls.size(); ++i)
			m_OStream << (i ? ", " : "") << *n.decls[i];

		m_OStream << "})";
	}
}

std::ostream &operator<<(std::ostream &os, const ast::Node &n) {
	ast::Printer(os).dispatch(n);
	return os;
}