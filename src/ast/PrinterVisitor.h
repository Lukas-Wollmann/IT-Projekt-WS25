#pragma once
#include "AST.h"

struct PrintVisitor : public Visitor
{
    friend std::ostream &operator<<(std::ostream &os, const Node &node);
    
private:
    std::ostream &m_Stream;

public:
    explicit PrintVisitor(std::ostream &stream);

    virtual void visit(const ValueType &node) override;
    virtual void visit(const PointerType &node) override;
    virtual void visit(const ArrayType &node) override; 
    virtual void visit(const FunctionType &node) override;
    virtual void visit(const IntegerLiteral &node) override;
    virtual void visit(const DoubleLiteral &node) override;
    virtual void visit(const CharLiteral &node) override;
    virtual void visit(const BoolLiteral &node) override;
    virtual void visit(const StringLiteral &node) override;
    virtual void visit(const ArrayLiteral &node) override;
    virtual void visit(const UnaryExpression &node) override;
    virtual void visit(const BinaryExpression &node) override;
    virtual void visit(const FunctionCall &node) override;
    virtual void visit(const VariableUse &node) override;
    virtual void visit(const CodeBlock &node) override;
    virtual void visit(const IfStatement &node) override;
    virtual void visit(const WhileStatement &node) override;
    virtual void visit(const ReturnStatement &node) override;
    virtual void visit(const VariableDeclaration &node) override;
    virtual void visit(const FunctionDeclaration &node) override;
};

std::ostream &operator<<(std::ostream &os, const Node &node); 
std::ostream &operator<<(std::ostream &os, UnaryOperatorKind op);
std::ostream &operator<<(std::ostream &os, BinaryOperatorKind op);