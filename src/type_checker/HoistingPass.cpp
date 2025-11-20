#include "HoistingPass.h"

void HoistingPass::visit(CodeBlock &node)
{
    m_SymbolTable.enterScope();

    for (StmtPtr &n : node.getStmts())
        n->accept(*this);

    m_SymbolTable.exitScope();
}

void HoistingPass::visit(FuncDecl &node) 
{
    TypeList paramTypes;

    for (const Param &param : node.getParams())
        paramTypes.push_back(param.second->copy());

    auto funcType = std::make_unique<FunctionType>(
        std::move(paramTypes), node.getReturnType().copy()
    );

    m_SymbolTable.addSymbol(node.getIdent(), SymbolInfo(std::move(funcType)));
}