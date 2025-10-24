#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "typedef.h"

/**
 * NOTE:
 * - A char literal is an i32, because the maximum size of a UTF-8 codepoint is 4 bytes.
 * - I used strong encapsulation for all members (private, protected), not sure if it's the best choice here.
 *   We will need to write a lot of getter methods later on, but we also want to avoid public access to the shared_ptrs.
 * - I only added the "biggest" numeric types for now (i64 and double), we can add smaller types later if needed.
 * - Pretty sure I forgot some AST nodes, tell me if something is missing.
 */

enum struct NodeKind 
{
    ValueType,
    PointerType,
    ArrayType,
    FunctionType,
    IntegerLiteral,
    DoubleLiteral,
    CharLiteral,
    BoolLiteral,
    StringLiteral,
    ArrayLiteral,
    UnaryExpression,
    BinaryExpression,
    FunctionCall,
    VariableUse,
    CodeBlock,
    IfStatement,
    WhileStatement,
    VariableDeclaration,
    FunctionDeclaration
};

enum struct UnaryOperatorKind
{
    LogicalNot,
    BitwiseNot,
    Positive,
    Negative
};

enum struct BinaryOperatorKind
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

struct Visitor;

struct Node 
{
private:
    const NodeKind m_Kind;

protected:
    Node(const NodeKind kind);

public:
    virtual ~Node() = default;

    virtual void accept(Visitor &visitor) const = 0;
};

struct Type : public Node 
{
protected:
    Type(const NodeKind kind);
    
public:
    virtual void accept(Visitor &visitor) const override = 0;
};

struct ValueType : public Type 
{
public:
    const std::string m_Typename;

    ValueType(std::string &&typename_);
    
    virtual void accept(Visitor &visitor) const override;
};

struct PointerType : public Type 
{
public:
    const std::unique_ptr<const Type> m_BaseType;

    PointerType(std::unique_ptr<const Type> &&baseType);

    virtual void accept(Visitor &visitor) const override;
};

struct ArrayType : public Type
{
public:
    const std::unique_ptr<const Type> m_ElementType;
    const std::optional<const size_t> m_Size;

    ArrayType(std::unique_ptr<const Type> &&elementType, const std::optional<const size_t> size);

    virtual void accept(Visitor &visitor) const override;
};

using ParameterTypeList = std::vector<std::unique_ptr<const Type>> ;

struct FunctionType : public Type 
{
public:    
    const ParameterTypeList m_Parameters;
    const std::unique_ptr<const Type> m_ReturnType;

    FunctionType(ParameterTypeList &&parameters, std::unique_ptr<const Type> &&returnType);

    virtual void accept(Visitor &visitor) const override;
};

struct Statement : public Node 
{
protected:
    Statement(const NodeKind kind);

public:
    virtual void accept(Visitor &visitor) const override = 0;
};

struct Expression : public Statement
{
protected:
    Expression(const NodeKind kind);

public:
    virtual void accept(Visitor &visitor) const override = 0;
};

struct IntegerLiteral : public Expression 
{
public:
    const i64 m_Value;

    IntegerLiteral(const i64 value);

    virtual void accept(Visitor &visitor) const override;
};

struct DoubleLiteral : public Expression 
{
public:
    const double m_Value;

    DoubleLiteral(const double value);

    virtual void accept(Visitor &visitor) const override;
};

struct CharLiteral : public Expression 
{
public:
    const i32 m_Value;

    CharLiteral(const i32 value);

    virtual void accept(Visitor &visitor) const override;
};

struct BoolLiteral : public Expression
{
public:
    const bool m_Value;

    BoolLiteral(const bool value);

    virtual void accept(Visitor &visitor) const override;
};

struct StringLiteral : public Expression 
{
public:
    const std::string m_Value;

    StringLiteral(std::string &&value);

    virtual void accept(Visitor &visitor) const override;
};

using ArgumentList = std::vector<std::unique_ptr<const Expression>>;

struct ArrayLiteral : public Expression 
{
public:
    const std::unique_ptr<const ArrayType> m_Type;
    const ArgumentList m_Values;

    ArrayLiteral(std::unique_ptr<const ArrayType> &&type, ArgumentList &&values);

    virtual void accept(Visitor &visitor) const override;
};

struct UnaryExpression : public Expression 
{
public:
    const UnaryOperatorKind m_Operator;
    const std::unique_ptr<const Expression> m_Operand;

    UnaryExpression(const UnaryOperatorKind operator_, std::unique_ptr<const Expression> &&operand);

    virtual void accept(Visitor &visitor) const override;
};

struct BinaryExpression : public Expression 
{
public:
    const BinaryOperatorKind m_Operator;
    const std::unique_ptr<const Expression> m_LeftOperand, m_RightOperand;

    BinaryExpression(const BinaryOperatorKind operator_, std::unique_ptr<const Expression> &&leftOperand, std::unique_ptr<const Expression> &&rightOperand);
    
    virtual void accept(Visitor &visitor) const override;
};

struct VariableUse : public Expression
{
public:
    const std::string m_Name;

    VariableUse(std::string &&name);

    virtual void accept(Visitor &visitor) const override;
};

struct FunctionCall : public Expression
{
public:
    const std::string m_Name;
    const ArgumentList m_Arguments;

    FunctionCall(std::string &&name, ArgumentList &&arguments);
    
    virtual void accept(Visitor &visitor) const override;
};

using StatementList = std::vector<std::unique_ptr<const Statement>>;

struct CodeBlock : public Statement 
{
public:
    const StatementList m_Statements;

    CodeBlock(StatementList &&statements);

    virtual void accept(Visitor &visitor) const override;
};

struct IfStatement : public Statement 
{
public:
    const std::unique_ptr<const Expression> m_Condition;
    const std::unique_ptr<const CodeBlock> m_ThenBlock, m_ElseBlock;

    IfStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&thenBlock, std::unique_ptr<const CodeBlock> &&elseBlock);
    
    virtual void accept(Visitor &visitor) const override;
};

struct WhileStatement : public Statement 
{
public:
    const std::unique_ptr<const Expression> m_Condition;
    const std::unique_ptr<const CodeBlock> m_Body;

    WhileStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&body);
    
    virtual void accept(Visitor &visitor) const override;
};

struct VariableDeclaration : public Statement
{
public:
    const std::string m_Name;
    const std::unique_ptr<const Type> m_Type;
    const std::unique_ptr<const Expression> m_Value;

    VariableDeclaration(std::string &&name, std::unique_ptr<const Type> &&type, std::unique_ptr<const Expression> &&value);
    
    virtual void accept(Visitor &visitor) const override;
};

struct Parameter 
{
public:
    const std::string m_Name;
    const std::unique_ptr<const Type> m_Type;

    Parameter(std::string &&name, std::unique_ptr<const Type> &&type);
};

using ParameterList = std::vector<Parameter>;

struct FunctionDeclaration : public Node 
{
public:
    const std::string m_Name;
    const ParameterList m_Parameters;
    const std::unique_ptr<const Type> m_ReturnType;
    const std::unique_ptr<const CodeBlock> m_Body;

    FunctionDeclaration(std::string &&name, ParameterList &&parameters, std::unique_ptr<const Type> &&returnType, std::unique_ptr<const CodeBlock> &&body);
    
    virtual void accept(Visitor &visitor) const override;
};

struct Visitor 
{
public:
    virtual ~Visitor() = default;

    virtual void visit(const ValueType &node) = 0;
    virtual void visit(const PointerType &node) = 0;
    virtual void visit(const ArrayType &node) = 0;
    virtual void visit(const FunctionType &node) = 0;
    virtual void visit(const IntegerLiteral &node) = 0;
    virtual void visit(const DoubleLiteral &node) = 0;
    virtual void visit(const CharLiteral &node) = 0;
    virtual void visit(const BoolLiteral &node) = 0;
    virtual void visit(const StringLiteral &node) = 0;
    virtual void visit(const ArrayLiteral &node) = 0;
    virtual void visit(const UnaryExpression &node) = 0;
    virtual void visit(const BinaryExpression &node) = 0;
    virtual void visit(const FunctionCall &node) = 0;
    virtual void visit(const VariableUse &node) = 0;
    virtual void visit(const CodeBlock &node) = 0;
    virtual void visit(const IfStatement &node) = 0;
    virtual void visit(const WhileStatement &node) = 0;
    virtual void visit(const VariableDeclaration &node) = 0;
    virtual void visit(const FunctionDeclaration &node) = 0;
};