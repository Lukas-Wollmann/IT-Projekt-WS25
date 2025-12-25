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

	Instantiation::Instantiation(Box<type::Type>, Vec<Box<Expr>> args)
		: Expr(NodeKind::Instantiation)
		, type(std::move(type))
		, args(std::move(args)) {}

	BlockStmt::BlockStmt(Vec<Box<Stmt>> stmts)
		: Stmt(NodeKind::BlockStmt)
		, stmts(std::move(stmts)) {}

	IfStmt::IfStmt(Box<Expr> cond, Box<BlockStmt> then, Opt<Box<BlockStmt>> elseBlock)
		: Stmt(NodeKind::IfStmt)
		, cond(std::move(cond))
		, then(std::move(then))
		, elseBlock(std::move(elseBlock))
		, elseIfBlock(std::nullopt) {}

	IfStmt::IfStmt(Box<Expr> cond, Box<BlockStmt> then, Opt<Box<IfStmt>> elseIfBlock)
		: Stmt(NodeKind::IfStmt)
		, cond(std::move(cond))
		, then(std::move(then))
		, elseBlock(std::nullopt)
		, elseIfBlock(std::move(elseIfBlock)) {}

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
		case LogicalNot: return os << "LogicalNot";
		case BitwiseNot: return os << "BitwiseNot";
		case Positive:	 return os << "Positive";
		case Negative:	 return os << "Negative";
		default:		 UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, ast::AssignmentKind op) {
	using enum ast::AssignmentKind;

	switch (op) {
		case Simple:		 return os << "Assignment";
		case Addition:		 return os << "AdditionAssignment";
		case Subtraction:	 return os << "SubtractionAssignment";
		case Multiplication: return os << "MultiplicationAssignment";
		case Division:		 return os << "DivisionAssignment";
		case Modulo:		 return os << "ModuloAssignment";
		case BitwiseAnd:	 return os << "BitwiseAndAssignment";
		case BitwiseOr:		 return os << "BitwiseOrAssignment";
		case BitwiseXor:	 return os << "BitwiseXorAssignment";
		case LeftShift:		 return os << "LeftShiftAssignment";
		case RightShift:	 return os << "RightShiftAssignment";
		default:			 UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, ast::BinaryOpKind op) {
	using enum ast::BinaryOpKind;

	switch (op) {
		case Addition:			 return os << "Addition";
		case Subtraction:		 return os << "Subtraction";
		case Multiplication:	 return os << "Multiplication";
		case Division:			 return os << "Division";
		case Modulo:			 return os << "Modulo";
		case Equality:			 return os << "Equality";
		case Inequality:		 return os << "Inequality";
		case LessThan:			 return os << "LessThan";
		case GreaterThan:		 return os << "GreaterThan";
		case LessThanOrEqual:	 return os << "LessThanOrEqual";
		case GreaterThanOrEqual: return os << "GreaterThanOrEqual";
		case LogicalAnd:		 return os << "LogicalAnd";
		case LogicalOr:			 return os << "LogicalOr";
		case BitwiseAnd:		 return os << "BitwiseAnd";
		case BitwiseOr:			 return os << "BitwiseOr";
		case BitwiseXor:		 return os << "BitwiseXor";
		case LeftShift:			 return os << "LeftShift";
		case RightShift:		 return os << "RightShift";
		default:				 UNREACHABLE();
	}
}