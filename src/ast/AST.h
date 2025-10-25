#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "../Typedef.h"

/**
 * NOTE:
 * - A char literal is an i32, because the maximum size of a UTF-8 codepoint is 4 bytes.
 * - All members are private with inline getters (no raw pointers exposed).
 * - std::string parameters are taken by value (move into members in .cpp).
 * - Constructors are declared only (no implementations here).
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
    explicit Node(const NodeKind kind);

public:
    virtual ~Node()  = default;

    NodeKind getNodeKind() const;

    virtual void accept(Visitor &visitor) const = 0;
};

struct Type : public Node 
{
protected:
    explicit Type(const NodeKind kind);
    
public:
    virtual void accept(Visitor &visitor) const override = 0;
};

struct ValueType : public Type 
{
public:
    const std::string typeName;

    explicit ValueType(std::string typeName);

    virtual void accept(Visitor &visitor) const override;
};

struct PointerType : public Type 
{
public:
    const std::unique_ptr<const Type> baseType;

    explicit PointerType(std::unique_ptr<const Type> &&baseType);

    virtual void accept(Visitor &visitor) const override;
};

struct ArrayType : public Type
{
public:
    const std::unique_ptr<const Type> elementType;
    const std::optional<size_t> size;

    explicit ArrayType(std::unique_ptr<const Type> &&elementType, const std::optional<size_t> size);

    virtual void accept(Visitor &visitor) const override;
};

using ParameterTypeList = std::vector<std::unique_ptr<const Type>>;

struct FunctionType : public Type 
{    
public:
    const ParameterTypeList parameters;
    const std::unique_ptr<const Type> returnType;

    explicit FunctionType(ParameterTypeList &&parameters, std::unique_ptr<const Type> &&returnType);

    virtual void accept(Visitor &visitor) const override;
};

struct Statement : public Node 
{
protected:
    explicit Statement(const NodeKind kind);

public:
    virtual void accept(Visitor &visitor) const override = 0;
};

struct Expression : public Statement
{
protected:
    explicit Expression(const NodeKind kind);

public:
    virtual void accept(Visitor &visitor) const override = 0;
};

struct IntegerLiteral : public Expression 
{
public:
    const i64 value;

    explicit IntegerLiteral(const i64 value);

    virtual void accept(Visitor &visitor) const override;
};

struct DoubleLiteral : public Expression 
{
public:
    const double value;

    explicit DoubleLiteral(const double value);

    virtual void accept(Visitor &visitor) const override;
};

struct CharLiteral : public Expression 
{
public:
    const i32 value;

    explicit CharLiteral(const i32 value);

    virtual void accept(Visitor &visitor) const override;
};

struct BoolLiteral : public Expression
{
public:
    const bool value;

    explicit BoolLiteral(const bool value) ;

    virtual void accept(Visitor &visitor) const override;
};

struct StringLiteral : public Expression 
{
public:
    const std::string value;

    explicit StringLiteral(std::string value);

    virtual void accept(Visitor &visitor) const override;
};

using ArgumentList = std::vector<std::unique_ptr<const Expression>>;

struct ArrayLiteral : public Expression 
{
public:
    const std::unique_ptr<const ArrayType> type;
    const ArgumentList values;

    explicit ArrayLiteral(std::unique_ptr<const ArrayType> &&type, ArgumentList &&values);

    virtual void accept(Visitor &visitor) const override;
};

struct UnaryExpression : public Expression 
{
public:
    const UnaryOperatorKind operator_;
    const std::unique_ptr<const Expression> operand;

    explicit UnaryExpression(const UnaryOperatorKind operator_, std::unique_ptr<const Expression> &&operand);

    virtual void accept(Visitor &visitor) const override;
};

struct BinaryExpression : public Expression 
{
public:
    const BinaryOperatorKind operator_;
    const std::unique_ptr<const Expression> leftOperand;
    const std::unique_ptr<const Expression> rightOperand;

    explicit BinaryExpression(const BinaryOperatorKind operator_, std::unique_ptr<const Expression> &&leftOperand, std::unique_ptr<const Expression> &&rightOperand);

    virtual void accept(Visitor &visitor) const override;
};

struct VariableUse : public Expression
{
public:
    const std::string name;

    explicit VariableUse(std::string name);

    virtual void accept(Visitor &visitor) const override;
};

struct FunctionCall : public Expression
{
public:
    const std::string name;
    const ArgumentList arguments;

    explicit FunctionCall(std::string name, ArgumentList &&arguments);

    virtual void accept(Visitor &visitor) const override;
};

using StatementList = std::vector<std::unique_ptr<const Statement>>;

struct CodeBlock : public Statement 
{
public:
    const StatementList statements;

    explicit CodeBlock(StatementList &&statements);

    virtual void accept(Visitor &visitor) const override;
};

struct IfStatement : public Statement 
{
public:
    const std::unique_ptr<const Expression> condition;
    const std::unique_ptr<const CodeBlock> thenBlock;
    const std::unique_ptr<const CodeBlock> elseBlock;

    explicit IfStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&thenBlock, std::unique_ptr<const CodeBlock> &&elseBlock);

    virtual void accept(Visitor &visitor) const override;
};

struct WhileStatement : public Statement 
{
public:
    const std::unique_ptr<const Expression> condition;
    const std::unique_ptr<const CodeBlock> body;

    explicit WhileStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&body);

    virtual void accept(Visitor &visitor) const override;
};

struct VariableDeclaration : public Statement
{
public:
    const std::string name;
    const std::unique_ptr<const Type> type;
    const std::unique_ptr<const Expression> value;

    explicit VariableDeclaration(std::string name, std::unique_ptr<const Type> &&type, std::unique_ptr<const Expression> &&value);

    virtual void accept(Visitor &visitor) const override;
};

struct Parameter 
{
public:
    std::string name;
    std::unique_ptr<const Type> type;

    explicit Parameter(std::string name, std::unique_ptr<const Type> &&type);
};

using ParameterList = std::vector<Parameter>;

struct FunctionDeclaration : public Node 
{
public:
    const std::string name;
    const ParameterList parameters;
    const std::unique_ptr<const Type> returnType;
    const std::unique_ptr<const CodeBlock> body;

    FunctionDeclaration(std::string name, ParameterList &&parameters, std::unique_ptr<const Type> &&returnType, std::unique_ptr<const CodeBlock> &&body);

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
