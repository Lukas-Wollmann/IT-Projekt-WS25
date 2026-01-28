#include "Printer.h"

#include "type/Printer.h"

namespace ast {
	Printer::Printer(Iterator out)
		: m_Out(out)
		, m_Prefix(u8"")
		, m_IsLast(true) {}

	Printer::Iterator Printer::printNode(const Node &n) {
		dispatch(n);

		return m_Out;
	}

	void Printer::printLine(const U8String &text) {
		U8String connector = m_IsLast ? u8"└─ " : u8"├─ ";
		m_Out = std::format_to(m_Out, "{}{}{}\n", m_Prefix, connector, text);
	}

	Printer Printer::child(bool isLast) const {
		Printer p(m_Out);
		U8String prefix = m_IsLast ? u8"   " : u8"│  ";

		p.m_Prefix = m_Prefix + prefix;
		p.m_IsLast = isLast;

		return p;
	}

	void Printer::printLabeledChild(const U8String &label, const Node &node, bool isLast) const {
		auto p = child(isLast);
		p.printLine(label);
		p.child(true).printNode(node);
	}

	void Printer::visit(const IntLit &n) {
		printLine(std::format("IntLit({})", n.value));
	}

	void Printer::visit(const CharLit &n) {
		printLine(std::format("CharLit('{}')", U8String(n.value)));
	}

	void Printer::visit(const BoolLit &n) {
		printLine(std::format("BoolLit({})", n.value));
	}

	void Printer::visit(const UnitLit &n) {
		printLine(u8"UnitLit");
	}

	void Printer::visit(const VarRef &n) {
		printLine(std::format("VarRef(\"{}\")", n.ident));
	}

	void Printer::visit(const UnaryExpr &n) {
		printLine(std::format("UnaryExpr({})", n.op));
		child(true).printNode(*n.operand);
	}

	void Printer::visit(const BinaryExpr &n) {
		printLine(std::format("BinaryExpr({})", n.op));
		child(false).printNode(*n.left);
		child(true).printNode(*n.right);
	}

	void Printer::visit(const Assignment &n) {
		printLine(std::format("Assignment({})", n.assignmentKind));
		child(false).printNode(*n.left);
		child(true).printNode(*n.right);
	}

	void Printer::visit(const FuncCall &n) {
		printLine(u8"FuncCall");

		auto callee = child();
		callee.printLine(u8"Callee");
		callee.child(true).printNode(*n.expr);

		auto args = child(true);
		args.printLine(u8"Args");

		for (size_t i = 0; i < n.args.size(); ++i) {
			const auto isLast = i + 1 == n.args.size();
			args.child(isLast).printNode(*n.args[i]);
		}
	}

	void Printer::visit(const BlockStmt &n) {
		printLine(u8"BlockStmt");

		for (size_t i = 0; i < n.stmts.size(); ++i) {
			const auto isLast = i + 1 == n.stmts.size();
			child(isLast).printNode(*n.stmts[i]);
		}
	}

	void Printer::visit(const IfStmt &n) {
		printLine(u8"IfStmt");
		printLabeledChild(u8"Cond", *n.cond);
		printLabeledChild(u8"Then", *n.then);
		printLabeledChild(u8"Else", *n.else_, true);
	}

	void Printer::visit(const WhileStmt &n) {
		printLine(u8"WhileStmt");
		printLabeledChild(u8"Cond", *n.cond);
		printLabeledChild(u8"Body", *n.body, true);
	}

	void Printer::visit(const ReturnStmt &n) {
		printLine(u8"ReturnStmt");
		child(true).printNode(*n.expr);
	}

	void Printer::visit(const VarDef &n) {
		printLine(u8"VarDef(\"" + n.ident + u8"\")");
		printLabeledChild(u8"Value", *n.value, true);
	}

	void Printer::visit(const FuncDecl &n) {
		printLine(u8"FuncDecl(\"" + n.ident + u8"\")");

		auto params = child();
		params.printLine(u8"Params");

		for (size_t i = 0; i < n.params.size(); ++i) {
			const auto &[name, type] = n.params[i];
			const auto isLast = i + 1 == n.params.size();
			params.child(isLast).printLine(std::format("{}: {}", name, *type));
		}

		auto ret = child();
		ret.printLine(std::format("ReturnType: {}", *n.returnType));

		auto body = child(true);
		body.printLine(u8"Body");
		body.child(true).printNode(*n.body);
	}

	void Printer::visit(const Module &n) {
		printLine(u8"Module(\"" + n.name + u8"\")");

		for (size_t i = 0; i < n.decls.size(); ++i) {
			const auto isLast = i + 1 == n.decls.size();
			child(isLast).printNode(*n.decls[i]);
		}
	}
}