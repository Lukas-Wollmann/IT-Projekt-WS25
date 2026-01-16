#pragma once
#include "Typedef.h"
#include "core/Operators.h"
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
		const NodeKind kind;

		virtual ~Node() = default;

	protected:
		explicit Node(NodeKind kind);
	};

	struct Stmt : Node {
	protected:
		explicit Stmt(NodeKind kind);
	};

	struct Expr : Stmt {
		Opt<type::TypePtr> inferredType;
		Opt<ValueCategory> valueCategory;

		void infer(type::TypePtr type, ValueCategory category);
		[[nodiscard]] bool isInferred() const;

	protected:
		explicit Expr(NodeKind kind);
	};

	struct IntLit : Expr {
		const i32 value;

		explicit IntLit(i32 value);
	};

	struct FloatLit : Expr {
		const f32 value;

		explicit FloatLit(f32 value);
	};

	struct CharLit : Expr {
		const char32_t value;

		explicit CharLit(char32_t value);
	};

	struct BoolLit : Expr {
		const bool value;

		explicit BoolLit(bool value);
	};

	struct StringLit : Expr {
		const U8String value;

		explicit StringLit(U8String value);
	};

	struct UnitLit : Expr {
		UnitLit();
	};

	struct ArrayExpr : Expr {
		const type::TypePtr elementType;
		const Vec<Box<Expr>> values;

		ArrayExpr(type::TypePtr elementType, Vec<Box<Expr>> values);
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

	struct HeapAlloc : Expr {
		const Box<Expr> expr;

		explicit HeapAlloc(Box<Expr> expr);
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
		const Opt<Box<Expr>> expr;

		explicit ReturnStmt(Opt<Box<Expr>> expr = std::nullopt);
	};

	struct VarDef : Stmt {
		const U8String ident;
		const type::TypePtr type;
		const Box<Expr> value;

		VarDef(U8String ident, type::TypePtr type, Box<Expr> value);
	};

	using Param = Pair<U8String, type::TypePtr>;

	struct FuncDecl : Node {
		const U8String ident;
		const Vec<Param> params;
		const type::TypePtr returnType;
		const Box<BlockStmt> body;

		FuncDecl(U8String ident, Vec<Param> params, type::TypePtr returnType, Box<BlockStmt> body);
	};

	struct Module : Node {
		const U8String name;
		const Vec<Box<FuncDecl>> decls;

		Module(U8String name, Vec<Box<FuncDecl>> decls);
	};

	std::ostream &operator<<(std::ostream &os, NodeKind kind);
	std::ostream &operator<<(std::ostream &os, AssignmentKind kind);
	std::ostream &operator<<(std::ostream &os, ValueCategory cat);
}