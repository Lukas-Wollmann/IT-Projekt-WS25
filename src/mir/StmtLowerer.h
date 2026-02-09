#pragma once
#include "ExprLowerer.h"
#include "ast/Visitor.h"

namespace mir {
struct StmtLowerer : ast::ConstVisitor<void> {
private:
	Function &m_CurrentFunc;
	LoweringContext &m_Context;

public:
	StmtLowerer(Function &func, LoweringContext &ctx);

private:
	void visit(const ast::VarDef &n) override;
	void visit(const ast::IfStmt &n) override;
	void visit(const ast::ReturnStmt &n) override;
	void visit(const ast::BlockStmt &n) override;
};
}
