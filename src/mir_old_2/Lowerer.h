#pragma once
#include "StmtLowerer.h"

namespace mir {
struct Lowerer {
	Module lowerModule(const ast::Module &n) {
		Module module(n.name);

		for (const auto &d : n.decls) {
			module.funcs.push_back(lowerFuncDecl(*d));
		}

		return module;
	}

	Ptr<Function> lowerFuncDecl(const ast::FuncDecl &n) {
		const auto func = std::make_shared<Function>(n.ident);
		LoweringContext ctx(func, func->createBlock());

		ctx.openScope();

		for (const auto &param : n.params) {
			const auto regID = func->nextRegID();
			ctx.bind(param.first, regID);
			func->params.push_back(regID);

			// If param is a pointer, track it for cleanup
			if (param.second->isTypeKind(type::TypeKind::Pointer)) {
				ctx.addToScopeCleanup(regID, param.second);
			}
		}

		StmtLowerer(ctx).lowerStmt(*n.body);

		if (!ctx.getCurrentBlock()->isTerminated()) {
			ctx.emitScopeCleanup();

			const auto dest = ctx.nextRegID();

			ctx.emit(std::make_unique<UnitLit>(dest));
			ctx.emitTerm(std::make_unique<Return>(dest));
		}

		// Do not emit cleanups here, because we clean the params at the caller side
		ctx.closeScope(false);

		return func;
	}
};
}
