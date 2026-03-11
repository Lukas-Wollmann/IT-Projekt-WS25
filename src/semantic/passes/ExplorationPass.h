#pragma once
#include <unordered_set>

#include "ast/Visitor.h"
#include "semantic/common/TypeCheckerContext.h"

namespace sem {
///
/// Find all declaration that exist in a module. Only top level
/// declaration are supported as of right now. That means it is
/// not possible to nest a declaration inside another one. This
/// has to be ensured by using correct parsing rules.
///
struct ExplorationPass : ast::ConstVisitor<void> {
private:
	TypeCheckerContext &m_Context;
	std::unordered_set<StructType *> m_ValidatedStructs;
	StructType *m_CurrentRootBeingValidated = nullptr;

public:
	explicit ExplorationPass(TypeCheckerContext &ctx);

private:
	void visit(const ast::Module &n) override;
	void visit(const ast::StructDecl &n) override;
	void visit(const ast::FuncDecl &n) override;

	void validateNoCycles(StructType *root);
	bool checkRecursive(StructType *current, Vec<StructType *> &path);
};
}