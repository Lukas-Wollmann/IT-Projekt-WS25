#include "PrinterVisitor.h"
#include <format>

PrinterVisitor::PrinterVisitor(std::ostream &stream)
    : m_Stream(stream)
{}

void PrinterVisitor::visit(const ValueType &node)
{
    this->m_Stream << std::format("ValueType({})", node.m_Typename);
}

void visit(const PointerType &node) ;
void visit(const ArrayType &node) ;
void visit(const FunctionType &node) ;
void visit(const IntegerLiteral &node) ;
void visit(const DoubleLiteral &node) ;
void visit(const CharLiteral &node) ;
void visit(const BoolLiteral &node) ;
void visit(const StringLiteral &node) ;
void visit(const ArrayLiteral &node) ;
void visit(const UnaryExpression &node) ;
void visit(const BinaryExpression &node) ;
void visit(const FunctionCall &node) ;
void visit(const VariableUse &node) ;
void visit(const CodeBlock &node) ;
void visit(const IfStatement &node) ;
void visit(const WhileStatement &node) ;
void visit(const VariableDeclaration &node) ;
void visit(const Parameter &node) ;
void visit(const FunctionDeclaration &node) ;