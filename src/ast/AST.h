#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <optional>
#include "Typedef.h"
#include "Visitor.h"
#include "Type.h"

struct Node;
struct Expr;
struct Stmt;
struct CodeBlock;

using ExprPtr = std::unique_ptr<Expr>;
using ExprList = std::vector<ExprPtr>;
using StmtPtr = std::unique_ptr<Stmt>;
using StmtList = std::vector<StmtPtr>;
using CodeBlockPtr = std::unique_ptr<CodeBlock>;

enum struct NodeKind 
{
    IntLit,
    FloatLit,
    CharLit,
    BoolLit,
    StringLit,
    ArrayExpr,
    UnaryExpr,
    BinaryExpr,
    FuncCall,
    VarRef,
    CodeBlock,
    IfStmt,
    WhileStmt,
    ReturnStmt,
    VarDecl,
    FuncDecl
};

struct Node 
{
private:
    const NodeKind m_Kind;

protected:
    explicit Node(NodeKind kind);

public:
    virtual ~Node()  = default;

    virtual void toString(std::ostream &os) const = 0;
    virtual void accept(Visitor &v) = 0;

    NodeKind getNodeKind() const;

    friend std::ostream &operator<<(std::ostream &os, const Node &node);
};

struct Stmt : public Node 
{
protected:
    explicit Stmt(NodeKind kind);

public:
    void toString(std::ostream &os) const override = 0;
    void accept(Visitor &v) override = 0;
};

struct Expr : public Stmt
{
private:
    std::optional<TypePtr> m_Type;

protected:
    explicit Expr(NodeKind kind);

public:
    void toString(std::ostream &os) const override = 0;
    void accept(Visitor &v) override = 0;

    std::optional<Ref<const Type>> getType() const;
    void setType(TypePtr type) { m_Type = std::move(type); }
};

struct IntLit : public Expr 
{
private:
    const i32 m_Value;

public:    
    explicit IntLit(i32 value);

    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    i32 getValue() const { return m_Value; }
};

struct FloatLit : public Expr 
{
private:
    const f32 m_Value;
    
public:
    explicit FloatLit(f32 value);

    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    f32 getValue() const { return m_Value; }
};

struct CharLit : public Expr 
{
private:
    const char32_t m_Value;
    
public:
    explicit CharLit(char32_t value);

    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    char32_t getValue() const { return m_Value; }
};

struct BoolLit : public Expr
{
private:
    const bool m_Value;
    
public:
    explicit BoolLit(bool value);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    bool getValue() const { return m_Value; }
};

struct StringLit : public Expr
{
private:
    const std::string m_Value;
    
public:
    explicit StringLit(std::string value);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string getValue() const { return m_Value; }
};

struct ArrayExpr : public Expr 
{
private:
    TypePtr m_ElemType;
    ExprList m_Values;

public:
    explicit ArrayExpr(TypePtr elemType, ExprList values);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const Type &getElementType() const { return *m_ElemType; }
};

enum struct UnaryOpKind 
{ 
    LogicalNot, 
    BitwiseNot, 
    Positive, 
    Negative 
};

std::ostream &operator<<(std::ostream &os, UnaryOpKind op);

struct UnaryExpr : public Expr 
{
private:
    const UnaryOpKind m_Op;
    ExprPtr m_Operand;

public:
    explicit UnaryExpr(UnaryOpKind op, ExprPtr operand);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    UnaryOpKind getOp() const { return m_Op; }
    Expr &getOperand() const { return *m_Operand; }
};

enum struct BinaryOpKind
{
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Modulo,
    Equality,
    Inequality,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,
    LogicalAnd,
    LogicalOr,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    LeftShift,
    RightShift,
    Assignment,
    AdditionAssignment,
    SubtractionAssignment,
    MultiplicationAssignment,
    DivisionAssignment,
    ModuloAssignment,
    BitwiseAndAssignment,
    BitwiseOrAssignment,
    BitwiseXorAssignment,
    LeftShiftAssignment,
    RightShiftAssignment
};

std::ostream &operator<<(std::ostream &os, BinaryOpKind op);

struct BinaryExpr : public Expr 
{
private:
    const BinaryOpKind m_Op;
    ExprPtr m_LeftOp, m_RightOp;

public:
    explicit BinaryExpr(BinaryOpKind op, ExprPtr leftOp, ExprPtr rightOp);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    BinaryOpKind getOp() const { return m_Op; }
    Expr &getLeftOp() const { return *m_LeftOp; }
    Expr &getRightOp() const { return *m_RightOp; }
};

struct VarRef : public Expr
{
private:
    const std::string m_Ident;
    
public:
    explicit VarRef(std::string ident);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string &getIdent() const { return m_Ident; }
};

struct FuncCall : public Expr
{
private:
    const std::string m_Ident;
    ExprList m_Args;
    
public:
    explicit FuncCall(std::string ident, ExprList args);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string &getIdent() const { return m_Ident; }
    ExprList &getArgs() { return m_Args; }
};

struct CodeBlock : public Stmt 
{
private:
    StmtList m_Stmts;
    
public:
    explicit CodeBlock(StmtList stmts);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    StmtList &getStmts() { return m_Stmts; }
};

struct IfStmt : public Stmt 
{
private:
    ExprPtr m_Cond;
    CodeBlockPtr m_Then, m_Else;

public:
    explicit IfStmt(ExprPtr cond, CodeBlockPtr then, CodeBlockPtr else_);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    Expr &getCond() const { return *m_Cond; }
    CodeBlock &getThen() const { return *m_Then; }
    CodeBlock &getElse() const { return *m_Else; }
};

struct WhileStmt : public Stmt 
{
private:
    ExprPtr m_Cond;
    CodeBlockPtr m_Body;
    
public:
    explicit WhileStmt(ExprPtr cond, CodeBlockPtr body);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    Expr &getCond() const { return *m_Cond; }
    CodeBlock &getBody() const { return *m_Body; }
};

struct ReturnStmt : public Stmt
{
private:
    ExprPtr m_Expr;

public:
    explicit ReturnStmt(ExprPtr expr);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    Expr &getExpr() const { return *m_Expr; }
};

struct VarDecl : public Stmt
{
private:
    const std::string m_Ident;
    TypePtr m_Type;
    ExprPtr m_Value;

public:
    explicit VarDecl(std::string name, TypePtr type, ExprPtr value);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string &getIdent() const { return m_Ident; }
    const Type &getType() const { return *m_Type; }
    Expr &getValue() const { return *m_Value; }
};

using Param = std::pair<std::string, TypePtr>;
using ParamList = std::vector<Param>;

struct FuncDecl : public Node 
{

private:
    const std::string m_Ident;
    ParamList m_Params;
    TypePtr m_ReturnType;
    CodeBlockPtr m_Body;

public:
    explicit FuncDecl(std::string name, ParamList params, TypePtr returnType, CodeBlockPtr body);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string &getIdent() const { return m_Ident; };
    ParamList &getParams() { return m_Params; } 
    const Type &getReturnType() const { return *m_ReturnType; }
    CodeBlock &getBody() const { return *m_Body; }
};
