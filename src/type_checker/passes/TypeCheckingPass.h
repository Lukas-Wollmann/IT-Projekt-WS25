#pragma once
#include "type_checker/common/SymbolTable.h"
#include "type_checker/common/TypeCheckerContext.h"
#include "ast/AST.h"
#include "ast/Type.h"
#include "ast/Visitor.h"

struct TypeCheckingPass : public Visitor
{
private:
    TypeCheckerContext &m_Context;
    SymbolTable m_SymbolTable;
    TypePtr m_CurrentFunctionReturnType;
    
public:
    TypeCheckingPass(TypeCheckerContext &context);
    ~TypeCheckingPass();

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
    void visit(Module &node) override;
};