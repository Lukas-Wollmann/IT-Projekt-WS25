#pragma once

#include "ExprCodeGen.h"

namespace codegen {
	struct CodeGen : public ast::ConstVisitor<void> {
	private:
		CodeGenContext &m_Context;
        RValueCodeGen m_RValueCodeGen;
        LValueCodeGen m_LValueCodeGen;

	public:
        static void generate(std::ostream &os, const ast::Module &module);

	private:
		CodeGen(CodeGenContext &ctx);
        
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