#include "AST.h"
#include <codecvt>
#include <locale>

Node::Node(const NodeKind kind)
    : m_Kind(kind)
{}

NodeKind Node::getNodeKind() const
{
    return m_Kind;
}

Stmt::Stmt(const NodeKind kind)
    : Node(kind)
{}

Expr::Expr(const NodeKind kind, std::optional<TypePtr> type)
    : Stmt(kind)
    , m_Type(std::move(type))
{}

IntLit::IntLit(i32 value)
    : Expr(NodeKind::IntLit)
    , m_Value(value)
{}

FloatLit::FloatLit(f32 value)
    : Expr(NodeKind::FloatLit)
    , m_Value(value)
{}

CharLit::CharLit(char32_t value)
    : Expr(NodeKind::CharLit)
    , m_Value(value)
{}

BoolLit::BoolLit(bool value)
    : Expr(NodeKind::BoolLit)
    , m_Value(value)
{}

StringLit::StringLit(std::string value)
    : Expr(NodeKind::StringLit)
    , m_Value(std::move(value))
{}

ArrayExpr::ArrayExpr(TypePtr elemType, ExprList values)
    : Expr(NodeKind::ArrayExpr)
    , m_ElemType(std::move(elemType))
    , m_Values(std::move(values))
{}

UnaryExpr::UnaryExpr(UnaryOperatorKind op, ExprPtr operand)
    : Expr(NodeKind::UnaryExpr)
    , m_Op(op)
    , m_Operand(std::move(operand))
{}

BinaryExpr::BinaryExpr(BinaryOperatorKind op, ExprPtr leftOp, ExprPtr rightOp)
    : Expr(NodeKind::BinaryExpr)
    , m_Op(op)
    , m_LeftOp(std::move(leftOp))
    , m_RightOp(std::move(rightOp))
{}

VarRef::VarRef(std::string ident)
    : Expr(NodeKind::VarRef)
    , m_Ident(std::move(ident))
{}

FuncCall::FuncCall(std::string ident, ExprList args)
    : Expr(NodeKind::FuncCall)
    , m_Ident(std::move(ident))
    , m_Args(std::move(args))
{}

CodeBlock::CodeBlock(StmtList stmts)
    : Stmt(NodeKind::CodeBlock)
    , m_Stmts(std::move(stmts))
{}

IfStmt::IfStmt(ExprPtr cond, CodeBlockPtr thenBlock, CodeBlockPtr elseBlock)
    : Stmt(NodeKind::IfStmt)
    , m_Cond(std::move(cond))
    , m_Then(std::move(thenBlock))
    , m_Else(std::move(elseBlock))
{}

WhileStmt::WhileStmt(ExprPtr cond, CodeBlockPtr body)
    : Stmt(NodeKind::WhileStmt)
    , m_Cond(std::move(cond))
    , m_Body(std::move(body))
{}

ReturnStmt::ReturnStmt(ExprPtr expr)
    : Stmt(NodeKind::ReturnStmt)
    , m_Expr(std::move(expr))
{}

VarDecl::VarDecl(std::string ident, TypePtr type, ExprPtr value)
    : Stmt(NodeKind::VarDecl)
    , m_Ident(std::move(ident))
    , m_Type(std::move(type))
    , m_Value(std::move(value))
{}

Param::Param(std::string ident, TypePtr type)
    : m_Ident(std::move(ident))
    , m_Type(std::move(type))
{}

FuncDecl::FuncDecl(std::string ident, ParamList params, TypePtr returnType, CodeBlockPtr body)
    : Node(NodeKind::FuncDecl)
    , m_Ident(std::move(ident))
    , m_Params(std::move(params))
    , m_ReturnType(std::move(returnType))
    , m_Body(std::move(body))
{}

void IntLit::toString(std::ostream &os) const
{
    os << "IntLit(" << m_Value << ")";
}

void FloatLit::toString(std::ostream &os) const
{
    os << "FloatLit(" << m_Value << ")";
}

void CharLit::toString(std::ostream &os) const
{
    std::u32string u32str(1, m_Value);
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::string utf8 = conv.to_bytes(u32str);

    os << "CharLit('" << utf8 << "')";
}

void BoolLit::toString(std::ostream &os) const
{
    os << "BoolLit(" << m_Value << ")";
}

void StringLit::toString(std::ostream &os) const
{
    os << "StringLit(\"" << m_Value << "\")";
}

void ArrayExpr::toString(std::ostream &os) const
{
    os << "ArrayExpr(";
    os << *m_ElemType << ", {";

    for (size_t i = 0; i < m_Values.size(); ++i)
    {
        if (i > 0) os << ", ";
        
        os << *m_Values[i];
    }

    os << " })";
}

void UnaryExpr::toString(std::ostream &os) const
{
    os << "UnaryExpr(";
    os << *m_Operand << ", ";
    os << m_Op << ")";
}

void BinaryExpr::toString(std::ostream &os) const
{
    os << "BinaryExpr(";
    os << *m_LeftOp << ", ";
    os << *m_RightOp << ", ";
    os << m_Op << ")";
}

void VarRef::toString(std::ostream &os) const
{
    os << "VarRef(" << m_Ident << ")";
}

void FuncCall::toString(std::ostream &os) const
{
    os << "FuncCall(";
    os << m_Ident << ", {";

    for (size_t i = 0; i < m_Args.size(); ++i)
    {
        if (i > 0) os << ", ";
        
        os << *m_Args[i];
    }

    os << " })";
}

void CodeBlock::toString(std::ostream &os) const
{
    os << "CodeBlock({";
    
    for (size_t i = 0; i < m_Stmts.size(); ++i)
    {
        if (i > 0) os << ", ";
        
        os << *m_Stmts[i];
    }

    os << " })";
}

void IfStmt::toString(std::ostream &os) const
{
    os << "IfStmt(";
    os << *m_Cond << ", ";
    os << *m_Then << ", ";
    os << *m_Else << ")";
}

void WhileStmt::toString(std::ostream &os) const
{
    os << "WhileStmt(";
    os << *m_Cond << ", ";
    os << *m_Body << ")";
}

void ReturnStmt::toString(std::ostream &os) const
{
    os << "ReturnStmt(";
    os << *m_Expr << ")";
}

void VarDecl::toString(std::ostream &os) const
{
    os << "VarDecl(";
    os << m_Ident << ", ";
    os << *m_Type << ", ";
    os << *m_Value << ")";
}

void FuncDecl::toString(std::ostream &os) const
{
    os << "FuncDecl(";
    os << m_Ident << ", {";

    for (size_t i = 0; i < m_Params.size(); ++i)
    {
        if (i > 0) os << ", ";
        
        os << *m_Params[i];
    }

    os << " }, ";
    os << *m_ReturnType << ", ";
    os << *m_Body << ")";
}

std::ostream &operator<<(std::ostream &os, const Node &node)
{
    node.toString(os);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Param &param)
{
    return os << "Param(" << param.getIdent() << ", " << *param.getType() << ")";
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
    return os << "<Unknown-Unary-Operator>";
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
    return os << "<Unknown-Binary-Operator>";
}