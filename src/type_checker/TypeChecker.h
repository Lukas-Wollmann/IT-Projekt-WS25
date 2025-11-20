#pragma once
#include "type_checker/SymbolTable.h"
#include "Token.h"
#include "ast/AST.h"
#include "ast/Type.h"
#include "ast/Visitor.h"

struct TypeCheckingPass : public Visitor
{
private:
    TypeCheckingContext &m_Context;
    TypePtr m_CurrentFunctionReturnType;
    
public:
    TypeCheckingPass(TypeCheckingContext &context);

    void visit(IntLit &node) override;
    void visit(FloatLit &node) override;
    void visit(CharLit &node) override;
    void visit(BoolLit &node) override;
    void visit(StringLit &node) override;
    void visit(ArrayExpr &node) override;
    void visit(UnaryExpr &node) override;
    void visit(BinaryExpr &node) override;
    void visit(FuncCall &node) override;
    void visit(VarRef &node) override;
    void visit(CodeBlock &node) override;
    void visit(IfStmt &node) override;
    void visit(WhileStmt &node) override;
    void visit(ReturnStmt &node) override;
    void visit(VarDecl &node) override;
    void visit(FuncDecl &node) override;
};