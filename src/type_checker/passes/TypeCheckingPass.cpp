#include "TypeCheckingPass.h"
#include <sstream>

TypeCheckingPass::TypeCheckingPass(TypeCheckerContext &context)
    : m_Context(context)
{
    m_SymbolTable.enterScope();
}

TypeCheckingPass::~TypeCheckingPass()
{
    m_SymbolTable.exitScope();  
}

void TypeCheckingPass::visit(IntLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped IntLit node");

    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
}

void TypeCheckingPass::visit(FloatLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped FloatLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));
}

void TypeCheckingPass::visit(CharLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped CharLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));
}

void TypeCheckingPass::visit(BoolLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped BoolLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool));
}

void TypeCheckingPass::visit(StringLit &node)
{
    if (node.getType().has_value())
        throw std::runtime_error("Expected untyped StringLit node");
    
    node.setType(std::make_unique<PrimitiveType>(PrimitiveTypeKind::String));
}

void TypeCheckingPass::visit(ArrayExpr &node)
{
    (void)node;
    throw std::runtime_error("Not implemented");
}

void TypeCheckingPass::visit(UnaryExpr &node)
{
    // Infert the type of the operand expression
    node.getOperand().accept(*this);
    
    const Type &type = node.getOperand().getType().value();
    
    // For now, the unary operation just yields the same result.
    // If the result has type <error-type> just keep it as well.
    node.setType(type.copy());
}

void TypeCheckingPass::visit(BinaryExpr &node)
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
    
    m_Context.addError(ss.str());
    node.setType(std::make_unique<ErrorType>());
}

void TypeCheckingPass::visit(FuncCall &node)
{
    auto func = m_Context.getGlobalNamespace().getFunction(node.getIdent());

    if (!func.has_value())
    {   
        node.setType(std::make_unique<ErrorType>());
        
        std::stringstream ss;
        ss << "Illegal call to undefined function " << node.getIdent();

        m_Context.addError(ss.str());
        return;
    }

    const Type &type = func->get().getType();

    if (type.getKind() != TypeKind::Function)
    {
        node.setType(std::make_unique<ErrorType>());
        
        std::stringstream ss;
        ss << "Illegal call to " << node.getIdent() << ": ";
        ss << " this symbol is not a function.";

        m_Context.addError(ss.str());
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

        m_Context.addError(ss.str());
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

        m_Context.addError(ss.str());
    }
}

void TypeCheckingPass::visit(VarRef &node)
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
    m_Context.addError(ss.str());

    node.setType(std::make_unique<ErrorType>());
}

void TypeCheckingPass::visit(CodeBlock &stmt)
{
    m_SymbolTable.enterScope();

    for (StmtPtr &n : stmt.getStmts())
        n->accept(*this);

    m_SymbolTable.exitScope();
}

void TypeCheckingPass::visit(IfStmt &node)
{
    node.getCond().accept(*this);
    
    const Type &type = node.getCond().getType()->get();

    // If the condition has type <error-type> fail silently
    if (type.getKind() != TypeKind::Error && type != PrimitiveType(PrimitiveTypeKind::Bool))
    {
        std::stringstream ss;
        ss << "Cannot accept type " << type << " inside an if condition.";

        m_Context.addError(ss.str());
    }
}

void TypeCheckingPass::visit(WhileStmt &node)
{
    node.getCond().accept(*this);
    
    const Type &type = node.getCond().getType()->get();

    // If the condition has type <error-type> fail silently
    if (type.getKind() != TypeKind::Error && type != PrimitiveType(PrimitiveTypeKind::Bool))
    {
        std::stringstream ss;
        ss << "Cannot accept type " << type << " inside a while condition.";

        m_Context.addError(ss.str());
    }
    
    node.getBody().accept(*this);
}

void TypeCheckingPass::visit(ReturnStmt &node)
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

    m_Context.addError(ss.str());
}

void TypeCheckingPass::visit(VarDecl &node)
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

        m_Context.addError(ss.str());   
    }

    // Does this symbol already exist in the current scope (shadowing possible)
    if (m_SymbolTable.isSymbolDefinedInCurrentScope(node.getIdent()))
    {
        std::stringstream ss;
        ss << "Illegal redefinition of symbol: " << node.getIdent();

        m_Context.addError(ss.str());
        return;
    }

    m_SymbolTable.addSymbol(node.getIdent(), SymbolInfo(node.getType().copy()));
}

void TypeCheckingPass::visit(FuncDecl &node)
{
    m_SymbolTable.enterScope();

    // Add all params as symbols to the function scope
    for (const auto &param : node.getParams())
        m_SymbolTable.addSymbol(param.first, SymbolInfo(param.second->copy()));

    // Set the current expected return type
    m_CurrentFunctionReturnType = node.getReturnType().copy();

    // Type check the function body (in a nested scope, allows param shadowing)
    node.getBody().accept(*this);

    m_SymbolTable.exitScope();

    // We already explored this function during the exploration pass, dont
    // add it to the symbol table a second time, that would break it.
}

void TypeCheckingPass::visit(Module &node)
{
    for (FuncDeclPtr &decl : node.getDeclarations())
        decl->accept(*this);
}