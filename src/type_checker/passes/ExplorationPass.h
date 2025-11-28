#pragma once
#include "ast/Visitor.h"
#include "type_checker/common/TypeCheckerContext.h"

///
/// Find all declaration that exist in a module. Only top level
/// declaration are supported as of right now. That means it is
/// not possible to nest a declaration inside another one. This
/// has to be ensured by using correct parsing rules.
///

struct ExplorationPass : public ast::ConstVisitor<void> {
private:
	TypeCheckerContext &m_Context;

public:
	ExplorationPass(TypeCheckerContext &context);

	
private:
	void visit(const ast::Module &n) override;
	void visit(const ast::FuncDecl &n) override;
};