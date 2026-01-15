#include "ExplorationPass.h"

#include <ranges>

#include "semantic/common/ErrorMessages.h"
#include "semantic/common/OperatorTable.h"

namespace semantic {
	using namespace type;

	ExplorationPass::ExplorationPass(TypeCheckerContext &context)
		: m_Context(context) {}

	void ExplorationPass::visit(const ast::Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);
	}

	void ExplorationPass::visit(const ast::FuncDecl &n) {
		TypeList params;

		std::ranges::copy(n.params | std::views::values, std::back_inserter(params));

		const auto funcType = std::make_shared<FunctionType>(std::move(params), n.returnType);
		auto &global = m_Context.getGlobalNamespace();

		if (global.getFunction(n.ident)) {
			m_Context.addError(ErrorMessage<ErrorMessageKind::SymbolRedefinition>::str(n.ident));
			return;
		}

		global.addFunction(n.ident, funcType);
	}
}