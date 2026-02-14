#pragma once
#include "ExprLowerer.h"
#include "ast/Visitor.h"

namespace mir {
struct StmtLowerer : ast::ConstVisitor<void> {
private:
	LoweringContext &m_Context;

public:
	StmtLowerer(LoweringContext &ctx);

    void lowerStmt(const ast::Stmt &n);

private:
	void visit(const ast::VarDef &n) override;
	void visit(const ast::IfStmt &n) override;
	void visit(const ast::ReturnStmt &n) override;
	void visit(const ast::BlockStmt &n) override;
};
}
