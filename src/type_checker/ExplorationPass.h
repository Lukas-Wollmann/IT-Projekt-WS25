#pragma once
#include "ast/Visitor.h"
#include "ast/AST.h"
#include "type_checker/SymbolTable.h"

struct ExplorationPass : public Visitor
{
private:
    TypeCheckingContext &m_Context;

public:
    ExplorationPass(TypeCheckingContext &context);
    
    void visit(CodeBlock &node) override;
    void visit(FuncDecl &node) override;
};