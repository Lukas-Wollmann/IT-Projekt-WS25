#pragma once
#include "AllocManager.h"
#include "CodeGenContext.h"
#include "ast/Visitor.h"

namespace gen {
struct ExprResult {
	llvm::Value *value;
	Type type;
	bool isTemp;
};

struct ExprLowerer : ast::ConstVisitor<ExprResult> {
private:
	CodeGenContext &m_Context;
	AllocManager &m_AllocManager;
	Vec<TrackedValue> m_ExprCleanup;

public:
	ExprLowerer(CodeGenContext &ctx, AllocManager &allocManager);

	ExprResult lowerExpr(const ast::Expr &n);

	void addToExprCleanup(llvm::Value *value, Type type);
	void emitExprCleanup();
	void removeFromExprCleanup(llvm::Value *value);

	ExprResult lowerLValue(const ast::Expr &n);
	ExprResult visit(const ast::IntLit &n) override;
	ExprResult visit(const ast::BoolLit &n) override;
	ExprResult visit(const ast::CharLit &n) override;
	ExprResult visit(const ast::NullLit &n) override;
	ExprResult visit(const ast::UnitLit &n) override;
	ExprResult visit(const ast::HeapAlloc &n) override;
	ExprResult visit(const ast::StructInit &n) override;
	ExprResult visit(const ast::VarRef &n) override;
	ExprResult visit(const ast::FieldAccess &n) override;
	ExprResult visit(const ast::UnaryExpr &n) override;
	ExprResult visit(const ast::BinaryExpr &n) override;
	ExprResult visit(const ast::FuncCall &n) override;
	ExprResult visit(const ast::Assignment &n) override;
};
}