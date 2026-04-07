#pragma once

#include "ExprCodeGen.h"

namespace llvm {
struct Value;
}

namespace gen {
struct CodeGenContext;

struct CodeGen : public ast::ConstVisitor<void> {
	CodeGenContext &m_Context;
	LValueCodeGen m_LValueCodeGen;
	llvm::Value *m_LastValue;

	static void generate(std::ostream &os, const ast::Module &module);

	CodeGen(CodeGenContext &ctx);

	void visit(const ast::Assignment &n) override;
	void visit(const ast::BlockStmt &n) override;
	void visit(const ast::IfStmt &n) override;
	void visit(const ast::WhileStmt &n) override;
	void visit(const ast::ReturnStmt &n) override;
	void visit(const ast::VarDef &n) override;
	void visit(const ast::FuncDecl &n) override;
	void visit(const ast::Module &n) override;
	void visit(const ast::IntLit &n) override;
	void visit(const ast::BoolLit &n) override;
	void visit(const ast::UnitLit &n) override;
	void visit(const ast::CharLit &n) override;
	void visit(const ast::UnaryExpr &n) override;
	void visit(const ast::BinaryExpr &n) override;
	void visit(const ast::VarRef &n) override;
	void visit(const ast::FuncCall &n) override;
};
}