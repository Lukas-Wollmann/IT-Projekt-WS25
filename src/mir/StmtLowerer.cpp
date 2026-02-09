#include "StmtLowerer.h"

namespace mir {
StmtLowerer::StmtLowerer(Function &func, LoweringContext &ctx)
	: m_CurrentFunc(func)
	, m_Context(ctx) {}

void StmtLowerer::visit(const ast::VarDef &n) {
	const auto result = ExprLowerer(m_CurrentFunc, m_Context).lowerExpr(*n.value);
	const auto left = m_CurrentFunc.nextRegID();
	const auto right = result.reg;
	const auto &type = n.type;

	m_CurrentFunc.bind(n.ident, left);

	m_Context.emit(std::make_unique<Store>(left, right));

	if (type->isTypeKind(type::TypeKind::Pointer)) {
		m_Context.m_TrackedValues.back().push_back({left, type});

		if (result.isTemp) {
			m_Context.removeFromCleanup(right);
		} else {
			m_Context.emit(std::make_unique<SPRetain>(left, type));
		}
	}
}

void StmtLowerer::visit(const ast::IfStmt &n) {}

void StmtLowerer::visit(const ast::ReturnStmt &n) {}

void StmtLowerer::visit(const ast::BlockStmt &n) {}
}