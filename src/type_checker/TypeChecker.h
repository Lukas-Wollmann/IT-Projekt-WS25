#pragma once
#include "type_checker/SymbolTable.h"
#include "ast/AST.h"
#include "ast/Type.h"


struct TypeError
{
public:
    std::string m_Msg;

    TypeError(std::string msg);
};


struct TypeChecker
{
public:
    SymbolTable m_SymbolTable;
    std::vector<TypeError> m_Errors;

    TypeChecker();

    void checkUnaryExpr(const UnaryExpr &expr);
    void checkBinaryExpr(const BinaryExpr &expr);
    void checkVarRef(const VarRef &expr);
    void checkExpr(const Expr &expr);

    void checkVarDecl(const VarDecl &stmt);
    void checkCodeBlock(const CodeBlock &stmt);
    void checkStmt(const Stmt &stmt);
};