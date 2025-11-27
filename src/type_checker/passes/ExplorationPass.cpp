#include "ExplorationPass.h"

#include <sstream>

ExplorationPass::ExplorationPass(TypeCheckerContext &context) : m_Context(context) {}

void ExplorationPass::visit(const ast::Module &n) {
	for (auto &d : n.decls)
		dispatch(*d);
}

void ExplorationPass::visit(const ast::FuncDecl &n) {
	TypeList params;

	for (auto &p : n.params)
		params.push_back(p.second->copy());

	auto funcType = std::make_unique<FunctionType>(std::move(params), n.returnType->copy());

	Namespace &gloabl = m_Context.getGlobalNamespace();

	if (gloabl.getFunction(n.ident)) {
		std::stringstream ss;
		ss << "Illegal redeclaration of function '" << n.ident << "'.";
		m_Context.addError(ss.str());
		return;
	}

	gloabl.addFunction(FunctionDeclaration(n.ident, std::move(funcType)));
}