#include "AST.h"

#include "core/U8String.h"

namespace ast {
Node::Node(const NodeKind kind, const SourceLoc &loc)
	: kind(kind)
	, loc(loc) {}

Stmt::Stmt(const NodeKind kind)
	: Node(kind) {}

Expr::Expr(const NodeKind kind)
	: Stmt(kind) {}

void Expr::infer(type::TypePtr type, ValueCategory category) {
	inferredType = std::move(type);
	valueCategory = category;
}

bool Expr::isInferred() const {
	return inferredType.has_value() && valueCategory.has_value();
}

IntLit::IntLit(const i32 value)
	: Expr(NodeKind::IntLit)
	, value(value) {}

CharLit::CharLit(const char32_t value)
	: Expr(NodeKind::CharLit)
	, value(value) {}

BoolLit::BoolLit(const bool value)
	: Expr(NodeKind::BoolLit)
	, value(value) {}

UnitLit::UnitLit()
	: Expr(NodeKind::UnitLit) {}

UnaryExpr::UnaryExpr(const UnaryOpKind op, Box<Expr> operand)
	: Expr(NodeKind::UnaryExpr)
	, op(op)
	, operand(std::move(operand)) {}

BinaryExpr::BinaryExpr(const BinaryOpKind op, Box<Expr> left, Box<Expr> right)
	: Expr(NodeKind::BinaryExpr)
	, op(op)
	, left(std::move(left))
	, right(std::move(right)) {}

Assignment::Assignment(const AssignmentKind assignmentKind, Box<Expr> left, Box<Expr> right)
	: Expr(NodeKind::Assignment)
	, assignmentKind(assignmentKind)
	, left(std::move(left))
	, right(std::move(right)) {}

VarRef::VarRef(U8String ident)
	: Expr(NodeKind::VarRef)
	, ident(std::move(ident)) {}

FuncCall::FuncCall(Box<Expr> expr, Vec<Box<Expr>> args)
	: Expr(NodeKind::FuncCall)
	, expr(std::move(expr))
	, args(std::move(args)) {}

BlockStmt::BlockStmt(Vec<Box<Stmt>> stmts)
	: Stmt(NodeKind::BlockStmt)
	, stmts(std::move(stmts)) {}

IfStmt::IfStmt(Box<Expr> cond, Box<BlockStmt> then, Box<BlockStmt> else_)
	: Stmt(NodeKind::IfStmt)
	, cond(std::move(cond))
	, then(std::move(then))
	, else_(std::move(else_)) {}

WhileStmt::WhileStmt(Box<Expr> cond, Box<BlockStmt> body)
	: Stmt(NodeKind::WhileStmt)
	, cond(std::move(cond))
	, body(std::move(body)) {}

ReturnStmt::ReturnStmt(Box<Expr> expr)
	: Stmt(NodeKind::ReturnStmt)
	, expr(std::move(expr)) {}

VarDef::VarDef(U8String ident, type::TypePtr type, Box<Expr> value)
	: Stmt(NodeKind::VarDef)
	, ident(std::move(ident))
	, type(std::move(type))
	, value(std::move(value)) {}

FuncDecl::FuncDecl(U8String ident, Vec<Param> params, type::TypePtr returnType, Box<BlockStmt> body)
	: Node(NodeKind::FuncDecl)
	, ident(std::move(ident))
	, params(std::move(params))
	, returnType(std::move(returnType))
	, body(std::move(body)) {}

Module::Module(U8String name, Vec<Box<FuncDecl>> decls)
	: Node(NodeKind::Module)
	, name(std::move(name))
	, decls(std::move(decls)) {}
}