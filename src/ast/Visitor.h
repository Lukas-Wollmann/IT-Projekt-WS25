#pragma once

struct IntLit;
struct FloatLit;
struct CharLit;
struct BoolLit;
struct StringLit;
struct ArrayExpr;
struct UnaryExpr;
struct BinaryExpr;
struct FuncCall;
struct VarRef;
struct CodeBlock;
struct IfStmt;
struct WhileStmt;
struct ReturnStmt;
struct VarDecl;
struct FuncDecl;

struct Visitor
{
    virtual ~Visitor() = default;

    virtual void visit(IntLit &node) = 0;
    virtual void visit(FloatLit &node) = 0;
    virtual void visit(CharLit &node) = 0;
    virtual void visit(BoolLit &node) = 0;
    virtual void visit(StringLit &node) = 0;
    virtual void visit(ArrayExpr &node) = 0;
    virtual void visit(UnaryExpr &node) = 0;
    virtual void visit(BinaryExpr &node) = 0;
    virtual void visit(FuncCall &node) = 0;
    virtual void visit(VarRef &node) = 0;
    virtual void visit(CodeBlock &node) = 0;
    virtual void visit(IfStmt &node) = 0;
    virtual void visit(WhileStmt &node) = 0;
    virtual void visit(ReturnStmt &node) = 0;
    virtual void visit(VarDecl &node) = 0;
    virtual void visit(FuncDecl &node) = 0;
};