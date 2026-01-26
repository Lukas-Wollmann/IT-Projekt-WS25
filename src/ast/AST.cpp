#include "AST.h"

#include "Macros.h"
#include "core/U8String.h"
#include "type/Printer.h"

namespace ast {
	using namespace type;

	Node::Node(const NodeKind kind)
		: kind(kind) {}

	Stmt::Stmt(const NodeKind kind)
		: Node(kind) {}

	Expr::Expr(const NodeKind kind)
		: Stmt(kind) {}

	void Expr::infer(TypePtr type, ValueCategory category) {
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

	VarDef::VarDef(U8String ident, TypePtr type, Box<Expr> value)
		: Stmt(NodeKind::VarDef)
		, ident(std::move(ident))
		, type(std::move(type))
		, value(std::move(value)) {}

	FuncDecl::FuncDecl(U8String ident, Vec<Param> params, TypePtr returnType, Box<BlockStmt> body)
		: Node(NodeKind::FuncDecl)
		, ident(std::move(ident))
		, params(std::move(params))
		, returnType(std::move(returnType))
		, body(std::move(body)) {}

	Module::Module(U8String name, Vec<Box<FuncDecl>> decls)
		: Node(NodeKind::Module)
		, name(std::move(name))
		, decls(std::move(decls)) {}

	U8String str(NodeKind kind) {
		using enum NodeKind;

		switch (kind) {
			case IntLit:	 return u8"IntLit";
			case CharLit:	 return u8"CharLit";
			case BoolLit:	 return u8"BoolLit";
			case UnaryExpr:	 return u8"UnaryExpr";
			case BinaryExpr: return u8"BinaryExpr";
			case FuncCall:	 return u8"FuncCall";
			case VarRef:	 return u8"VarRef";
			case BlockStmt:	 return u8"BlockStmt";
			case IfStmt:	 return u8"IfStmt";
			case WhileStmt:	 return u8"WhileStmt";
			case ReturnStmt: return u8"ReturnStmt";
			case VarDef:	 return u8"VarDef";
			case FuncDecl:	 return u8"FuncDecl";
			case Module:	 return u8"Module";
			default:		 UNREACHABLE();
		}
	}

	U8String str(AssignmentKind kind) {
		using enum AssignmentKind;

		switch (kind) {
			case Simple:		 return u8"=";
			case Addition:		 return u8"+=";
			case Subtraction:	 return u8"-=";
			case Multiplication: return u8"*=";
			case Division:		 return u8"/=";
			case Modulo:		 return u8"%=";
			case BitwiseAnd:	 return u8"&=";
			case BitwiseOr:		 return u8"|=";
			case BitwiseXor:	 return u8"^=";
			case LeftShift:		 return u8"<<=";
			case RightShift:	 return u8">>=";
			default:			 UNREACHABLE();
		}
	}

	U8String str(ValueCategory cat) {
		using enum ValueCategory;

		switch (cat) {
			case LValue: return u8"LValue";
			case RValue: return u8"RValue";
			default:	 UNREACHABLE();
		}
	}
}