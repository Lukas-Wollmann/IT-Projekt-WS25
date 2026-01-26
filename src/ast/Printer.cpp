#include "Printer.h"

#include "type/Printer.h"

namespace ast {
	ASTPrinter::ASTPrinter(std::ostream &os)
		: m_OStream(os)
		, m_Prefix(u8"")
		, m_IsLast(true) {}

	void ASTPrinter::print(const Node &n) {
		dispatch(n);
	}

	void ASTPrinter::printLine(const U8String &text) {
		m_OStream << m_Prefix << (m_IsLast ? "└─ " : "├─ ") << text << "\n";
	}

	ASTPrinter ASTPrinter::child(bool isLast) const {
		ASTPrinter p(m_OStream);
		p.m_Prefix = m_Prefix + (m_IsLast ? u8"   " : u8"│  ");
		p.m_IsLast = isLast;

		return p;
	}

	void ASTPrinter::printLabeledChild(const U8String &label, const Node &node, bool isLast) const {
		auto p = child(isLast);
		p.printLine(label);
		p.child(true).print(node);
	}

	void ASTPrinter::visit(const IntLit &n) {
		printLine("IntLit(" + std::to_string(n.value) + ")");
	}

	void ASTPrinter::visit(const CharLit &n) {
		printLine(u8"CharLit('" + U8String(n.value) + u8"')");
	}

	void ASTPrinter::visit(const BoolLit &n) {
		printLine(U8String("BoolLit(") + (n.value ? u8"true" : u8"false") + u8")");
	}

	void ASTPrinter::visit(const UnitLit &n) {
		printLine(u8"UnitLit()");
	}

	void ASTPrinter::visit(const VarRef &n) {
		printLine(u8"VarRef(\"" + n.ident + u8"\")");
	}

	void ASTPrinter::visit(const UnaryExpr &n) {
		printLine(u8"UnaryExpr(" + str(n.op) + u8")");
		child(true).print(*n.operand);
	}

	void ASTPrinter::visit(const BinaryExpr &n) {
		printLine(u8"BinaryExpr(" + str(n.op) + u8")");
		child(false).print(*n.left);
		child(true).print(*n.right);
	}

	void ASTPrinter::visit(const Assignment &n) {
		printLine(u8"Assignment(" + str(n.assignmentKind) + u8")");
		child(false).print(*n.left);
		child(true).print(*n.right);
	}

	void ASTPrinter::visit(const FuncCall &n) {
		printLine(u8"FuncCall");

		auto callee = child();
		callee.printLine(u8"Callee");
		callee.child(true).print(*n.expr);

		auto args = child(true);
		args.printLine(u8"Args");

		for (size_t i = 0; i < n.args.size(); ++i)
			args.child(i + 1 == n.args.size()).print(*n.args[i]);
	}

	void ASTPrinter::visit(const BlockStmt &n) {
		printLine(u8"BlockStmt");

		for (size_t i = 0; i < n.stmts.size(); ++i)
			child(i + 1 == n.stmts.size()).print(*n.stmts[i]);
	}

	void ASTPrinter::visit(const IfStmt &n) {
		printLine(u8"IfStmt");
		printLabeledChild(u8"Cond", *n.cond);
		printLabeledChild(u8"Then", *n.then);
		printLabeledChild(u8"Else", *n.else_, true);
	}

	void ASTPrinter::visit(const WhileStmt &n) {
		printLine(u8"WhileStmt");
		printLabeledChild(u8"Cond", *n.cond);
		printLabeledChild(u8"Body", *n.body, true);
	}

	void ASTPrinter::visit(const ReturnStmt &n) {
		printLine(u8"ReturnStmt");
		child(true).print(*n.expr);
	}

	void ASTPrinter::visit(const VarDef &n) {
		printLine(u8"VarDef(\"" + n.ident + u8"\")");
		printLabeledChild(u8"Value", *n.value, true);
	}

	void ASTPrinter::visit(const FuncDecl &n) {
		printLine(u8"FuncDecl(\"" + n.ident + u8"\")");

		auto params = child();
		params.printLine(u8"Params");

		for (size_t i = 0; i < n.params.size(); ++i) {
			const auto &[name, type] = n.params[i];
			params.child(i + 1 == n.params.size()).printLine(name + u8": " + type::str(*type));
		}

		auto ret = child();
		ret.printLine(u8"ReturnType: " + type::str(*n.returnType));

		auto body = child(true);
		body.printLine(u8"Body");
		body.child(true).print(*n.body);
	}

	void ASTPrinter::visit(const Module &n) {
		printLine(u8"Module(\"" + n.name + u8"\")");

		for (size_t i = 0; i < n.decls.size(); ++i)
			child(i + 1 == n.decls.size()).print(*n.decls[i]);
	}

	std::ostream &operator<<(std::ostream &os, const Node &n) {
		ASTPrinter(os).print(n);
		return os;
	}
}