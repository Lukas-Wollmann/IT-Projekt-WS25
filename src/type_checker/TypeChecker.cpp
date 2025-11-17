#include "TypeChecker.h"
#include <cassert>
#include <sstream>

std::ostream &operator<<(std::ostream &os, const TypeError &err)
{
    os << "Error at " << err.m_Loc.line << ":" << err.m_Loc.column;
    return os << ": '" << err.m_Msg << "'";
}

TypeError::TypeError(std::string msg)
    : m_Msg(std::move(msg))
    , m_Loc{0, 0, 0}
{}

TypeChecker::TypeChecker()
{
    m_SymbolTable.pushScope();
}

TypeChecker::~TypeChecker()
{
    m_SymbolTable.popScope();
}

void TypeChecker::visit(IntLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped IntLit node");

    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
}

void TypeChecker::visit(FloatLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped FloatLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));
}

void TypeChecker::visit(CharLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped CharLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));
}

void TypeChecker::visit(BoolLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped BoolLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool));
}

void TypeChecker::visit(StringLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped StringLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::String));
}

void TypeChecker::visit(ArrayExpr &node)
{
    (void)node;
    throw std::runtime_error("Not implemented");
}

void TypeChecker::visit(UnaryExpr &node)
{
    // Infert the type of the operand expression
    node.getOperand().accept(*this);
    
    const Type &type = node.getOperand().getType().value();
    
    // For now, the unary operation just yields the same result.
    // If the result has type <error-type> just keep it as well.
    node.setType(type.copy());
}

void TypeChecker::visit(BinaryExpr &node)
{
    // Infer the type of the left and right operand expressions
    node.getLeftOp().accept(*this);
    node.getRightOp().accept(*this);

    const Type &leftType = node.getLeftOp().getType().value();
    const Type &rightType = node.getRightOp().getType().value();

    // If one of the two expressions is of type <error-type> 
    // then propagate the error upwards in the type tree.
    // The error did not really happen here, so dont add to m_Errors.
    if (leftType.getKind() == TypeKind::Error 
    || rightType.getKind() == TypeKind::Error)
    {
        node.setType(std::make_unique<ErrorType>());
        return;
    }
    
    // Both types are not <error-type> and they are the same.
    // (Criteria for now, this will be changed.)
    if (leftType == rightType)
    {
        node.setType(leftType.copy());
        return;
    }

    // Here an actual error happenes. Add the error and make the type of
    // the binary expression an <error-type> to mark the subtree as invalid.
    std::stringstream ss;
    ss << "Found illegal binary expression: ";
    ss << node.getOp() << " cannot be called with ";
    ss  << leftType << " and " << rightType;
    
    m_Errors.push_back(TypeError(ss.str()));
    node.setType(std::make_unique<ErrorType>());
}

void TypeChecker::visit(FuncCall &node)
{
    auto symbol = m_SymbolTable.getSymbol(node.getIdent());

    if (!symbol.has_value())
    {   
        node.setType(std::make_unique<ErrorType>());
        
        std::stringstream ss;
        ss << "Illegal call to undefined function " << node.getIdent();

        m_Errors.push_back(TypeError(ss.str()));
        return;
    }

    const Type &type = symbol->get().getType();

    if (type.getKind() != TypeKind::Function)
    {
        node.setType(std::make_unique<ErrorType>());
        
        std::stringstream ss;
        ss << "Illegal call to " << node.getIdent() << ": ";
        ss << " this symbol is not a function.";

        m_Errors.push_back(TypeError(ss.str()));
        return;
    }

    const FunctionType &funcType = static_cast<const FunctionType&>(type);
    const TypeList &paramTypes = funcType.getParameterTypes();
    ExprList &args = node.getArgs();
    
    node.setType(funcType.getReturnType().copy());

    if (args.size() != paramTypes.size())
    {
        std::stringstream ss;
        ss << "Illegal call to " << node.getIdent() << ": expected ";
        ss << paramTypes.size() << " arguments but got " << args.size();

        m_Errors.push_back(TypeError(ss.str()));
        return;
    }

    for (size_t i = 0; i < args.size(); ++i)
    {
        args[i]->accept(*this);

        const Type &argType = args[i]->getType()->get();
        const Type &paramType = *paramTypes[i];
        
        if (argType.getKind() == TypeKind::Error || argType == paramType)
            continue;

        std::stringstream ss;
        ss << "Cannot use type " << argType << " as parameter ";
        ss << " for an argument of type " << paramType;

        m_Errors.push_back(TypeError(ss.str()));
    }
}

void TypeChecker::visit(VarRef &node)
{
    auto symbol = m_SymbolTable.getSymbol(node.getIdent());

    // The symbol is known, so take its type
    if (symbol.has_value())
    {
        node.setType(symbol->get().getType().copy());
        return;
    }

    // The symbol is unknown, so throw an error
    std::stringstream ss;
    ss << "Undefined identifier: " << node.getIdent();
    m_Errors.push_back(TypeError(ss.str()));

    node.setType(std::make_unique<ErrorType>());
}

void TypeChecker::visit(CodeBlock &stmt)
{
    m_SymbolTable.pushScope();

    for (StmtPtr &n : stmt.getStmts())
        n->accept(*this);

    m_SymbolTable.popScope();
}

void TypeChecker::visit(IfStmt &node)
{
    node.getCond().accept(*this);
    
    const Type &type = node.getCond().getType()->get();

    // If the condition has type <error-type> fail silently
    if (type.getKind() != TypeKind::Error && type != PrimitiveType(PrimitiveTypeKind::Bool))
    {
        std::stringstream ss;
        ss << "Cannot accept type " << type << " inside an if condition.";

        m_Errors.push_back(TypeError(ss.str()));
    }
}

void TypeChecker::visit(WhileStmt &node)
{
    node.getCond().accept(*this);
    
    const Type &type = node.getCond().getType()->get();

    // If the condition has type <error-type> fail silently
    if (type.getKind() != TypeKind::Error && type != PrimitiveType(PrimitiveTypeKind::Bool))
    {
        std::stringstream ss;
        ss << "Cannot accept type " << type << " inside a while condition.";

        m_Errors.push_back(TypeError(ss.str()));
    }
    
    node.getBody().accept(*this);
}

void TypeChecker::visit(ReturnStmt &node)
{
    // Type check the return expression
    node.getExpr().accept(*this);

    // Get the type of the return value
    const Type &type = node.getExpr().getType()->get();

    if (!m_CurrentFunctionReturnType)
        throw std::runtime_error("Illegal ReturnStmt: m_CurrentFunctionReturnType is nullptr");

    // If the type is <error-type> or if the return type matches
    // the function declaration, its okay and return
    if (type.getKind() == TypeKind::Error || type == *m_CurrentFunctionReturnType)
        return;

    // The return type is not matching the function declaration
    std::stringstream ss;
    ss << "The type " << type << " does not match function declaration. ";
    ss << "Expected type: " << *m_CurrentFunctionReturnType;

    m_Errors.push_back(TypeError(ss.str()));
}

void TypeChecker::visit(VarDecl &node)
{
    // Infer the type of the assigned expression
    node.getValue().accept(*this);

    const Type &type = node.getValue().getType().value();

    // The expression is not of type <error-type> but does not match type 
    // type of the variable declaration - this is an actual error
    if (type.getKind() != TypeKind::Error && type != node.getType())
    {
        std::stringstream ss;
        ss << "Missmatchting types at variable declaration: ";
        ss << "Expected " << node.getType() << " but got " << type;

        m_Errors.push_back(TypeError(ss.str()));   
    }

    // Does this symbol already exist in the current scope (shadowing possible)
    if (m_SymbolTable.hasSymbolInCurrentScope(node.getIdent()))
    {
        std::stringstream ss;
        ss << "Illegal redefinition of symbol: " << node.getIdent();

        m_Errors.push_back(TypeError(ss.str()));
        return;
    }

    m_SymbolTable.addSymbol(node.getIdent(), SymbolInfo(node.getType().copy()));
}

void TypeChecker::visit(FuncDecl &node)
{
    m_SymbolTable.pushScope();

    // Add all params as symbols to the function scope
    for (const auto &param : node.getParams())
        m_SymbolTable.addSymbol(param.first, SymbolInfo(param.second->copy()));

    // Set the current expected return type
    m_CurrentFunctionReturnType = node.getReturnType().copy();

    // Type check the function body (in a nested scope, allows param shadowing)
    node.getBody().accept(*this);

    m_SymbolTable.popScope();

    // Now we have to add this function to the scope
    TypeList paramTypes;

    for (const Param &param : node.getParams())
        paramTypes.push_back(param.second->copy());
    

    auto funcType = std::make_unique<FunctionType>(
        std::move(paramTypes), node.getReturnType().copy()
    );
    
    m_SymbolTable.addSymbol(node.getIdent(), SymbolInfo(std::move(funcType)));
}