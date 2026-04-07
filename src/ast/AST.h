#pragma once

#include "core/Operators.h"
#include "core/SourceLoc.h"
#include "core/Typedef.h"
#include "core/U8String.h"
#include "type/Type.h"

namespace ast {
enum struct NodeKind {
	IntLit,
	CharLit,
	BoolLit,
	UnitLit,
	HeapAlloc,
	UnaryExpr,
	BinaryExpr,
	Assignment,
	VarRef,
	FuncCall,
	BlockStmt,
	IfStmt,
	WhileStmt,
	ReturnStmt,
	VarDef,
	FuncDecl,
	Module
};

enum struct ValueCategory {
	LValue,
	RValue,
};

struct Node {
	const NodeKind kind;
	const SourceLoc loc;

	virtual ~Node() = default;

protected:
	explicit Node(NodeKind kind, const SourceLoc &loc = {});
};

struct Stmt : Node {
protected:
	explicit Stmt(NodeKind kind, const SourceLoc &loc = {});
};

struct Expr : Stmt {
	Opt<type::TypePtr> inferredType;
	Opt<ValueCategory> valueCategory;

	void infer(type::TypePtr type, ValueCategory category);
	[[nodiscard]] bool isInferred() const;

protected:
	explicit Expr(NodeKind kind, const SourceLoc &loc = {});
};

struct IntLit : Expr {
	const i32 value;

	IntLit(i32 value, const SourceLoc &loc);
};

struct CharLit : Expr {
	const char32_t value;

	CharLit(char32_t value, const SourceLoc &loc);
};

struct BoolLit : Expr {
	const bool value;

	BoolLit(bool value, const SourceLoc &loc);
};

struct UnitLit : Expr {
	explicit UnitLit(const SourceLoc &loc);
};

struct HeapAlloc : Expr {
	const type::TypePtr type;
	const Box<Expr> expr;

	HeapAlloc(type::TypePtr type, Box<Expr> expr, const SourceLoc &loc);
};

struct UnaryExpr : Expr {
	const UnaryOpKind op;
	const Box<Expr> operand;

	UnaryExpr(UnaryOpKind op, Box<Expr> operand, const SourceLoc &loc);
};

struct BinaryExpr : Expr {
	const BinaryOpKind op;
	const Box<Expr> left, right;

	BinaryExpr(BinaryOpKind op, Box<Expr> left, Box<Expr> right, const SourceLoc &loc);
};

struct Assignment : Expr {
	const AssignmentKind assignmentKind;
	const Box<Expr> left, right;

	Assignment(AssignmentKind assignmentKind, Box<Expr> left, Box<Expr> right,
			   const SourceLoc &loc);
};

struct VarRef : Expr {
	const U8String ident;

	VarRef(U8String ident, const SourceLoc &loc);
};

struct FuncCall : Expr {
	const Box<Expr> expr;
	const Vec<Box<Expr>> args;

	FuncCall(Box<Expr> expr, Vec<Box<Expr>> args, const SourceLoc &loc);
};

struct BlockStmt : Stmt {
	const Vec<Box<Stmt>> stmts;

	BlockStmt(Vec<Box<Stmt>> stmts, const SourceLoc &loc);
};

struct IfStmt : Stmt {
	const Box<Expr> cond;
	const Box<BlockStmt> then;
	const Box<BlockStmt> else_;

	IfStmt(Box<Expr> cond, Box<BlockStmt> then, Box<BlockStmt> else_, const SourceLoc &loc);
};

struct WhileStmt : Stmt {
	const Box<Expr> cond;
	const Box<BlockStmt> body;

	WhileStmt(Box<Expr> cond, Box<BlockStmt> body, const SourceLoc &loc);
};

struct ReturnStmt : Stmt {
	const Box<Expr> expr;

	ReturnStmt(Box<Expr> expr, const SourceLoc &loc);
};

struct VarDef : Stmt {
	const U8String ident;
	const type::TypePtr type;
	const Box<Expr> value;

	VarDef(U8String ident, type::TypePtr type, Box<Expr> value, const SourceLoc &loc);
};

using Param = Pair<U8String, type::TypePtr>;

struct FuncDecl : Node {
	const U8String ident;
	const Vec<Param> params;
	const type::TypePtr returnType;
	const Box<BlockStmt> body;

	FuncDecl(U8String ident, Vec<Param> params, type::TypePtr returnType, Box<BlockStmt> body,
			 const SourceLoc &loc);
};

struct Module : Node {
	const U8String name;
	const Vec<Box<FuncDecl>> decls;

	Module(U8String name, Vec<Box<FuncDecl>> decls, const SourceLoc &loc);
};
}