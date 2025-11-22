#pragma once
#include "ast/Visitor.h"
#include "ast/AST.h"
#include "type_checker/common/TypeCheckerContext.h"

struct ExplorationPass : public Visitor
{
private:
    TypeCheckerContext &m_Context;

public:
    ExplorationPass(TypeCheckerContext &context);
    
    void visit(FuncDecl &node) override;
    void visit(Module &node) override;
};