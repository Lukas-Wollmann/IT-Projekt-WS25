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
    m_Stream << "ValueType(" << node.m_Typename << ")";
}

void PrinterVisitor::visit(const PointerType &node)
{
    m_Stream << "PointerType(" << *node.m_BaseType << ")";
}

void PrinterVisitor::visit(const ArrayType &node)
{
    m_Stream << "ArrayType(" << *node.m_ElementType << ")";
}

void PrinterVisitor::visit(const FunctionType &node)
{
    this->m_Stream << "FunctionType({";

    for (size_t i = 0; i < node.m_Parameters.size(); ++i)
    {
        if (i > 0) 
        {
            m_Stream << ", ";
        }

        this->m_Stream << *node.m_Parameters[i];
    }

    this->m_Stream << "}, " << *node.m_ReturnType << ")";
}

void PrinterVisitor::visit(const IntegerLiteral &node)
{
    this->m_Stream << "IntegerLiteral(" << node.m_Value << ")";
}

void PrinterVisitor::visit(const DoubleLiteral &node)
{
    this->m_Stream << "DoubleLiteral(" << node.m_Value << ")";
}

void PrinterVisitor::visit(const CharLiteral &node)
{
    this->m_Stream << "CharLiteral(" << node.m_Value << ")";
}

void PrinterVisitor::visit(const BoolLiteral &node)
{
    this->m_Stream << "BoolLiteral(" << node.m_Value << ")";
}

void PrinterVisitor::visit(const StringLiteral &node)
{
    this->m_Stream << "StringLiteral(" << node.m_Value << ")";
}

void PrinterVisitor::visit(const ArrayLiteral &node)
{
    this->m_Stream << "ArrayLiteral(" << node.m_Type << ", {";

    for (size_t i = 0; i < node.m_Values.size(); ++i)
    {
        if (i > 0) 
        {
            this->m_Stream << ", ";
        }

        this->m_Stream << *node.m_Values[i];
    }

    this->m_Stream << "})";
}

void PrinterVisitor::visit(const UnaryExpression &node)
{
    this->m_Stream << "UnaryExpression(" << *node.m_Operand << ", " << node.m_Operator << ")";
}

void PrinterVisitor::visit(const BinaryExpression &node)
{
    this->m_Stream << "BinaryExpression(" << *node.m_LeftOperand << ", ";
    this->m_Stream << *node.m_RightOperand << ", " << node.m_Operator << ")";
}

void PrinterVisitor::visit(const FunctionCall &node)
{
    m_Stream << "FunctionCall(" << node.m_Name << ", {";

    for (size_t i = 0; i < node.m_Arguments.size(); ++i)
    {
        if (i > 0)
        {
            m_Stream << ", ";
        }

        this->m_Stream << *node.m_Arguments[i];
    }

    this->m_Stream << "})";
}

void PrinterVisitor::visit(const VariableUse &node)
{
    m_Stream << "VariableUse(" << node.m_Name << ")";
}

void PrinterVisitor::visit(const CodeBlock &node)
{
    this->m_Stream << "CodeBlock({";

    for (size_t i = 0; i < node.m_Statements.size(); ++i)
    {
        if (i > 0) 
        {
            m_Stream << ", ";
        }

        this->m_Stream << *node.m_Statements[i];
    }

    this->m_Stream << "})";
}

void PrinterVisitor::visit(const IfStatement &node)
{
    this->m_Stream << "IfStatement(" << *node.m_Condition << ", ";
    this->m_Stream << *node.m_ThenBlock << ", " << *node.m_ElseBlock << ")";
}

void PrinterVisitor::visit(const WhileStatement &node)
{
    this->m_Stream << "WhileStatement(" << *node.m_Condition << ", " << *node.m_Body << ")";
}

void PrinterVisitor::visit(const VariableDeclaration &node)
{
    this->m_Stream << "VariableDeclaration(" << node.m_Name << ", ";
    this->m_Stream << ", " << *node.m_Type << ", " << *node.m_Value << ")";
}

void PrinterVisitor::visit(const FunctionDeclaration &node)
{
    this->m_Stream << "FunctionDeclaration(" << node.m_Name << ", {";
    
    for (size_t i = 0; i < node.m_Parameters.size(); ++i)
    {
        if (i > 0) 
        {
            this->m_Stream << ", ";
        }

        const Parameter &param = node.m_Parameters[i]; 

        std::cout << "Parameter(" << param.m_Name << ", " << *param.m_Type << ")";
    }
    
    this->m_Stream << "}, " << *node.m_ReturnType << ", " << *node.m_Body << ")";
}

std::ostream &operator<<(std::ostream &os, UnaryOperatorKind op)
{
    switch (op)
    {
        case UnaryOperatorKind::LogicalNot: return os << "LogicalNot";
        case UnaryOperatorKind::BitwiseNot: return os << "BitwiseNot";
        case UnaryOperatorKind::Positive:   return os << "Positive";
        case UnaryOperatorKind::Negative:   return os << "Negative";
    }

    return os << "<UnknownUnaryOperator>";
}

std::ostream &operator<<(std::ostream &os, BinaryOperatorKind op)
{
    switch (op)
    {
        case BinaryOperatorKind::Addition:                 return os << "Addition";
        case BinaryOperatorKind::Subtraction:              return os << "Subtraction";
        case BinaryOperatorKind::Multiplication:           return os << "Multiplication";
        case BinaryOperatorKind::Division:                 return os << "Division";
        case BinaryOperatorKind::Modulo:                   return os << "Modulo";
        case BinaryOperatorKind::Equality:                 return os << "Equality";
        case BinaryOperatorKind::Inequality:               return os << "Inequality";
        case BinaryOperatorKind::LessThan:                 return os << "LessThan";
        case BinaryOperatorKind::GreaterThan:              return os << "GreaterThan";
        case BinaryOperatorKind::LessThanOrEqual:          return os << "LessThanOrEqual";
        case BinaryOperatorKind::GreaterThanOrEqual:       return os << "GreaterThanOrEqual";
        case BinaryOperatorKind::LogicalAnd:               return os << "LogicalAnd";
        case BinaryOperatorKind::LogicalOr:                return os << "LogicalOr";
        case BinaryOperatorKind::BitwiseAnd:               return os << "BitwiseAnd";
        case BinaryOperatorKind::BitwiseOr:                return os << "BitwiseOr";
        case BinaryOperatorKind::BitwiseXor:               return os << "BitwiseXor";
        case BinaryOperatorKind::LeftShift:                return os << "LeftShift";
        case BinaryOperatorKind::RightShift:               return os << "RightShift";
        case BinaryOperatorKind::Assignment:               return os << "Assignment";
        case BinaryOperatorKind::AdditionAssignment:       return os << "AdditionAssignment";
        case BinaryOperatorKind::SubtractionAssignment:    return os << "SubtractionAssignment";
        case BinaryOperatorKind::MultiplicationAssignment: return os << "MultiplicationAssignment";
        case BinaryOperatorKind::DivisionAssignment:       return os << "DivisionAssignment";
        case BinaryOperatorKind::ModuloAssignment:         return os << "ModuloAssignment";
        case BinaryOperatorKind::BitwiseAndAssignment:     return os << "BitwiseAndAssignment";
        case BinaryOperatorKind::BitwiseOrAssignment:      return os << "BitwiseOrAssignment";
        case BinaryOperatorKind::BitwiseXorAssignment:     return os << "BitwiseXorAssignment";
        case BinaryOperatorKind::LeftShiftAssignment:      return os << "LeftShiftAssignment";
        case BinaryOperatorKind::RightShiftAssignment:     return os << "RightShiftAssignment";
    }

    return os << "<UnknownBinaryOperator>";
}