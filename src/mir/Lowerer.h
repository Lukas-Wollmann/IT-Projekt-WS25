#pragma once
#include "MIR.h"
#include "ast/AST.h"

namespace mir {
struct TrackedVar {
	RegisterID id;
	type::TypePtr type;
};

struct ExprResult {
	RegisterID reg;
	// isManaged = true means this register holds a value with an "outstanding"
	// ref count (e.g. fresh allocation or return from function) that must
	// be adopted or released.
	bool isManaged;
};

class Lowerer {
	Module m_Module;

	Opt<Ref<Function>> m_CurrentFunc;
	Opt<Ref<BasicBlock>> m_CurrentBlock;

	Vec<Map<U8String, RegisterID>> m_NamedValues;

	// Stack of scopes, each containing tracked variables for RAII
	Vec<Vec<TrackedVar>> m_TrackedValues;

	// Temporaries in the current statement that need cleanup if not consumed
	Vec<TrackedVar> m_ExprTemps;

public:
	explicit Lowerer(U8String moduleName)
		: m_Module(std::move(moduleName)) {}

	Module lowerModule(const ast::Module &n);

private:
	void openScope();
	void closeScope();
	void bind(const U8String &name, RegisterID id);
	Opt<RegisterID> lookup(const U8String &name);

	// Helpers
	RegisterID newRegister();
	void emit(Box<Instr> instr);
	void emitTerm(Box<Term> term);

	// AST Visitors
	void lowerFuncDecl(const ast::FuncDecl &n);
	void lowerBlockStmt(const ast::BlockStmt &n);
	void lowerStmt(const ast::Stmt &n);
	void lowerIfStmt(const ast::IfStmt &n);
	void lowerReturnStmt(const ast::ReturnStmt &n);
	ExprResult lowerExpr(const ast::Expr &n);

	// RAII / Cleanup
	void generateScopeCleanup();		   // Clean current scope
	void cleanupScopesUntil(size_t depth); // Unwind stack to specific depth
	void cleanupExprTemps();			   // Clean up temporaries
};
}