#pragma once
#include "ast/Visitor.h"
#include "type_checker/common/TypeCheckerContext.h"

///
/// We know that every return statment does actually return the correct type.
/// This pass exists to ensure that every control flow path in a function
/// actually does return eventually.
///
struct ControlFlowPass : public ast::ConstVisitor<bool> {
private:
    TypeCheckerContext &m_Context;

public:
    ControlFlowPass(TypeCheckerContext &context);
	
    bool visit(const ast::Module &n) override;
    bool visit(const ast::IntLit &n) override;
    bool visit(const ast::FloatLit &n) override;
    bool visit(const ast::CharLit &n) override;
    bool visit(const ast::BoolLit &n) override;
    bool visit(const ast::StringLit &n) override;
    bool visit(const ast::ArrayExpr &n) override;
    bool visit(const ast::UnaryExpr &n) override;
    bool visit(const ast::BinaryExpr &n) override;
    bool visit(const ast::FuncCall &n) override;
    bool visit(const ast::VarRef &n) override;
    bool visit(const ast::BlockStmt &n) override;
    bool visit(const ast::IfStmt &n) override;
    bool visit(const ast::WhileStmt &n) override;
    bool visit(const ast::ReturnStmt &n) override;
    bool visit(const ast::VarDef &n) override;
    bool visit(const ast::FuncDecl &n) override;
};
