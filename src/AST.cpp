#include "AST.h"
#include <format>

Node::Node(const NodeKind kind) 
    : m_Kind(kind) 
{}

Type::Type(const NodeKind kind) 
    : Node(kind) 
{}

ValueType::ValueType(std::string &&typename_) 
    : Type(NodeKind::ValueType)
    , m_Typename(std::move(typename_)) 
{}

void ValueType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

PointerType::PointerType(std::unique_ptr<const Type> &&baseType) 
    : Type(NodeKind::PointerType)
    , m_BaseType(std::move(baseType))
{}

void PointerType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

ArrayType::ArrayType(std::unique_ptr<const Type> &&elementType, const std::optional<const size_t> size)
    : Type(NodeKind::ArrayType)
    , m_ElementType(std::move(elementType))
    , m_Size(size)
{}

void ArrayType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

FunctionType::FunctionType(ParameterTypeList &&parameters, std::unique_ptr<const Type> &&returnType)
    : Type(NodeKind::FunctionType)
    , m_Parameters(std::move(parameters))
    , m_ReturnType(std::move(returnType)) 
{}

void FunctionType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

Statement::Statement(const NodeKind kind) 
    : Node(kind) 
{}

Expression::Expression(const NodeKind kind) 
    : Statement(kind) 
{}

IntegerLiteral::IntegerLiteral(const i64 value)
    : Expression(NodeKind::IntegerLiteral)
    , m_Value(value) 
{}

void IntegerLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

DoubleLiteral::DoubleLiteral(const double value)
    : Expression(NodeKind::DoubleLiteral)
    , m_Value(value) 
{}

void DoubleLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

CharLiteral::CharLiteral(const i32 value)
    : Expression(NodeKind::CharLiteral)
    , m_Value(value) 
{}

void CharLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

BoolLiteral::BoolLiteral(const bool value)
    : Expression(NodeKind::BoolLiteral)
    , m_Value(value)
{}

void BoolLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

StringLiteral::StringLiteral(std::string &&value)
    : Expression(NodeKind::StringLiteral)
    , m_Value(std::move(value))
{}

void StringLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

ArrayLiteral::ArrayLiteral(std::unique_ptr<const ArrayType> &&type, ArgumentList &&values)
    : Expression(NodeKind::ArrayLiteral)
    , m_Type(std::move(type))
    , m_Values(std::move(values))
{}

void ArrayLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

UnaryExpression::UnaryExpression(const UnaryOperatorKind operator_, std::unique_ptr<const Expression> &&operand) 
    : Expression(NodeKind::UnaryExpression)
    , m_Operator(operator_)
    , m_Operand(std::move(operand))
{}

void UnaryExpression::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

BinaryExpression::BinaryExpression(const BinaryOperatorKind operator_, 
    std::unique_ptr<const Expression> &&leftOperand, std::unique_ptr<const Expression> &&rightOperand) 
    : Expression(NodeKind::BinaryExpression)
    , m_Operator(operator_)
    , m_LeftOperand(std::move(leftOperand))
    , m_RightOperand(std::move(rightOperand))
{}

void BinaryExpression::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

VariableUse::VariableUse(std::string &&name)
    : Expression(NodeKind::VariableUse)
    , m_Name(std::move(name))
{}

void VariableUse::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

FunctionCall::FunctionCall(std::string &&name, ArgumentList &&arguments)
    : Expression(NodeKind::FunctionCall)
    , m_Name(std::move(name))
    , m_Arguments(std::move(arguments))
{}

void FunctionCall::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

CodeBlock::CodeBlock(StatementList &&statements) 
    : Statement(NodeKind::CodeBlock)
    , m_Statements(std::move(statements)) 
{}

void CodeBlock::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

IfStatement::IfStatement(std::unique_ptr<const Expression> &&condition, 
    std::unique_ptr<const CodeBlock> &&thenBlock, std::unique_ptr<const CodeBlock> &&elseBlock)
    : Statement(NodeKind::IfStatement)
    , m_Condition(std::move(condition))
    , m_ThenBlock(std::move(thenBlock))
    , m_ElseBlock(std::move(elseBlock)) 
{}

void IfStatement::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

WhileStatement::WhileStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&body)
    : Statement(NodeKind::WhileStatement)
    , m_Condition(std::move(condition))
    , m_Body(std::move(body))
{}

void WhileStatement::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

VariableDeclaration::VariableDeclaration(std::string &&name, std::unique_ptr<const Type> &&type, std::unique_ptr<const Expression> &&value)
    : Statement(NodeKind::VariableDeclaration)
    , m_Name(std::move(name))
    , m_Type(std::move(type))
    , m_Value(std::move(value))
{}

void VariableDeclaration::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

Parameter::Parameter(std::string &&name, std::unique_ptr<const Type> &&type) 
    : m_Name(std::move(name))
    , m_Type(std::move(type)) 
{}

FunctionDeclaration::FunctionDeclaration(std::string &&name, ParameterList &&parameters, std::unique_ptr<const Type> &&returnType, std::unique_ptr<const CodeBlock> &&body)
    : Node(NodeKind::FunctionDeclaration)
    , m_Name(std::move(name))
    , m_Parameters(std::move(parameters))
    , m_ReturnType(std::move(returnType))
    , m_Body(std::move(body))
{}

void FunctionDeclaration::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}