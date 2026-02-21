#include "AST.h"

#include "core/U8String.h"

namespace ast {
Node::Node(const NodeKind kind, const SourceLoc &loc)
	: kind(kind)
	, loc(loc) {}

Stmt::Stmt(const NodeKind kind, const SourceLoc &loc)
	: Node(kind, loc) {}

Expr::Expr(const NodeKind kind, const SourceLoc &loc)
	: Stmt(kind, loc) {}

void Expr::infer(type::TypePtr type, ValueCategory category) {
	inferredType = std::move(type);
	valueCategory = category;
}

bool Expr::isInferred() const {
	return inferredType.has_value() && valueCategory.has_value();
}

IntLit::IntLit(const i32 value, const SourceLoc &loc)
	: Expr(NodeKind::IntLit, loc)
	, value(value) {}

CharLit::CharLit(const char32_t value, const SourceLoc &loc)
	: Expr(NodeKind::CharLit, loc)
	, value(value) {}

BoolLit::BoolLit(const bool value, const SourceLoc &loc)
	: Expr(NodeKind::BoolLit, loc)
	, value(value) {}

UnitLit::UnitLit(const SourceLoc &loc)
	: Expr(NodeKind::UnitLit, loc) {}

HeapAlloc::HeapAlloc(type::TypePtr type, Box<Expr> expr, const SourceLoc &loc)
	: Expr(NodeKind::HeapAlloc, loc)
	, type(std::move(type))
	, expr(std::move(expr)) {}

UnaryExpr::UnaryExpr(const UnaryOpKind op, Box<Expr> operand, const SourceLoc &loc)
	: Expr(NodeKind::UnaryExpr, loc)
	, op(op)
	, operand(std::move(operand)) {}

BinaryExpr::BinaryExpr(const BinaryOpKind op, Box<Expr> left, Box<Expr> right, const SourceLoc &loc)
	: Expr(NodeKind::BinaryExpr, loc)
	, op(op)
	, left(std::move(left))
	, right(std::move(right)) {}

Assignment::Assignment(const AssignmentKind assignmentKind, Box<Expr> left, Box<Expr> right,
					   const SourceLoc &loc)
	: Expr(NodeKind::Assignment, loc)
	, assignmentKind(assignmentKind)
	, left(std::move(left))
	, right(std::move(right)) {}

VarRef::VarRef(U8String ident, const SourceLoc &loc)
	: Expr(NodeKind::VarRef, loc)
	, ident(std::move(ident)) {}

FuncCall::FuncCall(Box<Expr> expr, Vec<Box<Expr>> args, const SourceLoc &loc)
	: Expr(NodeKind::FuncCall, loc)
	, expr(std::move(expr))
	, args(std::move(args)) {}

BlockStmt::BlockStmt(Vec<Box<Stmt>> stmts, const SourceLoc &loc)
	: Stmt(NodeKind::BlockStmt, loc)
	, stmts(std::move(stmts)) {}

IfStmt::IfStmt(Box<Expr> cond, Box<BlockStmt> then, Box<BlockStmt> else_, const SourceLoc &loc)
	: Stmt(NodeKind::IfStmt, loc)
	, cond(std::move(cond))
	, then(std::move(then))
	, else_(std::move(else_)) {}

WhileStmt::WhileStmt(Box<Expr> cond, Box<BlockStmt> body, const SourceLoc &loc)
	: Stmt(NodeKind::WhileStmt, loc)
	, cond(std::move(cond))
	, body(std::move(body)) {}

ReturnStmt::ReturnStmt(Box<Expr> expr, const SourceLoc &loc)
	: Stmt(NodeKind::ReturnStmt, loc)
	, expr(std::move(expr)) {}

VarDef::VarDef(U8String ident, type::TypePtr type, Box<Expr> value, const SourceLoc &loc)
	: Stmt(NodeKind::VarDef, loc)
	, ident(std::move(ident))
	, type(std::move(type))
	, value(std::move(value)) {}

FuncDecl::FuncDecl(U8String ident, Vec<Param> params, type::TypePtr returnType, Box<BlockStmt> body,
				   const SourceLoc &loc)
	: Node(NodeKind::FuncDecl, loc)
	, ident(std::move(ident))
	, params(std::move(params))
	, returnType(std::move(returnType))
	, body(std::move(body)) {}

Module::Module(U8String name, Vec<Box<FuncDecl>> decls, const SourceLoc &loc)
	: Node(NodeKind::Module, loc)
	, name(std::move(name))
	, decls(std::move(decls)) {}
}