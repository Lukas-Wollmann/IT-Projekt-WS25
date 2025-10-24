#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <functional> // for reference_wrapper
#include "typedef.h"

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

    NodeKind getNodeKind() const { return m_Kind; }

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
private:
    const std::string m_Typename;

public:
    explicit ValueType(std::string typename_);

    const std::string &getTypename() const { return m_Typename; }

    virtual void accept(Visitor &visitor) const override;
};

struct PointerType : public Type 
{
private:
    const std::unique_ptr<const Type> m_BaseType;

public:
    explicit PointerType(std::unique_ptr<const Type> &&baseType);

    const Type &getBaseType() const { return *m_BaseType; }

    virtual void accept(Visitor &visitor) const override;
};

struct ArrayType : public Type
{
private:
    const std::unique_ptr<const Type> m_ElementType;
    const std::optional<const size_t> m_Size;

public:
    explicit ArrayType(std::unique_ptr<const Type> &&elementType, const std::optional<const size_t> size);

    const Type &getElementType() const { return *m_ElementType; }
    std::optional<size_t> getSize() const { return m_Size; }

    virtual void accept(Visitor &visitor) const override;
};

using ParameterTypeList = std::vector<std::unique_ptr<const Type>>;

struct FunctionType : public Type 
{
private:    
    const ParameterTypeList m_Parameters;
    const std::unique_ptr<const Type> m_ReturnType;

public:
    explicit FunctionType(ParameterTypeList &&parameters, std::unique_ptr<const Type> &&returnType);

    const ParameterTypeList &getParameters() const { return m_Parameters; }
    const Type &getReturnType() const { return *m_ReturnType; }

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
private:
    const i64 m_Value;

public:
    explicit IntegerLiteral(const i64 value);

    i64 getValue() const { return m_Value; }

    virtual void accept(Visitor &visitor) const override;
};

struct DoubleLiteral : public Expression 
{
private:
    const double m_Value;

public:
    explicit DoubleLiteral(const double value);

    double getValue() const { return m_Value; }

    virtual void accept(Visitor &visitor) const override;
};

struct CharLiteral : public Expression 
{
private:
    const i32 m_Value;

public:
    explicit CharLiteral(const i32 value);

    i32 getValue() const { return m_Value; }

    virtual void accept(Visitor &visitor) const override;
};

struct BoolLiteral : public Expression
{
private:
    const bool m_Value;

public:
    explicit BoolLiteral(const bool value) ;

    bool getValue() const { return m_Value; }

    virtual void accept(Visitor &visitor) const override;
};

struct StringLiteral : public Expression 
{
private:
    const std::string m_Value;

public:
    explicit StringLiteral(std::string value);

    const std::string &getValue() const { return m_Value; }

    virtual void accept(Visitor &visitor) const override;
};

using ArgumentList = std::vector<std::unique_ptr<const Expression>>;

struct ArrayLiteral : public Expression 
{
private:
    const std::unique_ptr<const ArrayType> m_Type;
    const ArgumentList m_Values;

public:
    explicit ArrayLiteral(std::unique_ptr<const ArrayType> &&type, ArgumentList &&values);

    const ArrayType &getType() const { return *m_Type; }
    const ArgumentList &getValues() const { return m_Values; }

    virtual void accept(Visitor &visitor) const override;
};

struct UnaryExpression : public Expression 
{
private:
    const UnaryOperatorKind m_Operator;
    const std::unique_ptr<const Expression> m_Operand;

public:
    explicit UnaryExpression(const UnaryOperatorKind operator_, std::unique_ptr<const Expression> &&operand);

    UnaryOperatorKind getOperator() const { return m_Operator; }
    const Expression &getOperand() const { return *m_Operand; }

    virtual void accept(Visitor &visitor) const override;
};

struct BinaryExpression : public Expression 
{
private:
    const BinaryOperatorKind m_Operator;
    const std::unique_ptr<const Expression> m_LeftOperand;
    const std::unique_ptr<const Expression> m_RightOperand;

public:
    explicit BinaryExpression(const BinaryOperatorKind operator_, std::unique_ptr<const Expression> &&leftOperand, std::unique_ptr<const Expression> &&rightOperand);

    BinaryOperatorKind getOperator() const { return m_Operator; }
    const Expression &getLeftOperand() const { return *m_LeftOperand; }
    const Expression &getRightOperand() const { return *m_RightOperand; }

    virtual void accept(Visitor &visitor) const override;
};

struct VariableUse : public Expression
{
private:
    const std::string m_Name;

public:
    explicit VariableUse(std::string name);

    const std::string &name() const { return m_Name; }

    virtual void accept(Visitor &visitor) const override;
};

struct FunctionCall : public Expression
{
private:
    const std::string m_Name;
    const ArgumentList m_Arguments;

public:
    explicit FunctionCall(std::string name, ArgumentList &&arguments);

    const std::string &getName() const { return m_Name; }
    const ArgumentList &getArguments() const { return m_Arguments; }

    virtual void accept(Visitor &visitor) const override;
};

using StatementList = std::vector<std::unique_ptr<const Statement>>;

struct CodeBlock : public Statement 
{
private:
    const StatementList m_Statements;

public:
    explicit CodeBlock(StatementList &&statements);

    const StatementList &getStatements() const { return m_Statements; }

    virtual void accept(Visitor &visitor) const override;
};

struct IfStatement : public Statement 
{
private:
    const std::unique_ptr<const Expression> m_Condition;
    const std::unique_ptr<const CodeBlock> m_ThenBlock;
    const std::unique_ptr<const CodeBlock> m_ElseBlock;

public:
    explicit IfStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&thenBlock, std::unique_ptr<const CodeBlock> &&elseBlock);

    const Expression &getCondition() const { return *m_Condition; }
    const CodeBlock &getThenBlock() const { return *m_ThenBlock; }
    const CodeBlock &getElseBlock() const { return *m_ElseBlock; }

    virtual void accept(Visitor &visitor) const override;
};

struct WhileStatement : public Statement 
{
private:
    const std::unique_ptr<const Expression> m_Condition;
    const std::unique_ptr<const CodeBlock> m_Body;

public:
    explicit WhileStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&body);

    const Expression &getCondition() const { return *m_Condition; }
    const CodeBlock &getBody() const { return *m_Body; }

    virtual void accept(Visitor &visitor) const override;
};

struct VariableDeclaration : public Statement
{
private:
    const std::string m_Name;
    const std::unique_ptr<const Type> m_Type;
    const std::unique_ptr<const Expression> m_Value;

public:
    explicit VariableDeclaration(std::string name, std::unique_ptr<const Type> &&type, std::unique_ptr<const Expression> &&value);

    const std::string &getName() const { return m_Name; }
    const Type &getType() const  { return *m_Type; }
    const Expression &getValue() const { return *m_Value; }

    virtual void accept(Visitor &visitor) const override;
};

struct Parameter 
{
private:
    const std::string m_Name;
    const std::unique_ptr<const Type> m_Type;

public:
    explicit Parameter(std::string name, std::unique_ptr<const Type> &&type);

    const std::string &getName() const { return m_Name; }
    const Type &getType() const { return *m_Type; }
};

using ParameterList = std::vector<Parameter>;

struct FunctionDeclaration : public Node 
{
private:
    const std::string m_Name;
    const ParameterList m_Parameters;
    const std::unique_ptr<const Type> m_ReturnType;
    const std::unique_ptr<const CodeBlock> m_Body;

public:
    FunctionDeclaration(std::string name, ParameterList &&parameters, std::unique_ptr<const Type> &&returnType, std::unique_ptr<const CodeBlock> &&body);

    const std::string &getName() const { return m_Name; }
    const ParameterList &getParameters() const { return m_Parameters; }
    const Type &getReturnType() const { return *m_ReturnType; }
    const CodeBlock &getBody() const { return *m_Body; }

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
