#include "AST.h"

#include "Macros.h"
#include "core/U8String.h"

namespace ast {
	Node::Node(NodeKind kind)
		: kind(kind) {}

	Stmt::Stmt(NodeKind kind)
		: Node(kind) {}

	Expr::Expr(NodeKind kind)
		: Stmt(kind) {}

	IntLit::IntLit(i32 value)
		: Expr(NodeKind::IntLit)
		, value(value) {}

	FloatLit::FloatLit(f32 value)
		: Expr(NodeKind::FloatLit)
		, value(value) {}

	CharLit::CharLit(char32_t value)
		: Expr(NodeKind::CharLit)
		, value(value) {}

	BoolLit::BoolLit(bool value)
		: Expr(NodeKind::BoolLit)
		, value(value) {}

	StringLit::StringLit(U8String value)
		: Expr(NodeKind::StringLit)
		, value(std::move(value)) {}

	ArrayExpr::ArrayExpr(Box<const type::Type> elementType, Vec<Box<Expr>> values)
		: Expr(NodeKind::ArrayExpr)
		, elementType(std::move(elementType))
		, values(std::move(values)) {}

	UnaryExpr::UnaryExpr(UnaryOpKind op, Box<Expr> operand)
		: Expr(NodeKind::UnaryExpr)
		, op(op)
		, operand(std::move(operand)) {}

	BinaryExpr::BinaryExpr(BinaryOpKind op, Box<Expr> left, Box<Expr> right)
		: Expr(NodeKind::BinaryExpr)
		, op(op)
		, left(std::move(left))
		, right(std::move(right)) {}

	Assignment::Assignment(AssignmentKind assignmentKind, Box<Expr> left, Box<Expr> right)
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

	ReturnStmt::ReturnStmt(Opt<Box<Expr>> expr)
		: Stmt(NodeKind::ReturnStmt)
		, expr(std::move(expr)) {}

	VarDef::VarDef(U8String ident, Box<const type::Type> type, Box<Expr> value)
		: Stmt(NodeKind::VarDef)
		, ident(std::move(ident))
		, type(std::move(type))
		, value(std::move(value)) {}

	FuncDecl::FuncDecl(U8String ident, Vec<Param> params, Box<const type::Type> returnType,
					   Box<BlockStmt> body)
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

std::ostream &operator<<(std::ostream &os, ast::NodeKind kind) {
	using enum ast::NodeKind;

	switch (kind) {
		case IntLit:	 return os << "IntLit";
		case FloatLit:	 return os << "FloatLit";
		case CharLit:	 return os << "CharLit";
		case BoolLit:	 return os << "BoolLit";
		case StringLit:	 return os << "StringLit";
		case ArrayExpr:	 return os << "ArrayExpr";
		case UnaryExpr:	 return os << "UnaryExpr";
		case BinaryExpr: return os << "BinaryExpr";
		case FuncCall:	 return os << "FuncCall";
		case VarRef:	 return os << "VarRef";
		case BlockStmt:	 return os << "BlockStmt";
		case IfStmt:	 return os << "IfStmt";
		case WhileStmt:	 return os << "WhileStmt";
		case ReturnStmt: return os << "ReturnStmt";
		case VarDef:	 return os << "VarDef";
		case FuncDecl:	 return os << "FuncDecl";
		case Module:	 return os << "Module";
		default:		 UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, ast::UnaryOpKind op) {
	using enum ast::UnaryOpKind;

	switch (op) {
		case Not:		  return os << "!";
		case Positive:	  return os << "+";
		case Negative:	  return os << "-";
		case Dereference: return os << "*";
		default:		  UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, ast::AssignmentKind op) {
	using enum ast::AssignmentKind;

	switch (op) {
		case Simple:		 return os << "=";
		case Addition:		 return os << "+=";
		case Subtraction:	 return os << "-=";
		case Multiplication: return os << "*=";
		case Division:		 return os << "/=";
		case Modulo:		 return os << "%=";
		case BitwiseAnd:	 return os << "&=";
		case BitwiseOr:		 return os << "|=";
		case BitwiseXor:	 return os << "^=";
		case LeftShift:		 return os << "<<=";
		case RightShift:	 return os << ">>=";
		default:			 UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, ast::BinaryOpKind op) {
	using enum ast::BinaryOpKind;

	switch (op) {
		case Addition:			 return os << "+";
		case Subtraction:		 return os << "-";
		case Multiplication:	 return os << "*";
		case Division:			 return os << "/";
		case Modulo:			 return os << "%";
		case Equality:			 return os << "==";
		case Inequality:		 return os << "!=";
		case LessThan:			 return os << "<";
		case GreaterThan:		 return os << ">";
		case LessThanOrEqual:	 return os << "<=";
		case GreaterThanOrEqual: return os << ">=";
		case LogicalAnd:		 return os << "&&";
		case LogicalOr:			 return os << "||";
		case BitwiseAnd:		 return os << "&";
		case BitwiseOr:			 return os << "|";
		case BitwiseXor:		 return os << "^";
		case LeftShift:			 return os << "<<";
		case RightShift:		 return os << ">>";
		default:				 UNREACHABLE();
	}
}