#include "ExplorationPass.h"

#include <sstream>

#include "type/CloneVisitor.h"

using namespace type;

ExplorationPass::ExplorationPass(TypeCheckerContext &context)
	: m_Context(context) {}

void ExplorationPass::visit(const ast::Module &n) {
	for (auto &d : n.decls)
		dispatch(*d);
}

void ExplorationPass::visit(const ast::FuncDecl &n) {
	Vec<Box<const Type>> params;

	for (auto &p : n.params)
		params.push_back(clone(*p.second));

	auto funcType = std::make_unique<FunctionType>(std::move(params), clone(*n.returnType));

	Namespace &gloabl = m_Context.getGlobalNamespace();

	if (gloabl.getFunction(n.ident)) {
		std::stringstream ss;
		ss << "Illegal redeclaration of function '" << n.ident << "'.";
		m_Context.addError(ss.str());
		return;
	}

	gloabl.addFunction(n.ident, std::move(funcType));
}