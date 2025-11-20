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

    virtual void visit(IntLit &) {}
    virtual void visit(FloatLit &) {}
    virtual void visit(CharLit &) {}
    virtual void visit(BoolLit &) {}
    virtual void visit(StringLit &) {}
    virtual void visit(ArrayExpr &) {}
    virtual void visit(UnaryExpr &) {}
    virtual void visit(BinaryExpr &) {}
    virtual void visit(FuncCall &) {}
    virtual void visit(VarRef &) {}
    virtual void visit(CodeBlock &) {}
    virtual void visit(IfStmt &) {}
    virtual void visit(WhileStmt &) {}
    virtual void visit(ReturnStmt &) {}
    virtual void visit(VarDecl &) {}
    virtual void visit(FuncDecl &) {}
};