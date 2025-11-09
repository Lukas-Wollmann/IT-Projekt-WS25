#include "TypeChecker.h"
#include <cassert>
#include <sstream>


TypeError::TypeError(std::string msg)
    : m_Msg(std::move(msg)) 
{}

TypeChecker::TypeChecker()
{
    m_SymbolTable.pushScope();
}

void TypeChecker::checkUnaryExpr(const UnaryExpr &expr)
{
    // Infer the type of the expression the operator is applied to
    checkExpr(expr.getOperand());
 
    const Type &type = expr.getOperand().getType().value();
    
    // For now, the unary operation just yields the same result.
    // If the result has type <error-type> just keep it as well.
    expr.setType(type.copy());
}

void TypeChecker::checkBinaryExpr(const BinaryExpr &expr)
{
    checkExpr(expr.getLeftOp());
    checkExpr(expr.getRightOp());

    const Type &leftType = expr.getLeftOp().getType().value();
    const Type &rightType = expr.getRightOp().getType().value();
    
    // If one of the two expressions is of type <error-type> 
    // then propagate the error upwards in the type tree. 
    if (leftType.getKind() == Type::Kind::Error || rightType.getKind() == Type::Kind::Error)
    {
        expr.setType(std::make_unique<ErrorType>());
        return;
    }
    
    // Both types are not <error-type> and they are the same.
    // (Criteria for now, this will be changed.)
    if (leftType == rightType)
    {
        expr.setType(leftType.copy());
        return;
    }

    // Here an actual error happenes. Add the error and make the type of
    // the binary expression an <error-type> to mark the subtree as invalid.
    std::stringstream ss;
    ss << "Found illegal binary expression: ";
    ss << expr.getOp() << " on " << leftType << " and " << rightType;
    
    
    m_Errors.push_back(TypeError(ss.str()));
    expr.setType(std::make_unique<ErrorType>());
}

void TypeChecker::checkVarRef(const VarRef &expr)
{
    auto symbol = m_SymbolTable.getSymbol(expr.getIdent());

    // The symbol is known, so take its type
    if (symbol.has_value())
    {
        expr.setType(symbol.value().get().getType().copy());
        return;
    }

    // The symbol is unknown, so throw an error
    std::stringstream ss;
    ss << "Undefined identifier: " << expr.getIdent();
    m_Errors.push_back(TypeError(ss.str()));

    expr.setType(std::make_unique<ErrorType>());
}

void TypeChecker::checkExpr(const Expr &expr)
{
    if (expr.getType().has_value())
        throw std::runtime_error("Expected untyped expression node");

    switch (expr.getNodeKind())
    {
        case NodeKind::IntLit:
            expr.setType(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
            return;
        
        case NodeKind::FloatLit:
            expr.setType(std::make_unique<PrimitiveType>(PrimitiveKind::F32));
            return;

        case NodeKind::UnaryExpr: 
            checkUnaryExpr(dynamic_cast<const UnaryExpr&>(expr));
            return;

        case NodeKind::BinaryExpr: 
            checkBinaryExpr(dynamic_cast<const BinaryExpr&>(expr));
            return;

        case NodeKind::VarRef:
            checkVarRef(dynamic_cast<const VarRef&>(expr));
            return;

        default:
            throw std::runtime_error("Encountered unimplemented Expr node.");
    }
}

void TypeChecker::checkVarDecl(const VarDecl &node)
{
    checkExpr(node.getValue());

    const Type &type = node.getValue().getType().value();

    if (type.getKind() != Type::Kind::Error && type != node.getType())
    {
        std::stringstream ss;
        ss << "Missmatchting types at variable declaration: ";
        ss << "Expected " << node.getType() << " but got " << type;
        m_Errors.push_back(TypeError(ss.str()));   
    }

    if (m_SymbolTable.hasSymbolInCurrentScope(node.getIdent()))
    {
        std::stringstream ss;
        ss << "Illegal redefinition of symbol: " << node.getIdent();
        m_Errors.push_back(TypeError(ss.str()));

        return;
    }

    m_SymbolTable.addSymbol(node.getIdent(), SymbolInfo(node.getType().copy()));
}

void TypeChecker::checkCodeBlock(const CodeBlock &stmt)
{
    m_SymbolTable.pushScope();

    for (const StmtPtr &n : stmt.getStmts())
        checkStmt(*n);

    m_SymbolTable.popScope();
}

void TypeChecker::checkStmt(const Stmt &stmt)
{
    if (auto expr = dynamic_cast<const Expr*>(&stmt))
    {
        checkExpr(*expr);
        return;
    }
    
    switch (stmt.getNodeKind())
    {
        case NodeKind::VarDecl:
            checkVarDecl(dynamic_cast<const VarDecl&>(stmt));
            return;
        
        case NodeKind::CodeBlock:
            checkCodeBlock(dynamic_cast<const CodeBlock&>(stmt));
            return;

        default:
            throw std::runtime_error("Encountered unimplemented Stmt node");
    }
}