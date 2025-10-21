#include "typedef.h"

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <optional>

/**
 * NOTE:
 * - A char literal is an i32, because the maximum size of a UTF-8 codepoint is 4 bytes.
 * - Right now a PointerType does distinguish between different pointer kinds (shared, raw, weak, unique), we won't
 *   need that in the near future, but it might be useful later on.
 * - I don't know if all names I used in this file are the best possible ones, feel free to suggest better ones.
 * - If you want to reformat this file, feel free to do so, but my formatter is trolling me too hard.
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
    UnaryExpression,
    BinaryExpression,
    CodeBlock,
    IfStatement,
    WhileStatement,
    VariableDeclaration,
    FunctionDeclaration
};

enum struct PointerKind 
{
    SharedPointer,
    RawPointer,
    WeakPointer,
    UniquePointer
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

struct Node 
{
private:
    const NodeKind m_Kind;

protected:
    Node(const NodeKind kind);
};

struct Type : public Node 
{
protected:
    Type(const NodeKind kind);
};

struct ValueType : public Type 
{
private:
    const std::string m_Typename;

public:
    ValueType(std::string &&typename_);
};

struct PointerType : public Type 
{
private:
    const std::unique_ptr<Type> m_BaseType;
    const PointerKind m_PointerKind;

public:
    PointerType(std::unique_ptr<Type> &&baseType, const PointerKind kind);
};

struct ArrayType : public Type
{
private:
    const std::unique_ptr<Type> m_ElementType;
    const std::optional<size_t> m_Size;

public:
    ArrayType(std::unique_ptr<Type> &&elementType, const std::optional<const size_t> size);
};

using ParameterTypeList = std::vector<const std::unique_ptr<const Type>> ;

struct FunctionType : public Type 
{
private:
    const ParameterTypeList m_Parameters;
    std::unique_ptr<const Type> m_ReturnType;

public:
    FunctionType(ParameterTypeList &&parameters, std::unique_ptr<const Type> &&returnType);
};

struct Statement : public Node 
{
protected:
    Statement(const NodeKind kind);
};

struct Expression : public Statement
{
protected:
    Expression(const NodeKind kind);
};

struct IntegerLiteral : public Expression 
{
private:
    const i64 m_Value;

public:
    IntegerLiteral(const i64 value);
};

struct DoubleLiteral : public Expression 
{
private:
    double m_Value;

public:
    DoubleLiteral(const double value);
};

struct CharLiteral : public Expression 
{
private:
    const i32 m_Value;

public:
    CharLiteral(const i32 value);
};

struct BoolLiteral : public Expression
{
private:
    const bool m_Value;

public:
    BoolLiteral(const bool value);
};

struct StringLiteral : public Expression 
{
private:
    const std::string m_Value;

public:
    StringLiteral(std::string &&value);
};

struct UnaryExpression : public Expression 
{
private:
    const UnaryOperatorKind m_Operator;
    const std::unique_ptr<Expression> m_Operand;

public:
    UnaryExpression(const UnaryOperatorKind operator_, std::unique_ptr<Expression> &&operand);
};

struct BinaryExpression : public Expression 
{
private:
    const BinaryOperatorKind m_Operator;
    std::unique_ptr<const Expression> m_LeftOperand, m_RightOperand;

public:
    BinaryExpression(const BinaryOperatorKind operator_, std::unique_ptr<const Expression> &&leftOperand, std::unique_ptr<Expression> &&rightOperand);
};

using StatementList = std::vector<std::unique_ptr<const Statement>>;

struct CodeBlock : public Statement 
{
private:
    const StatementList m_Statements;

public:
    CodeBlock(StatementList &&statements);
};

struct IfStatement : public Statement 
{
private:
    std::unique_ptr<const Expression> m_Condition;
    std::unique_ptr<const CodeBlock> m_ThenBlock, m_ElseBlock;

public:
    IfStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<CodeBlock> &&thenBlock, std::unique_ptr<const CodeBlock> &&elseBlock);
};

struct WhileStatement : public Statement 
{
private:
    const std::unique_ptr<Expression> m_Condition;
    const std::unique_ptr<CodeBlock> m_Body;

public:
    WhileStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&body);
};

struct VariableDeclaration : public Statement
{
private:
    std::unique_ptr<const Type> m_Type;
    std::unique_ptr<const Expression> m_Value;

public:
    VariableDeclaration(std::unique_ptr<const Type> &&type, std::unique_ptr<const Expression> &&value);
};

struct Parameter 
{
private:
    const std::string m_Name;
    std::unique_ptr<const Type> m_Type;

public:
    Parameter(std::string &&name, std::unique_ptr<const Type> &&type);
};

typedef std::vector<const Parameter> ParameterList;

struct FunctionDeclaration : public Node 
{
private:
    const std::string m_Name;
    const ParameterList m_Parameters;
    std::unique_ptr<const CodeBlock> m_Body;
public:
    FunctionDeclaration(std::string &&name, ParameterList &&parameters, std::unique_ptr<const CodeBlock> &&body);
};