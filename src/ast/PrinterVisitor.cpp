#include "PrinterVisitor.h"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const Node &node)
{
    PrintVisitor printer(os);
    node.accept(printer);
    return os;
}

PrintVisitor::PrintVisitor(std::ostream &stream)
    : m_Stream(stream)
{}

void PrintVisitor::visit(const ValueType &node)
{
    this->m_Stream << "ValueType(" << node.typeName << ")";
}

void PrintVisitor::visit(const PointerType &node)
{
    this->m_Stream << "PointerType(" << node.baseType << ")";
}

void PrintVisitor::visit(const ArrayType &node)
{
    this->m_Stream << "ArrayType(" << node.elementType << ")";

    if (node.size)
    {
        this->m_Stream << ", " << node.size.value();
    }
}

void PrintVisitor::visit(const FunctionType &node)
{
    this->m_Stream << "FunctionType({";

    const ParameterTypeList &params = node.parameters;

    for (size_t i = 0; i < params.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *params[i];
    }

    this->m_Stream << "}, " << node.returnType << ")";
}

void PrintVisitor::visit(const IntegerLiteral &node)
{
    this->m_Stream << "IntegerLiteral(" << node.value << ")";
}

void PrintVisitor::visit(const DoubleLiteral &node)
{
    this->m_Stream << "DoubleLiteral(" << node.value << ")";
}

void PrintVisitor::visit(const CharLiteral &node)
{
    this->m_Stream << "CharLiteral('" << node.value << "')";
}

void PrintVisitor::visit(const BoolLiteral &node)
{
    this->m_Stream << "BoolLiteral(" << node.value << ")";
}

void PrintVisitor::visit(const StringLiteral &node)
{
    this->m_Stream << "StringLiteral(\"" << node.value << "\")";
}

void PrintVisitor::visit(const ArrayLiteral &node)
{
    this->m_Stream << "ArrayLiteral(" << node.type << ", {";

    const ArgumentList &values = node.values;

    for (size_t i = 0; i < values.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *values[i];
    }

    this->m_Stream << "})";
}

void PrintVisitor::visit(const UnaryExpression &node)
{
    this->m_Stream << "UnaryExpression(" << node.operand << ", " << node.operator_ << ")";
}

void PrintVisitor::visit(const BinaryExpression &node)
{
    this->m_Stream << "BinaryExpression(" << node.leftOperand << ", ";
    this->m_Stream << node.rightOperand << ", " << node.operator_ << ")";
}

void PrintVisitor::visit(const FunctionCall &node)
{
    this->m_Stream << "FunctionCall(" << node.name << ", {";

    const ArgumentList &args = node.arguments;

    for (size_t i = 0; i < args.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *args[i];
    }

    this->m_Stream << "})";
}

void PrintVisitor::visit(const VariableUse &node)
{
    this->m_Stream << "VariableUse(" << node.name << ")";
}

void PrintVisitor::visit(const CodeBlock &node)
{
    this->m_Stream << "CodeBlock({";

    for (size_t i = 0; i < node.statements.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *node.statements[i];
    }

    this->m_Stream << "})";
}

void PrintVisitor::visit(const IfStatement &node)
{
    this->m_Stream << "IfStatement(" << node.condition << ", " << node.thenBlock << ", ";
    this->m_Stream << node.elseBlock << ")";
}

void PrintVisitor::visit(const WhileStatement &node)
{
    this->m_Stream << "WhileStatement(" << node.condition << ", " << node.body << ")";
}

void PrintVisitor::visit(const VariableDeclaration &node)
{
    this->m_Stream << "VariableDeclaration(" << node.name << ", " << node.type << ", " << node.value << ")";
}

void PrintVisitor::visit(const FunctionDeclaration &node)
{
    this->m_Stream << "FunctionDeclaration(" << node.name << ", {";

    for (size_t i = 0; i < node.parameters.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        const Parameter &param = node.parameters[i];
        this->m_Stream << "Parameter(" << param.name << ", " << param.type << ")";
    }

    this->m_Stream << "}, " << node.returnType << ", " << node.body << ")";
}

std::ostream &operator<<(std::ostream &os, UnaryOperatorKind op)
{
    switch (op)
    {
        case UnaryOperatorKind::LogicalNot: 
            return os << "LogicalNot";
        case UnaryOperatorKind::BitwiseNot: 
            return os << "BitwiseNot";
        case UnaryOperatorKind::Positive:   
            return os << "Positive";
        case UnaryOperatorKind::Negative:   
            return os << "Negative";
    }

    return os << "<UnknownUnaryOperator>";
}

std::ostream &operator<<(std::ostream &os, BinaryOperatorKind op)
{
    switch (op)
    {
        case BinaryOperatorKind::Addition:                 
            return os << "Addition";
        case BinaryOperatorKind::Subtraction:              
            return os << "Subtraction";
        case BinaryOperatorKind::Multiplication:           
            return os << "Multiplication";
        case BinaryOperatorKind::Division:                 
            return os << "Division";
        case BinaryOperatorKind::Modulo:                   
            return os << "Modulo";
        case BinaryOperatorKind::Equality:                 
            return os << "Equality";
        case BinaryOperatorKind::Inequality:               
            return os << "Inequality";
        case BinaryOperatorKind::LessThan:                 
            return os << "LessThan";
        case BinaryOperatorKind::GreaterThan:              
            return os << "GreaterThan";
        case BinaryOperatorKind::LessThanOrEqual:          
            return os << "LessThanOrEqual";
        case BinaryOperatorKind::GreaterThanOrEqual:       
            return os << "GreaterThanOrEqual";
        case BinaryOperatorKind::LogicalAnd:               
            return os << "LogicalAnd";
        case BinaryOperatorKind::LogicalOr:                
            return os << "LogicalOr";
        case BinaryOperatorKind::BitwiseAnd:               
            return os << "BitwiseAnd";
        case BinaryOperatorKind::BitwiseOr:                
            return os << "BitwiseOr";
        case BinaryOperatorKind::BitwiseXor:               
            return os << "BitwiseXor";
        case BinaryOperatorKind::LeftShift:                
            return os << "LeftShift";
        case BinaryOperatorKind::RightShift:               
            return os << "RightShift";
        case BinaryOperatorKind::Assignment:               
            return os << "Assignment";
        case BinaryOperatorKind::AdditionAssignment:       
            return os << "AdditionAssignment";
        case BinaryOperatorKind::SubtractionAssignment:    
            return os << "SubtractionAssignment";
        case BinaryOperatorKind::MultiplicationAssignment: 
            return os << "MultiplicationAssignment";
        case BinaryOperatorKind::DivisionAssignment:       
            return os << "DivisionAssignment";
        case BinaryOperatorKind::ModuloAssignment:         
            return os << "ModuloAssignment";
        case BinaryOperatorKind::BitwiseAndAssignment:     
            return os << "BitwiseAndAssignment";
        case BinaryOperatorKind::BitwiseOrAssignment:      
            return os << "BitwiseOrAssignment";
        case BinaryOperatorKind::BitwiseXorAssignment:     
            return os << "BitwiseXorAssignment";
        case BinaryOperatorKind::LeftShiftAssignment:      
            return os << "LeftShiftAssignment";
        case BinaryOperatorKind::RightShiftAssignment:     
            return os << "RightShiftAssignment";
    }

    return os << "<UnknownBinaryOperator>";
}