#pragma once

#include "ExprCodeGen.h"

namespace codegen {
	struct StmtCodeGen : public ast::ConstVisitor<void> {
	private:
		CodeGenContext &m_Context;
		RValueCodeGen &m_RValueCodeGen;
		LValueCodeGen &m_LValueCodeGen;

	public:
		StmtCodeGen(CodeGenContext &ctx, RValueCodeGen &rValueCodeGen,
					LValueCodeGen &lValueCodeGen);

	private:
		void visit(const ast::Assignment &n) override;
		void visit(const ast::BlockStmt &n) override;
		void visit(const ast::IfStmt &n) override;
		void visit(const ast::WhileStmt &n) override;
		void visit(const ast::ReturnStmt &n) override;
		void visit(const ast::VarDef &n) override;
		void visit(const ast::FuncDecl &n) override;
		void visit(const ast::Module &n) override;
	};
}