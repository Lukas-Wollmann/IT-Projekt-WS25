#pragma once
#include "ast/Visitor.h"
#include "core/Operators.h"
#include "semantic/common/SymbolTable.h"
#include "semantic/common/TypeCheckerContext.h"
#include "type/TypeFactory.h"

namespace sem {
///
/// Type check all nodes of the AST, we use a recursive, bottom up appreach to infer
/// all types and value categories. If a type error happenes, report it and propagate
/// it upwards inside the tree. Error messages are stored as template specializations
/// inside ErrorMessages.h.
///
struct TypeCheckingPass : ast::Visitor<bool> {
private:
	TypeCheckerContext &m_Context;
	SymbolTable m_SymbolTable;
	Opt<Type> m_CurrentFunctionReturnType;

public:
	explicit TypeCheckingPass(TypeCheckerContext &ctx);

private:
	bool visit(ast::Module &n) override;
	bool visit(ast::IntLit &n) override;
	bool visit(ast::CharLit &n) override;
	bool visit(ast::BoolLit &n) override;
	bool visit(ast::UnitLit &n) override;
	bool visit(ast::HeapAlloc &n) override;
	bool visit(ast::UnaryExpr &n) override;
	bool visit(ast::BinaryExpr &n) override;
	bool visit(ast::Assignment &n) override;
	bool visit(ast::FuncCall &n) override;
	bool visit(ast::VarRef &n) override;
	bool visit(ast::BlockStmt &n) override;
	bool visit(ast::IfStmt &n) override;
	bool visit(ast::WhileStmt &n) override;
	bool visit(ast::ReturnStmt &n) override;
	bool visit(ast::VarDef &n) override;
	bool visit(ast::FuncDecl &n) override;

	Type checkExpression(ast::Expr &n);
	[[nodiscard]] static bool typesMatch(Type left, Type right);
	void checkIfArgsCanCallFunction(const TypeList &args, const FunctionType *func) const;
	[[nodiscard]] static Opt<BinaryOpKind> getBinaryOpFromAssignment(AssignmentKind kind);
};
}
