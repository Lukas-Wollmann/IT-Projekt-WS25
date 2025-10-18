#include "Types.h"

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
    StringLiteral,
    UnaryExpression,
    BinaryExpression,
    CodeBlock,
    IfStatement,
    WhileStatement,
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
protected:
    NodeKind m_Kind;

    Node(NodeKind kind);
};

struct Type : public Node 
{
protected:
    Type(NodeKind kind);
};

struct ValueType : public Type 
{
private:
    std::string m_Typename;

public:
    ValueType(std::string &&typename_);
};

struct PointerType : public Type 
{
private:
    std::unique_ptr<Type> m_BaseType;
    PointerKind m_PointerKind;

public:
    PointerType(std::unique_ptr<Type> &&baseType, PointerKind kind);
};

struct ArrayType : public Type
{
private:
    std::unique_ptr<Type> m_ElementType;
    std::optional<size_t> m_Size;

public:
    ArrayType(std::unique_ptr<Type> &&elementType, std::optional<size_t> size);
};

using ParameterTypeList = std::vector<std::unique_ptr<Type>> ;

struct FunctionType : public Type 
{
private:
    ParameterTypeList m_Parameters;
    std::unique_ptr<Type> m_ReturnType;

public:
    FunctionType(ParameterTypeList &&parameters, std::unique_ptr<Type> &&returnType);
};

struct Statement : public Node 
{
protected:
    Statement(NodeKind kind);
};

struct Expression : public Statement
{
protected:
    Expression(NodeKind kind);
};

struct IntegerLiteral : public Expression 
{
private:
    i64 m_Value;

public:
    IntegerLiteral(i64 value);
};

struct DoubleLiteral : public Expression 
{
private:
    double m_Value;

public:
    DoubleLiteral(double value);
};

struct CharLiteral : public Expression 
{
private:
    i32 m_Value;

public:
    CharLiteral(i32 value);
};

struct StringLiteral : public Expression 
{
private:
    std::string m_Value;

public:
    StringLiteral(std::string &&value);
};

struct UnaryExpression : public Expression 
{
private:
    UnaryOperatorKind m_Operator;
    std::unique_ptr<Expression> m_Operand;

public:
    UnaryExpression(UnaryOperatorKind operator_, std::unique_ptr<Expression> &&operand);
};

struct BinaryExpression : public Expression 
{
private:
    BinaryOperatorKind m_Operator;
    std::unique_ptr<Expression> m_LeftOperand, m_RightOperand;

public:
    BinaryExpression(BinaryOperatorKind operator_, std::unique_ptr<Expression> &&leftOperand, std::unique_ptr<Expression> &&rightOperand);
};

using StatementList = std::vector<std::unique_ptr<Statement>>;

struct CodeBlock : public Statement 
{
private:
    StatementList m_Statements;

public:
    CodeBlock(StatementList &&statements);
};

struct IfStatement : public Statement 
{
private:
    std::unique_ptr<Expression> m_Condition;
    std::unique_ptr<CodeBlock> m_ThenBlock, m_ElseBlock;

public:
    IfStatement(std::unique_ptr<Expression> &&condition, std::unique_ptr<CodeBlock> &&thenBlock, std::unique_ptr<CodeBlock> &&elseBlock);
};

struct WhileStatement : public Statement 
{
private:
    std::unique_ptr<Expression> m_Condition;
    std::unique_ptr<CodeBlock> m_Body;

public:
    WhileStatement(std::unique_ptr<Expression> &&condition, std::unique_ptr<CodeBlock> &&body);
};

struct Parameter 
{
private:
    std::string m_Name;
    std::unique_ptr<Type> m_Type;

public:
    Parameter(std::string &&name, std::unique_ptr<Type> &&type);
};

typedef std::vector<Parameter> ParameterList;

struct FunctionDeclaration : public Node 
{
private:
    std::string m_Name;
    ParameterList m_Parameters;
    std::unique_ptr<CodeBlock> m_Body;
public:
    FunctionDeclaration(std::string &&name, ParameterList &&parameters, std::unique_ptr<CodeBlock> &&body);
};