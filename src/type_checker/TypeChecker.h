#pragma once
#include "type_checker/SymbolTable.h"
#include "ast/AST.h"
#include "ast/Type.h"
#include "ast/Visitor.h"


struct TypeError
{
public:
    std::string m_Msg;

    TypeError(std::string msg);
};


struct TypeChecker : public Visitor
{
public:
    SymbolTable m_SymbolTable;
    std::vector<TypeError> m_Errors;
    TypePtr m_CurrentFunctionReturnType;

    TypeChecker();

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