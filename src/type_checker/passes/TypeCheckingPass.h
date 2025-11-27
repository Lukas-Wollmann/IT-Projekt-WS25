#pragma once
#include "type_checker/common/SymbolTable.h"
#include "type_checker/common/TypeCheckerContext.h"
#include "ast/Type.h"
#include "ast/Visitor.h"

struct TypeCheckingPass : public Visitor<void>
{
private:
    TypeCheckerContext &m_Context;
    SymbolTable m_SymbolTable;
    TypePtr m_CurrentFunctionReturnType;
    
public:
    TypeCheckingPass(TypeCheckerContext &context);
    ~TypeCheckingPass();
   
    void visit(ast::Module &n) override;

private:
    void visit(ast::IntLit &n) override;
    void visit(ast::FloatLit &n) override;
    void visit(ast::CharLit &n) override;
    void visit(ast::BoolLit &n) override;
    void visit(ast::StringLit &n) override;
    void visit(ast::ArrayExpr &n) override;
    void visit(ast::UnaryExpr &n) override;
    void visit(ast::BinaryExpr &n) override;
    void visit(ast::FuncCall &n) override;
    void visit(ast::VarRef &n) override;
    void visit(ast::BlockStmt &n) override;
    void visit(ast::IfStmt &n) override;
    void visit(ast::WhileStmt &n) override;
    void visit(ast::ReturnStmt &n) override;
    void visit(ast::VarDef &n) override;
    void visit(ast::FuncDecl &n) override;
};