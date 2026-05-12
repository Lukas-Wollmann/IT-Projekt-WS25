#pragma once
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>

#include "ExprCodeGen.h"
#include "core/DefaultDecls.h"

namespace gen {
struct CodeGen : ast::ConstVisitor<void> {
private:
	CodeGenContext &m_Context;
	AllocManager m_AllocManager;
	Opt<Type> m_CurrentFunctionReturnType;

public:
	explicit CodeGen(CodeGenContext &ctx);

	static void generate(std::ofstream &out, const ast::Module &module);
	void visitNode(const ast::Node &n);

	void visit(const ast::Module &n) override;
	void visit(const ast::FuncDecl &n) override;
	void visit(const ast::BlockStmt &n) override;
	void visit(const ast::IfStmt &n) override;
	void visit(const ast::WhileStmt &n) override;
	void visit(const ast::ReturnStmt &n) override;
	void visit(const ast::VarDef &n) override;
};
}