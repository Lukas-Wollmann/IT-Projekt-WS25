#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(std::ostream &stream)
    : m_Stream(stream)
{}

void CodeGenVisitor::visit(const ValueType &node)
{
    this->m_Stream << node.typeName;
}

void CodeGenVisitor::visit(const PointerType &node) {}

void CodeGenVisitor::visit(const ArrayType &node) {}

void CodeGenVisitor::visit(const FunctionType &node) {}

void CodeGenVisitor::visit(const IntegerLiteral &node) 
{
    this->m_Stream << node.value;
}

void CodeGenVisitor::visit(const DoubleLiteral &node) {}

void CodeGenVisitor::visit(const CharLiteral &node) {}

void CodeGenVisitor::visit(const BoolLiteral &node) {}

void CodeGenVisitor::visit(const StringLiteral &node) {}

void CodeGenVisitor::visit(const ArrayLiteral &node) {}

void CodeGenVisitor::visit(const UnaryExpression &node) {}

void CodeGenVisitor::visit(const BinaryExpression &node) {}

void CodeGenVisitor::visit(const FunctionCall &node) {}

void CodeGenVisitor::visit(const VariableUse &node) {}

void CodeGenVisitor::visit(const CodeBlock &node) {}

void CodeGenVisitor::visit(const IfStatement &node) {}

void CodeGenVisitor::visit(const WhileStatement &node) {}

void CodeGenVisitor::visit(const ReturnStatement &node) 
{
    this->m_Stream << "ret i32 ";

    node.expression->accept(*this);  
}

void CodeGenVisitor::visit(const VariableDeclaration &node) {}

void CodeGenVisitor::visit(const FunctionDeclaration &node)
{
    this->m_Stream << "define ";

    node.returnType->accept(*this);
    
    this->m_Stream << " @" << node.name << "(";

    for (size_t i = 0; i < node.parameters.size(); ++i) 
    {
        const Parameter &param = node.parameters[i];
        
        if (i > 0)
        {
            this->m_Stream << ", ";
        }

        param.type->accept(*this);
        
        this->m_Stream << " %" << param.name;
    }

    this->m_Stream << ") {\n";

    for (const std::unique_ptr<const Statement> &statement : node.body->statements)
    {
        this->m_Stream << "\t";

        statement->accept(*this);

        this->m_Stream << "\n";
    }

    this->m_Stream << "}";
}