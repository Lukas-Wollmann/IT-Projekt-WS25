#include "TypeCheckingPass.h"
#include "Macros.h"
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

void TypeCheckingPass::visit(ast::IntLit &n)
{
    VERIFY(!n.inferredType);

    n.inferredType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
}

void TypeCheckingPass::visit(ast::FloatLit &n)
{
    VERIFY(!n.inferredType);
    
    n.inferredType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32);
}

void TypeCheckingPass::visit(ast::CharLit &n)
{
    VERIFY(!n.inferredType);
    
    n.inferredType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char);
}

void TypeCheckingPass::visit(ast::BoolLit &n)
{
    VERIFY(!n.inferredType);
    
    n.inferredType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
}

void TypeCheckingPass::visit(ast::StringLit &n)
{
    VERIFY(!n.inferredType);
    
    n.inferredType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::String);
}

void TypeCheckingPass::visit(ast::ArrayExpr &n)
{
    size_t arraySize = n.values.size();

    n.inferredType = std::make_unique<ArrayType>(n.elementType->copy(), arraySize);

    for (auto &expr : n.values)
    {
        dispatch(*expr);
        VERIFY(expr->inferredType);

        auto &type = *expr->inferredType;

        // Missmatch between array element type and array type
        if (type->getKind() != TypeKind::Error && *type != *n.elementType)
        {
            std::stringstream ss;
            ss << "Expected type " << *n.elementType;
            ss << " for array elements but found " << *type << " instead.";

            m_Context.addError(ss.str());
        }
    }
}

void TypeCheckingPass::visit(ast::UnaryExpr &n)
{
    // Infert the type of the operand expression
    dispatch(*n.operand);
    VERIFY(n.operand->inferredType);

    // For now, the unary operation just yields the same result.
    // If the result has type <error-type> just keep it as well.
    n.inferredType = n.operand->inferredType.value()->copy();
}

void TypeCheckingPass::visit(ast::BinaryExpr &n)
{
    // Infer the type of the left and right operand expressions
    dispatch(*n.left);
    dispatch(*n.right);

    VERIFY(n.left->inferredType);
    VERIFY(n.right->inferredType);

    auto &leftType = *n.left->inferredType;
    auto &rightType = *n.right->inferredType;

    // If one of the two expressions is of type <error-type> 
    // then propagate the error upwards in the type tree.
    // The error did not really happen here, so dont add to m_Errors.
    if (leftType->getKind() == TypeKind::Error 
    || rightType->getKind() == TypeKind::Error)
    {
        n.inferredType = std::make_unique<ErrorType>();
        return;
    }
    
    // Both types are not <error-type> and they are the same.
    // (Criteria for now, this will be changed.)
    if (*leftType == *rightType)
    {
        n.inferredType = leftType->copy();
        return;
    }

    // Here an actual error happenes. Add the error and make the type of
    // the binary expression an <error-type> to mark the subtree as invalid.
    std::stringstream ss;
    ss << "Found illegal binary expression: ";
    ss << n.op << " cannot be called with ";
    ss  << *leftType << " and " << *rightType;
    
    m_Context.addError(ss.str());
    n.inferredType = std::make_unique<ErrorType>();
}

void TypeCheckingPass::visit(ast::FuncCall &n)
{
    dispatch(*n.expr);
    VERIFY(n.expr->inferredType);

    auto &type = **n.expr->inferredType;

    if (type.getKind() != TypeKind::Function)
    {
        if (type.getKind() != TypeKind::Error)
        {
            std::stringstream ss;
            ss << "Cannot call a non-function-type expression";
            m_Context.addError(ss.str());
        }

        n.inferredType = std::make_unique<ErrorType>();
        return;   
    }
    
    auto &funcType = static_cast<const FunctionType&>(type);
    auto &paramTypes = funcType.getParameterTypes();
    auto &args = n.args;
    
    n.inferredType = funcType.getReturnType().copy();

    if (args.size() != paramTypes.size())
    {
        std::stringstream ss;
        ss << "Provided wrong amount of args. Expected ";
        ss << paramTypes.size() << " arguments but got " << args.size();

        m_Context.addError(ss.str());
        return;
    }

    for (size_t i = 0; i < args.size(); ++i)
    {
        dispatch(*args[i]);
        VERIFY(args[i]->inferredType);

        auto &argType = **args[i]->inferredType;
        auto &paramType = *paramTypes[i];
        
        if (argType.getKind() == TypeKind::Error || argType == paramType)
            continue;

        std::stringstream ss;
        ss << "Cannot use type " << argType << " as parameter ";
        ss << " for an argument of type " << paramType;

        m_Context.addError(ss.str());
    }
}

void TypeCheckingPass::visit(ast::VarRef &n)
{
    auto symbol = m_SymbolTable.getSymbol(n.ident);
    
    // The symbol is known, so take its type
    if (symbol)
    {
        n.inferredType = symbol->get().getType().copy();
        return;
    }

    // If the symbol is not known, it maybe be a function
    auto func = m_Context.getGlobalNamespace().getFunction(n.ident);
    
    if (func)
    {
        n.inferredType = func->get().getType().copy();
        return;   
    }

    // The symbol is unknown, so throw an error
    std::stringstream ss;
    ss << "Undefined identifier: " << n.ident;
    m_Context.addError(ss.str());

    n.inferredType = std::make_unique<ErrorType>();
}

void TypeCheckingPass::visit(ast::BlockStmt &stmt)
{
    m_SymbolTable.enterScope();

    for (auto &s : stmt.stmts)
        dispatch(*s);

    m_SymbolTable.exitScope();
}

void TypeCheckingPass::visit(ast::IfStmt &n)
{
    dispatch(*n.cond);
    VERIFY(*n.cond->inferredType);
    
    auto &type = **n.cond->inferredType;

    // If the condition has type <error-type> fail silently
    if (type.getKind() != TypeKind::Error && type != PrimitiveType(PrimitiveTypeKind::Bool))
    {
        std::stringstream ss;
        ss << "Cannot accept type " << type << " inside an if condition.";

        m_Context.addError(ss.str());
    }

    dispatch(*n.then);
    dispatch(*n.else_);
}

void TypeCheckingPass::visit(ast::WhileStmt &n)
{
    dispatch(*n.cond);
    VERIFY(*n.cond->inferredType);

    auto &type = **n.cond->inferredType;

    // If the condition has type <error-type> fail silently
    if (type.getKind() != TypeKind::Error && type != PrimitiveType(PrimitiveTypeKind::Bool))
    {
        std::stringstream ss;
        ss << "Cannot accept type " << type << " inside a while condition.";

        m_Context.addError(ss.str());
    }
    
    dispatch(*n.body);
}

void TypeCheckingPass::visit(ast::ReturnStmt &n)
{
    VERIFY(m_CurrentFunctionReturnType);

    // TODO: Add void type and return types with no values
    VERIFY(n.expr);

    dispatch(**n.expr);
    VERIFY(n.expr->get()->inferredType);

    auto &type = **n.expr->get()->inferredType;
    
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

void TypeCheckingPass::visit(ast::VarDef &n)
{
    // Infer the type of the assigned expression
    dispatch(*n.value);
    VERIFY(n.value->inferredType);

    auto &type = **n.value->inferredType;

    // The expression is not of type <error-type> but does not match type 
    // type of the variable declaration - this is an actual error
    if (type.getKind() != TypeKind::Error && type != *n.type)
    {
        std::stringstream ss;
        ss << "Missmatchting types at variable declaration: ";
        ss << "Expected " << *n.type << " but got " << type;

        m_Context.addError(ss.str());   
    }

    // Does this symbol already exist in the current scope (shadowing possible)
    if (m_SymbolTable.isSymbolDefinedInCurrentScope(n.ident))
    {
        std::stringstream ss;
        ss << "Illegal redefinition of symbol: " << n.ident;

        m_Context.addError(ss.str());
        return;
    }

    m_SymbolTable.addSymbol(n.ident, SymbolInfo(n.type->copy()));
}

void TypeCheckingPass::visit(ast::FuncDecl &n)
{
    m_SymbolTable.enterScope();

    // Add all params as symbols to the function scope
    for (auto &param : n.params)
        m_SymbolTable.addSymbol(param.first, SymbolInfo(param.second->copy()));

    // Set the current expected return type

    m_CurrentFunctionReturnType = n.returnType->copy();

    // Type check the function body (in a nested scope, allows param shadowing)
    dispatch(*n.body);

    m_SymbolTable.exitScope();

    // We already explored this function during the exploration pass, dont
    // add it to the symbol table a second time, that would break it.
}

void TypeCheckingPass::visit(ast::Module &n)
{
    for (auto &d : n.decls)
        dispatch(*d);
}