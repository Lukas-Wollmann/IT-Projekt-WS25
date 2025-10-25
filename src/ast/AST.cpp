#include "AST.h"

Node::Node(const NodeKind kind)
    : m_Kind(kind)
{}

NodeKind Node::getNodeKind() const
{
    return this->m_Kind;
}

Type::Type(const NodeKind kind)
    : Node(kind)
{}

ValueType::ValueType(std::string typename_)
    : Type(NodeKind::ValueType)
    , typeName(std::move(typename_))
{}

PointerType::PointerType(std::unique_ptr<const Type> &&baseType)
    : Type(NodeKind::PointerType)
    , baseType(std::move(baseType))
{}

ArrayType::ArrayType(std::unique_ptr<const Type> &&elementType, const std::optional<size_t> size)
    : Type(NodeKind::ArrayType)
    , elementType(std::move(elementType))
    , size(size)
{}

FunctionType::FunctionType(ParameterTypeList &&parameters, std::unique_ptr<const Type> &&returnType)
    : Type(NodeKind::FunctionType)
    , parameters(std::move(parameters))
    , returnType(std::move(returnType))
{}

Statement::Statement(const NodeKind kind)
    : Node(kind)
{}

Expression::Expression(const NodeKind kind)
    : Statement(kind)
{}

IntegerLiteral::IntegerLiteral(const i64 value)
    : Expression(NodeKind::IntegerLiteral)
    , value(value)
{}

DoubleLiteral::DoubleLiteral(const double value)
    : Expression(NodeKind::DoubleLiteral)
    , value(value)
{}

CharLiteral::CharLiteral(const i32 value)
    : Expression(NodeKind::CharLiteral)
    , value(value)
{}

BoolLiteral::BoolLiteral(const bool value)
    : Expression(NodeKind::BoolLiteral)
    , value(value)
{}

StringLiteral::StringLiteral(std::string value)
    : Expression(NodeKind::StringLiteral)
    , value(std::move(value))
{}

ArrayLiteral::ArrayLiteral(std::unique_ptr<const ArrayType> &&type, ArgumentList &&values)
    : Expression(NodeKind::ArrayLiteral)
    , type(std::move(type))
    , values(std::move(values))
{}

UnaryExpression::UnaryExpression(const UnaryOperatorKind operator_, std::unique_ptr<const Expression> &&operand)
    : Expression(NodeKind::UnaryExpression)
    , operator_(operator_)
    , operand(std::move(operand))
{}

BinaryExpression::BinaryExpression(const BinaryOperatorKind operator_, std::unique_ptr<const Expression> &&leftOperand, std::unique_ptr<const Expression> &&rightOperand)
    : Expression(NodeKind::BinaryExpression)
    , operator_(operator_)
    , leftOperand(std::move(leftOperand))
    , rightOperand(std::move(rightOperand))
{}

VariableUse::VariableUse(std::string name)
    : Expression(NodeKind::VariableUse)
    , name(std::move(name))
{}

FunctionCall::FunctionCall(std::string name, ArgumentList &&arguments)
    : Expression(NodeKind::FunctionCall)
    , name(std::move(name))
    , arguments(std::move(arguments))
{}

CodeBlock::CodeBlock(StatementList &&statements)
    : Statement(NodeKind::CodeBlock)
    , statements(std::move(statements))
{}

IfStatement::IfStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&thenBlock, std::unique_ptr<const CodeBlock> &&elseBlock)
    : Statement(NodeKind::IfStatement)
    , condition(std::move(condition))
    , thenBlock(std::move(thenBlock))
    , elseBlock(std::move(elseBlock))
{}

WhileStatement::WhileStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&body)
    : Statement(NodeKind::WhileStatement)
    , condition(std::move(condition))
    , body(std::move(body))
{}

VariableDeclaration::VariableDeclaration(std::string name, std::unique_ptr<const Type> &&type, std::unique_ptr<const Expression> &&value)
    : Statement(NodeKind::VariableDeclaration)
    , name(std::move(name))
    , type(std::move(type))
    , value(std::move(value))
{}

Parameter::Parameter(std::string name, std::unique_ptr<const Type> &&type)
    : name(std::move(name))
    , type(std::move(type))
{}

FunctionDeclaration::FunctionDeclaration(std::string name, ParameterList &&parameters, std::unique_ptr<const Type> &&returnType, std::unique_ptr<const CodeBlock> &&body)
    : Node(NodeKind::FunctionDeclaration)
    , name(std::move(name))
    , parameters(std::move(parameters))
    , returnType(std::move(returnType))
    , body(std::move(body))
{}

void ValueType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void PointerType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void ArrayType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void FunctionType::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void IntegerLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void DoubleLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void CharLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void BoolLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void StringLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void ArrayLiteral::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void UnaryExpression::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void BinaryExpression::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void VariableUse::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void FunctionCall::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void CodeBlock::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void IfStatement::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void WhileStatement::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void VariableDeclaration::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}

void FunctionDeclaration::accept(Visitor &visitor) const
{
    visitor.visit(*this);
}
