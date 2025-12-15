#pragma once
#include "ast/Visitor.h"
#include "semantic/common/SymbolTable.h"
#include "semantic/common/TypeCheckerContext.h"
#include "type/Type.h"

namespace semantic {
	///
	/// Find all declaration that exist in a module. Only top level
	/// declaration are supported as of right now. That means it is
	/// not possible to nest a declaration inside another one. This
	/// has to be ensured by using correct parsing rules.
	///
	struct TypeCheckingPass : public ast::Visitor<bool> {
	private:
		TypeCheckerContext &m_Context;
		SymbolTable m_SymbolTable;
		type::TypePtr m_CurrentFunctionReturnType;

	public:
		TypeCheckingPass(TypeCheckerContext &context);
		~TypeCheckingPass();

	private:
		bool visit(ast::Module &n) override;
		bool visit(ast::IntLit &n) override;
		bool visit(ast::FloatLit &n) override;
		bool visit(ast::CharLit &n) override;
		bool visit(ast::BoolLit &n) override;
		bool visit(ast::StringLit &n) override;
		bool visit(ast::UnitLit &n) override;
		bool visit(ast::ArrayExpr &n) override;
		bool visit(ast::UnaryExpr &n) override;
		bool visit(ast::BinaryExpr &n) override;
		bool visit(ast::HeapAlloc &n) override;
		bool visit(ast::Assignment &n) override;
		bool visit(ast::FuncCall &n) override;
		bool visit(ast::VarRef &n) override;
		bool visit(ast::BlockStmt &n) override;
		bool visit(ast::IfStmt &n) override;
		bool visit(ast::WhileStmt &n) override;
		bool visit(ast::ReturnStmt &n) override;
		bool visit(ast::VarDef &n) override;
		bool visit(ast::FuncDecl &n) override;

	private:
		bool isAssignable(ast::Expr &e) const;
		bool canArgsCallFunc(const type::TypeList &args, const type::FunctionType &func) const;
		Opt<ast::BinaryOpKind> getBinaryOpFromAssignment(ast::AssignmentKind kind) const;
	};
}