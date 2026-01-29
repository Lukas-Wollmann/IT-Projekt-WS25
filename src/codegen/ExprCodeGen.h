#pragma once
#include "ast/Visitor.h"

namespace llvm {
class Value;
}

namespace gen {

struct CodeGenContext;

struct RValueCodeGen : public ast::ConstVisitor<llvm::Value *> {
private:
	CodeGenContext &m_Context;

public:
	RValueCodeGen(CodeGenContext &ctx);

private:
	llvm::Value *visit(const ast::IntLit &n) override;
	llvm::Value *visit(const ast::BoolLit &n) override;
	llvm::Value *visit(const ast::UnitLit &n) override;
	llvm::Value *visit(const ast::CharLit &n) override;
	llvm::Value *visit(const ast::VarRef &n) override;
	llvm::Value *visit(const ast::UnaryExpr &n) override;
	llvm::Value *visit(const ast::BinaryExpr &n) override;
};

struct LValueCodeGen : public ast::ConstVisitor<llvm::Value *> {
private:
	CodeGenContext &m_Context;

public:
	LValueCodeGen(CodeGenContext &ctx);

private:
	llvm::Value *visit(const ast::VarRef &n) override;
	llvm::Value *visit(const ast::UnaryExpr &n) override;
};
}