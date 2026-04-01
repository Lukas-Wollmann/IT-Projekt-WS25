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
	NullLit,
	UnitLit,
	HeapAlloc,
	UnaryExpr,
	BinaryExpr,
	Assignment,
	VarRef,
	FieldAccess,
	FuncCall,
	BlockStmt,
	IfStmt,
	WhileStmt,
	ReturnStmt,
	VarDef,
	FuncDecl,
	Module,
	StructDecl
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
	explicit Stmt(NodeKind kind);
};

struct Expr : Stmt {
	Opt<Type> inferredType;
	Opt<ValueCategory> valueCategory;

	void infer(Type type, ValueCategory category);
	[[nodiscard]] bool isInferred() const;

protected:
	explicit Expr(NodeKind kind);
};

struct IntLit : Expr {
	const i32 value;

	explicit IntLit(i32 value);
};

struct CharLit : Expr {
	const char32_t value;

	explicit CharLit(char32_t value);
};

struct BoolLit : Expr {
	const bool value;

	explicit BoolLit(bool value);
};

struct NullLit : Expr {
	NullLit();
};

struct UnitLit : Expr {
	UnitLit();
};

struct HeapAlloc : Expr {
	const Type type;
	const Box<Expr> expr;

	HeapAlloc(Type type, Box<Expr> expr);
};

struct UnaryExpr : Expr {
	const UnaryOpKind op;
	const Box<Expr> operand;

	UnaryExpr(UnaryOpKind op, Box<Expr> operand);
};

struct BinaryExpr : Expr {
	const BinaryOpKind op;
	const Box<Expr> left, right;

	BinaryExpr(BinaryOpKind op, Box<Expr> left, Box<Expr> right);
};

struct Assignment : Expr {
	const AssignmentKind assignmentKind;
	const Box<Expr> left, right;

	Assignment(AssignmentKind assignmentKind, Box<Expr> left, Box<Expr> right);
};

struct VarRef : Expr {
	const U8String ident;

	explicit VarRef(U8String ident);
};

struct FieldAccess : Expr {
	const Box<Expr> base;
	const U8String field;

	FieldAccess(Box<Expr> base, U8String field);
};

struct FuncCall : Expr {
	const Box<Expr> expr;
	const Vec<Box<Expr>> args;

	FuncCall(Box<Expr> expr, Vec<Box<Expr>> args);
};

struct BlockStmt : Stmt {
	const Vec<Box<Stmt>> stmts;

	explicit BlockStmt(Vec<Box<Stmt>> stmts);
};

struct IfStmt : Stmt {
	const Box<Expr> cond;
	const Box<BlockStmt> then;
	const Box<BlockStmt> else_;

	IfStmt(Box<Expr> cond, Box<BlockStmt> then, Box<BlockStmt> else_);
};

struct WhileStmt : Stmt {
	const Box<Expr> cond;
	const Box<BlockStmt> body;

	WhileStmt(Box<Expr> cond, Box<BlockStmt> body);
};

struct ReturnStmt : Stmt {
	const Box<Expr> expr;

	explicit ReturnStmt(Box<Expr> expr);
};

struct VarDef : Stmt {
	const U8String ident;
	const Type type;
	const Box<Expr> value;

	VarDef(U8String ident, Type type, Box<Expr> value);
};

using Param = Pair<U8String, Type>;
using StructField = Pair<U8String, Type>;

struct FuncDecl : Node {
	const U8String ident;
	const Vec<Param> params;
	const Type returnType;
	const Box<BlockStmt> body;

	FuncDecl(U8String ident, Vec<Param> params, Type returnType, Box<BlockStmt> body);
};

struct StructDecl : Node {
	const U8String ident;
	const Vec<StructField> fields;

	StructDecl(U8String ident, Vec<StructField> fields);
};

struct Module : Node {
	const U8String name;
	const Vec<Box<FuncDecl>> funcs;
	const Vec<Box<StructDecl>> structs;

	Module(U8String name, Vec<Box<FuncDecl>> decls, Vec<Box<StructDecl>> structs);
};
}