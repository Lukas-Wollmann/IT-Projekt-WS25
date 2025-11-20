#include "ExplorationPass.h"
#include <sstream>

ExplorationPass::ExplorationPass(TypeCheckingContext &context)
    : m_Context(context)
{}

void ExplorationPass::visit(CodeBlock &node)
{
    m_Context.getSymbolTable().createScope();

    for (StmtPtr &n : node.getStmts())
        n->accept(*this);

    m_Context.getSymbolTable().exitScope();
}

void ExplorationPass::visit(FuncDecl &node) 
{
    TypeList paramTypes;

    for (const Param &param : node.getParams())
        paramTypes.push_back(param.second->copy());

    auto funcType = std::make_unique<FunctionType>(
        std::move(paramTypes), node.getReturnType().copy()
    );

    if (m_Context.getSymbolTable().isSymbolDefinedInCurrentScope(node.getIdent()))
    {
        std::stringstream ss;
        ss << "Illegal redeclaration of function '" << node.getIdent() << "'.";
        m_Context.addError(TypeError(ss.str()));
    }
    else
    {
        m_Context.getSymbolTable().addSymbol(node.getIdent(), SymbolInfo(std::move(funcType)));
    }

    m_Context.getSymbolTable().createScope();

    node.getBody().accept(*this);

    m_Context.getSymbolTable().exitScope();
}