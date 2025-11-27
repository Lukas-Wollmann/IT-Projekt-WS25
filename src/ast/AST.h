#pragma once
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "Type.h"
#include "Typedef.h"
#include "core/U8String.h"

namespace ast {
	enum struct NodeKind {
		IntLit,
		FloatLit,
		CharLit,
		BoolLit,
		StringLit,
		ArrayExpr,
		UnaryExpr,
		BinaryExpr,
		FuncCall,
		VarRef,
		BlockStmt,
		IfStmt,
		WhileStmt,
		ReturnStmt,
		VarDef,
		FuncDecl,
		Module
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
		Opt<Box<const Type>> inferredType;

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

	struct ArrayExpr : public Expr {
	public:
		const Box<const Type> elementType;
		const Vec<Box<Expr>> values;

		ArrayExpr(Box<const Type> elementType, Vec<Box<Expr>> values);
	};

	enum struct UnaryOpKind { LogicalNot, BitwiseNot, Positive, Negative };

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
		RightShift,
		Assignment,
		AdditionAssignment,
		SubtractionAssignment,
		MultiplicationAssignment,
		DivisionAssignment,
		ModuloAssignment,
		BitwiseAndAssignment,
		BitwiseOrAssignment,
		BitwiseXorAssignment,
		LeftShiftAssignment,
		RightShiftAssignment
	};

	struct BinaryExpr : public Expr {
	public:
		const BinaryOpKind op;
		const Box<Expr> left, right;

		BinaryExpr(BinaryOpKind op, Box<Expr> left, Box<Expr> right);
	};

	struct VarRef : public Expr {
	public:
		const std::string ident;

		explicit VarRef(std::string ident);
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
		const std::string ident;
		const Box<const Type> type;
		const Box<Expr> value;

	public:
		VarDef(std::string ident, Box<const Type> type, Box<Expr> value);
	};

	using Param = Pair<std::string, Box<const Type>>;

	struct FuncDecl : public Node {
	public:
		const std::string ident;
		const Vec<Param> params;
		const Box<const Type> returnType;
		const Box<BlockStmt> body;

		FuncDecl(std::string ident, Vec<Param> params, Box<const Type> returnType,
				 Box<BlockStmt> body);
	};

	struct Module : public Node {
	public:
		const std::string name;
		const Vec<Box<FuncDecl>> decls;

		Module(std::string name, Vec<Box<FuncDecl>> decls);
	};

	std::ostream &operator<<(std::ostream &os, NodeKind kind);
	std::ostream &operator<<(std::ostream &os, UnaryOpKind op);
	std::ostream &operator<<(std::ostream &os, BinaryOpKind op);
}