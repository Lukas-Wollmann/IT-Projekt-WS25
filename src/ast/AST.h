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

using ExprPtr = std::unique_ptr<const Expr>;
using ExprList = std::vector<ExprPtr>;
using StmtPtr = std::unique_ptr<const Stmt>;
using StmtList = std::vector<StmtPtr>;
using CodeBlockPtr = std::unique_ptr<const CodeBlock>;

struct Node 
{
    enum struct Kind 
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

private:
    const Kind m_Kind;

protected:
    explicit Node(Kind kind);

public:
    virtual ~Node()  = default;

    virtual void toString(std::ostream &os) const = 0;
    virtual void accept(Visitor &v) = 0;

    Kind getNodeKind() const;

    friend std::ostream &operator<<(std::ostream &os, const Node &node);
};

struct Stmt : public Node 
{
protected:
    explicit Stmt(Kind kind);

public:
    void toString(std::ostream &os) const override = 0;
    void accept(Visitor &v) = 0;
};

struct Expr : public Stmt
{
private:
    std::optional<TypePtr> m_Type;

protected:
    explicit Expr(Kind kind);

public:
    void toString(std::ostream &os) const override = 0;
    void accept(Visitor &v) = 0;

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
    const TypePtr m_ElemType;
    const ExprList m_Values;

public:
    explicit ArrayExpr(TypePtr elemType, ExprList values);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const TypePtr &getElementType() const { return m_ElemType; }
};

struct UnaryExpr : public Expr 
{
    enum struct OperatorKind { LogicalNot, BitwiseNot, Positive, Negative };

private:
    const OperatorKind m_Op;
    const ExprPtr m_Operand;

public:
    explicit UnaryExpr(OperatorKind op, ExprPtr operand);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    OperatorKind getOp() const { return m_Op; }
    const Expr &getOperand() const { return *m_Operand; }

    friend std::ostream &operator<<(std::ostream &os, OperatorKind op);
};

struct BinaryExpr : public Expr 
{
    enum struct OperatorKind
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

private:
    const OperatorKind m_Op;
    const ExprPtr m_LeftOp, m_RightOp;

public:
    explicit BinaryExpr(OperatorKind op, ExprPtr leftOp, ExprPtr rightOp);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    OperatorKind getOp() const { return m_Op; }
    const Expr &getLeftOp() const { return *m_LeftOp; }
    const Expr &getRightOp() const { return *m_RightOp; }

    friend std::ostream &operator<<(std::ostream &os, OperatorKind op);
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
    const ExprList m_Args;
    
public:
    explicit FuncCall(std::string ident, ExprList args);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string &getIdent() const { return m_Ident; }
    const ExprList &getArgs() const { return m_Args; }
};

struct CodeBlock : public Stmt 
{
private:
    const StmtList m_Stmts;
    
public:
    explicit CodeBlock(StmtList stmts);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const StmtList &getStmts() const { return m_Stmts; }
};

struct IfStmt : public Stmt 
{
private:
    const ExprPtr m_Cond;
    const CodeBlockPtr m_Then, m_Else;

public:
    explicit IfStmt(ExprPtr cond, CodeBlockPtr then, CodeBlockPtr else_);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const ExprPtr &getCond() const { return m_Cond; }
    const CodeBlockPtr &getThen() const { return m_Then; }
    const CodeBlockPtr &getElse() const { return m_Else; }
};

struct WhileStmt : public Stmt 
{
private:
    const ExprPtr m_Cond;
    const CodeBlockPtr m_Body;
    
public:
    explicit WhileStmt(ExprPtr cond, CodeBlockPtr body);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const ExprPtr &getCond() const { return m_Cond; }
    const CodeBlockPtr &getBody() const { return m_Body; }
};

struct ReturnStmt : public Stmt
{
private:
    const ExprPtr m_Expr;

public:
    explicit ReturnStmt(ExprPtr expr);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const ExprPtr &getExpr() const { return m_Expr; }
};

struct VarDecl : public Stmt
{
private:
    const std::string m_Ident;
    const TypePtr m_Type;
    const ExprPtr m_Value;

public:
    explicit VarDecl(std::string name, TypePtr type, ExprPtr value);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string &getIdent() const { return m_Ident; }
    const Type &getType() const { return *m_Type; }
    const Expr &getValue() const { return *m_Value; }
};

struct FuncDecl : public Node 
{
    using Param = std::pair<std::string, TypePtr>;
    using ParamList = std::vector<Param>;

private:
    const std::string m_Ident;
    const ParamList m_Params;
    const TypePtr m_ReturnType;
    const CodeBlockPtr m_Body;

public:
    explicit FuncDecl(std::string name, ParamList params, TypePtr returnType, CodeBlockPtr body);
    
    void toString(std::ostream &os) const override;
    void accept(Visitor &v) override { v.visit(*this); }

    const std::string &getIdent() const { return m_Ident; };
    const ParamList &getParams() const { return m_Params; } 
    const TypePtr &getReturnType() const { return m_ReturnType; }
    const CodeBlockPtr &getBody() const { return m_Body; }
};
