#include "PrinterVisitor.h"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const Node &node)
{
    PrinterVisitor printer(os);
    node.accept(printer);
    return os;
}

PrinterVisitor::PrinterVisitor(std::ostream &stream)
    : m_Stream(stream)
{}

void PrinterVisitor::visit(const ValueType &node)
{
    this->m_Stream << "ValueType(" << node.getTypename() << ")";
}

void PrinterVisitor::visit(const PointerType &node)
{
    this->m_Stream << "PointerType(" << node.getBaseType() << ")";
}

void PrinterVisitor::visit(const ArrayType &node)
{
    this->m_Stream << "ArrayType(" << node.getElementType() << ")";
    auto size = node.getSize();
    if (size)
    {
        this->m_Stream << ", " << *size;
    }
}

void PrinterVisitor::visit(const FunctionType &node)
{
    this->m_Stream << "FunctionType({";

    const ParameterTypeList &params = node.getParameters();

    for (size_t i = 0; i < params.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *params[i];
    }

    this->m_Stream << "}, " << node.getReturnType() << ")";
}

void PrinterVisitor::visit(const IntegerLiteral &node)
{
    this->m_Stream << "IntegerLiteral(" << node.getValue() << ")";
}

void PrinterVisitor::visit(const DoubleLiteral &node)
{
    this->m_Stream << "DoubleLiteral(" << node.getValue() << ")";
}

void PrinterVisitor::visit(const CharLiteral &node)
{
    this->m_Stream << "CharLiteral('" << node.getValue() << "')";
}

void PrinterVisitor::visit(const BoolLiteral &node)
{
    this->m_Stream << "BoolLiteral(" << node.getValue() << ")";
}

void PrinterVisitor::visit(const StringLiteral &node)
{
    this->m_Stream << "StringLiteral(\"" << node.getValue() << "\")";
}

void PrinterVisitor::visit(const ArrayLiteral &node)
{
    this->m_Stream << "ArrayLiteral(" << node.getType() << ", {";

    const ArgumentList &vals = node.getValues();

    for (size_t i = 0; i < vals.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *vals[i];
    }

    this->m_Stream << "})";
}

void PrinterVisitor::visit(const UnaryExpression &node)
{
    this->m_Stream << "UnaryExpression(" << node.getOperand() << ", " << node.getOperator() << ")";
}

void PrinterVisitor::visit(const BinaryExpression &node)
{
    this->m_Stream << "BinaryExpression(" << node.getLeftOperand() << ", ";
    this->m_Stream << node.getRightOperand() << ", " << node.getOperator() << ")";
}

void PrinterVisitor::visit(const FunctionCall &node)
{
    this->m_Stream << "FunctionCall(" << node.getName() << ", {";

    const ArgumentList &args = node.getArguments();

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

void PrinterVisitor::visit(const VariableUse &node)
{
    this->m_Stream << "VariableUse(" << node.name() << ")";
}

void PrinterVisitor::visit(const CodeBlock &node)
{
    this->m_Stream << "CodeBlock({";

    const StatementList &statements = node.getStatements();

    for (size_t i = 0; i < statements.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *statements[i];
    }

    this->m_Stream << "})";
}

void PrinterVisitor::visit(const IfStatement &node)
{
    this->m_Stream << "IfStatement(" << node.getCondition() << ", " << node.getThenBlock() << ", ";
    this->m_Stream << node.getElseBlock() << ")";
}

void PrinterVisitor::visit(const WhileStatement &node)
{
    this->m_Stream << "WhileStatement(" << node.getCondition() << ", " << node.getBody() << ")";
}

void PrinterVisitor::visit(const VariableDeclaration &node)
{
    this->m_Stream << "VariableDeclaration(" << node.getName() << ", " << node.getType() << ", " << node.getValue() << ")";
}

void PrinterVisitor::visit(const FunctionDeclaration &node)
{
    this->m_Stream << "FunctionDeclaration(" << node.getName() << ", {";

    const ParameterList &params = node.getParameters();
    
    for (size_t i = 0; i < params.size(); ++i)
    {
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        const Parameter &param = params[i];
        this->m_Stream << "Parameter(" << param.getName() << ", " << param.getType() << ")";
    }

    this->m_Stream << "}, " << node.getReturnType() << ", " << node.getBody() << ")";
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