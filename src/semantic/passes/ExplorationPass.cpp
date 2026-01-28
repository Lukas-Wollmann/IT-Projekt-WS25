#include "ExplorationPass.h"

#include <ranges>

#include "semantic/common/ErrorMessages.h"
#include "semantic/common/OperatorTable.h"

namespace semantic {
	using namespace type;
	using namespace ast;
	using enum ErrorMessageKind;

	ExplorationPass::ExplorationPass(TypeCheckerContext &ctx)
		: m_Context(ctx) {}

	void ExplorationPass::visit(const Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);
	}

	void ExplorationPass::visit(const FuncDecl &n) {
		TypeList params;

		std::ranges::copy(n.params | std::views::values, std::back_inserter(params));

		const auto funcType = std::make_shared<FunctionType>(std::move(params), n.returnType);
		auto &global = m_Context.getGlobalNamespace();

		if (global.getFunction(n.ident)) {
			const auto msg = ErrorMessage<SymbolRedefinition>::str(n.ident);
			m_Context.submitError(msg, {});

			return;
		}

		global.addFunction(n.ident, funcType);
	}
}