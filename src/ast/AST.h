#pragma once
#include <iostream>

#include "Typedef.h"
#include "core/U8String.h"
#include "type/Type.h"

namespace ast {
	enum struct NodeKind {
		IntLit,
		FloatLit,
		CharLit,
		BoolLit,
		StringLit,
		UnitLit,
		ArrayExpr,
		UnaryExpr,
		BinaryExpr,
		HeapAlloc,
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
	public:
		const NodeKind kind;

		virtual ~Node() = default;

	protected:
		explicit Node(NodeKind kind);
	};

	struct Stmt : public Node {
	protected:
		explicit Stmt(NodeKind kind);
	};

	struct Expr : public Stmt {
	public:
		Opt<type::TypePtr> inferredType;
		Opt<ValueCategory> valueCategory;

		void infer(type::TypePtr type, ValueCategory category);
		bool isInferred() const;

	protected:
		explicit Expr(NodeKind kind);
	};

	struct IntLit : public Expr {
	public:
		const i32 value;

		explicit IntLit(i32 value);
	};

	struct FloatLit : public Expr {
	public:
		const f32 value;

		explicit FloatLit(f32 value);
	};

	struct CharLit : public Expr {
	public:
		const char32_t value;

		explicit CharLit(char32_t value);
	};

	struct BoolLit : public Expr {
	public:
		const bool value;

		explicit BoolLit(bool value);
	};

	struct StringLit : public Expr {
	public:
		const U8String value;

		explicit StringLit(U8String value);
	};

	struct UnitLit : public Expr {
	public:
		UnitLit();
	};

	struct ArrayExpr : public Expr {
	public:
		const type::TypePtr elementType;
		const Vec<Box<Expr>> values;

		ArrayExpr(type::TypePtr elementType, Vec<Box<Expr>> values);
	};

	enum struct UnaryOpKind { Not, Positive, Negative, Dereference };

	struct UnaryExpr : public Expr {
	public:
		const UnaryOpKind op;
		const Box<Expr> operand;

		UnaryExpr(UnaryOpKind op, Box<Expr> operand);
	};

	enum struct BinaryOpKind {
		Addition,
		Subtraction,
		Multiplication,
		Division,
		Modulo,
		Equality,
		Inequality,
		LessThan,
		GreaterThan,
		LessThanOrEqual,
		GreaterThanOrEqual,
		LogicalAnd,
		LogicalOr,
		BitwiseAnd,
		BitwiseOr,
		BitwiseXor,
		LeftShift,
		RightShift
	};

	struct BinaryExpr : public Expr {
	public:
		const BinaryOpKind op;
		const Box<Expr> left, right;

		BinaryExpr(BinaryOpKind op, Box<Expr> left, Box<Expr> right);
	};

	enum struct AssignmentKind {
		Simple,
		Addition,
		Subtraction,
		Multiplication,
		Division,
		Modulo,
		BitwiseAnd,
		BitwiseOr,
		BitwiseXor,
		LeftShift,
		RightShift,
	};

	struct HeapAlloc : public Expr {
	public:
		const type::TypePtr type;

		explicit HeapAlloc(type::TypePtr type);
	};

	struct Assignment : public Expr {
	public:
		const AssignmentKind assignmentKind;
		const Box<Expr> left, right;

		Assignment(AssignmentKind assignmentKind, Box<Expr> left, Box<Expr> right);
	};

	struct VarRef : public Expr {
	public:
		const U8String ident;

		explicit VarRef(U8String ident);
	};

	struct FuncCall : public Expr {
	public:
		const Box<Expr> expr;
		const Vec<Box<Expr>> args;

		FuncCall(Box<Expr> expr, Vec<Box<Expr>> args);
	};

	struct BlockStmt : public Stmt {
	public:
		const Vec<Box<Stmt>> stmts;

		explicit BlockStmt(Vec<Box<Stmt>> stmts);
	};

	struct IfStmt : public Stmt {
	public:
		const Box<Expr> cond;
		const Box<BlockStmt> then, else_;

		IfStmt(Box<Expr> cond, Box<BlockStmt> then, Box<BlockStmt> else_);
	};

	struct WhileStmt : public Stmt {
	public:
		const Box<Expr> cond;
		const Box<BlockStmt> body;

		WhileStmt(Box<Expr> cond, Box<BlockStmt> body);
	};

	struct ReturnStmt : public Stmt {
	public:
		const Opt<Box<Expr>> expr;

		explicit ReturnStmt(Opt<Box<Expr>> expr = std::nullopt);
	};

	struct VarDef : public Stmt {
	public:
		const U8String ident;
		const type::TypePtr type;
		const Box<Expr> value;

	public:
		VarDef(U8String ident, type::TypePtr type, Box<Expr> value);
	};

	using Param = Pair<U8String, type::TypePtr>;

	struct FuncDecl : public Node {
	public:
		const U8String ident;
		const Vec<Param> params;
		const type::TypePtr returnType;
		const Box<BlockStmt> body;

		FuncDecl(U8String ident, Vec<Param> params, type::TypePtr returnType, Box<BlockStmt> body);
	};

	struct Module : public Node {
	public:
		const U8String name;
		const Vec<Box<FuncDecl>> decls;

		Module(U8String name, Vec<Box<FuncDecl>> decls);
	};
}

std::ostream &operator<<(std::ostream &os, ast::NodeKind kind);
std::ostream &operator<<(std::ostream &os, ast::UnaryOpKind op);
std::ostream &operator<<(std::ostream &os, ast::AssignmentKind op);
std::ostream &operator<<(std::ostream &os, ast::BinaryOpKind op);