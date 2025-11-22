#include "ExplorationPass.h"
#include <sstream>

ExplorationPass::ExplorationPass(TypeCheckerContext &context)
    : m_Context(context)
{}

void ExplorationPass::visit(FuncDecl &node) 
{
    TypeList paramTypes;

    for (const Param &param : node.getParams())
        paramTypes.push_back(param.second->copy());

    auto funcType = std::make_unique<FunctionType>(
        std::move(paramTypes), node.getReturnType().copy()
    );

    Namespace &gloabl = m_Context.getGlobalNamespace();

    if (gloabl.getFunction(node.getIdent()))
    {
        std::stringstream ss;
        ss << "Illegal redeclaration of function '" << node.getIdent() << "'.";
        m_Context.addError(ss.str());
    }
    else
    {
        gloabl.addFunction(FunctionDeclaration(node.getIdent(), std::move(funcType)));
    }
}

void ExplorationPass::visit(Module &node)
{
    for (FuncDeclPtr &decl : node.getDeclarations())
        decl->accept(*this);
}