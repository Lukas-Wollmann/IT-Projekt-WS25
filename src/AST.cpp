#include "AST.h"

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

PointerType::PointerType(std::unique_ptr<const Type> &&baseType) 
    : Type(NodeKind::PointerType)
    , m_BaseType(std::move(baseType))
{}

ArrayType::ArrayType(std::unique_ptr<const Type> &&elementType, const std::optional<const size_t> size)
    : Type(NodeKind::ArrayType)
    , m_ElementType(std::move(elementType))
    , m_Size(size)
{}

FunctionType::FunctionType(ParameterTypeList &&parameters, std::unique_ptr<const Type> &&returnType)
    : Type(NodeKind::FunctionType)
    , m_Parameters(std::move(parameters))
    , m_ReturnType(std::move(returnType)) 
{}

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

DoubleLiteral::DoubleLiteral(const double value)
    : Expression(NodeKind::DoubleLiteral)
    , m_Value(value) 
{}

CharLiteral::CharLiteral(const i32 value)
    : Expression(NodeKind::CharLiteral)
    , m_Value(value) 
{}

BoolLiteral::BoolLiteral(const bool value)
    : Expression(NodeKind::BoolLiteral)
    , m_Value(value)
{}

StringLiteral::StringLiteral(std::string &&value)
    : Expression(NodeKind::StringLiteral)
    , m_Value(std::move(value))
{}

ArrayLiteral::ArrayLiteral(std::unique_ptr<const ArrayType> &&type, ArgumentList &&values)
    : Expression(NodeKind::ArrayLiteral)
    , m_Type(std::move(type))
    , m_Values(std::move(values))
{}

UnaryExpression::UnaryExpression(const UnaryOperatorKind operator_, std::unique_ptr<const Expression> &&operand) 
    : Expression(NodeKind::UnaryExpression)
    , m_Operator(operator_)
    , m_Operand(std::move(operand))
{}

BinaryExpression::BinaryExpression(const BinaryOperatorKind operator_, 
    std::unique_ptr<const Expression> &&leftOperand, std::unique_ptr<const Expression> &&rightOperand) 
    : Expression(NodeKind::BinaryExpression)
    , m_Operator(operator_)
    , m_LeftOperand(std::move(leftOperand))
    , m_RightOperand(std::move(rightOperand))
{}

VariableUse::VariableUse(std::string &&name)
    : Expression(NodeKind::VariableUse)
    , m_Name(std::move(name))
{}

FunctionCall::FunctionCall(std::string &&name, ArgumentList &&arguments)
    : Expression(NodeKind::FunctionCall)
    , m_Name(std::move(name))
    , m_Arguments(std::move(arguments))
{}


CodeBlock::CodeBlock(StatementList &&statements) 
    : Statement(NodeKind::CodeBlock)
    , m_Statements(std::move(statements)) 
{}

IfStatement::IfStatement(std::unique_ptr<const Expression> &&condition, 
    std::unique_ptr<const CodeBlock> &&thenBlock, std::unique_ptr<const CodeBlock> &&elseBlock)
    : Statement(NodeKind::IfStatement)
    , m_Condition(std::move(condition))
    , m_ThenBlock(std::move(thenBlock))
    , m_ElseBlock(std::move(elseBlock)) 
{}

WhileStatement::WhileStatement(std::unique_ptr<const Expression> &&condition, std::unique_ptr<const CodeBlock> &&body)
    : Statement(NodeKind::WhileStatement)
    , m_Condition(std::move(condition))
    , m_Body(std::move(body))
{}

VariableDeclaration::VariableDeclaration(std::unique_ptr<const Type> &&type, std::unique_ptr<const Expression> &&value)
    : Statement(NodeKind::VariableDeclaration)
    , m_Type(std::move(type))
    , m_Value(std::move(value))
{}


Parameter::Parameter(std::string &&name, std::unique_ptr<const Type> &&type) 
    : m_Name(std::move(name))
    , m_Type(std::move(type)) 
{}

FunctionDeclaration::FunctionDeclaration(std::string &&name, ParameterList &&parameters, std::unique_ptr<const CodeBlock> &&body)
    : Node(NodeKind::FunctionDeclaration)
    , m_Name(std::move(name))
    , m_Parameters(std::move(parameters))
    , m_Body(std::move(body))
{}