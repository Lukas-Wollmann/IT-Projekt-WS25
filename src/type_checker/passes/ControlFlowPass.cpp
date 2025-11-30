#include "ControlFlowPass.h"

ControlFlowPass::ControlFlowPass(TypeCheckerContext &context)
	: m_Context(context) {}

bool ControlFlowPass::visit(const ast::IntLit &) {
	return false;
}

bool ControlFlowPass::visit(const ast::FloatLit &) {
	return false;
}

bool ControlFlowPass::visit(const ast::CharLit &) {
	return false;
}

bool ControlFlowPass::visit(const ast::BoolLit &) {
	return false;
}

bool ControlFlowPass::visit(const ast::StringLit &) {
	return false;
}

bool ControlFlowPass::visit(const ast::ArrayExpr &) {
	return false;
}

bool ControlFlowPass::visit(const ast::UnaryExpr &) {
	return false;
}

bool ControlFlowPass::visit(const ast::BinaryExpr &) {
	return false;
}

bool ControlFlowPass::visit(const ast::FuncCall &) {
	return false;
}

bool ControlFlowPass::visit(const ast::VarRef &) {
	return false;
}

bool ControlFlowPass::visit(const ast::VarDef &) {
	return false;
}

bool ControlFlowPass::visit(const ast::BlockStmt &n) {
	for (size_t i = 0; i < n.stmts.size(); ++i) {
		auto &s = n.stmts[i];

		if (!dispatch(*s))
			continue;

		if (i < n.stmts.size() - 1) {
			std::stringstream ss;
			ss << "Unreachable statements detected after a return statement.";
			m_Context.addError(ss.str());
		}

		return true;
	}

	return false;
}

bool ControlFlowPass::visit(const ast::IfStmt &n) {
	return dispatch(*n.then) && dispatch(*n.else_);
}

bool ControlFlowPass::visit(const ast::WhileStmt &) {
	return false;
}

bool ControlFlowPass::visit(const ast::ReturnStmt &) {
	return true;
}

bool ControlFlowPass::visit(const ast::FuncDecl &n) {
	bool doesReturn = dispatch(*n.body);

	if (!doesReturn) {
		std::stringstream ss;
		ss << "Not all control flow paths in function '" << n.ident << "' return a value.";
		m_Context.addError(ss.str());
	}

	return doesReturn;
}

bool ControlFlowPass::visit(const ast::Module &n) {
	for (const auto &decl : n.decls)
		dispatch(*decl);

	return false;
}
