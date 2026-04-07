#include "StmtLowerer.h"

namespace mir {
StmtLowerer::StmtLowerer(LoweringContext &ctx)
	: m_Context(ctx) {}

void StmtLowerer::lowerStmt(const ast::Stmt &n) {
	dispatch(n);
	m_Context.emitExprCleanup();
}

void StmtLowerer::visit(const ast::VarDef &n) {
	const auto result = ExprLowerer(m_Context).lowerExpr(*n.value);
	const auto left = m_Context.nextRegID();
	const auto right = result.reg;
	const auto &type = n.type;

	m_Context.bind(n.ident, left);
	m_Context.emit(std::make_unique<Store>(left, right));

	if (type->isTypeKind(type::TypeKind::Pointer)) {
		m_Context.addToScopeCleanup(left, type);

		if (result.isTemp) {
			m_Context.removeFromExprCleanup(right);
		} else {
			m_Context.emit(std::make_unique<Copy>(left, type));
		}
	}

	m_Context.emitExprCleanup();
}

void StmtLowerer::visit(const ast::IfStmt &n) {
	const auto cond = ExprLowerer(m_Context).lowerExpr(*n.cond);
	m_Context.emitExprCleanup();

	const auto then = m_Context.createBlock();
	const auto else_ = m_Context.createBlock();
	const auto merge = m_Context.createBlock();

	m_Context.emitTerm(std::make_unique<Branch>(cond.reg, then->id, else_->id));

	m_Context.setCurrentBlock(then);
	lowerStmt(*n.then);

	if (!m_Context.getCurrentBlock()->isTerminated()) {
		m_Context.emitTerm(std::make_unique<Jump>(merge->id));
	}

	m_Context.setCurrentBlock(else_);
	lowerStmt(*n.else_);

	if (!m_Context.getCurrentBlock()->isTerminated()) {
		m_Context.emitTerm(std::make_unique<Jump>(merge->id));
	}

	m_Context.setCurrentBlock(merge);
}

void StmtLowerer::visit(const ast::ReturnStmt &n) {}

void StmtLowerer::visit(const ast::BlockStmt &n) {
	m_Context.openScope();

	for (const auto &stmt : n.stmts) {
		lowerStmt(*stmt);

		if (m_Context.getCurrentBlock()->isTerminated()) {
			break;
		}
	}

	m_Context.closeScope();
}
}