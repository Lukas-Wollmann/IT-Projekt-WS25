#pragma once
#include "AST.h"
#include "Typedef.h"

///
/// An interface for all traversing passes over nodes of the Abstract Syntax Tree.
/// The visitor uses double dispatch to call the correct visit method for the
/// corresponding node type. The visited note can be modified.
///
template <typename T, bool IsConst = false>
struct Visitor {
public:
	virtual ~Visitor() = default;

    template<typename U>
    using MaybeConst = std::conditional_t<IsConst, const U, U>;

	T dispatch(MaybeConst<ast::Node> &node) {
		using enum ast::NodeKind;

		switch (node.kind) {
			case IntLit:	 return visit(static_cast<MaybeConst<ast::IntLit &>>(node));
			case FloatLit:	 return visit(static_cast<MaybeConst<ast::FloatLit &>>(node));
			case CharLit:	 return visit(static_cast<MaybeConst<ast::CharLit &>>(node));
			case BoolLit:	 return visit(static_cast<MaybeConst<ast::BoolLit &>>(node));
			case StringLit:	 return visit(static_cast<MaybeConst<ast::StringLit &>>(node));
			case ArrayExpr:	 return visit(static_cast<MaybeConst<ast::ArrayExpr &>>(node));
			case UnaryExpr:	 return visit(static_cast<MaybeConst<ast::UnaryExpr &>>(node));
			case BinaryExpr: return visit(static_cast<MaybeConst<ast::BinaryExpr &>>(node));
			case FuncCall:	 return visit(static_cast<MaybeConst<ast::FuncCall &>>(node));
			case VarRef:	 return visit(static_cast<MaybeConst<ast::VarRef &>>(node));
			case BlockStmt:	 return visit(static_cast<MaybeConst<ast::BlockStmt &>>(node));
			case IfStmt:	 return visit(static_cast<MaybeConst<ast::IfStmt &>>(node));
			case WhileStmt:	 return visit(static_cast<MaybeConst<ast::WhileStmt &>>(node));
			case ReturnStmt: return visit(static_cast<MaybeConst<ast::ReturnStmt &>>(node));
			case VarDef:	 return visit(static_cast<MaybeConst<ast::VarDef &>>(node));
			case FuncDecl:	 return visit(static_cast<MaybeConst<ast::FuncDecl &>>(node));
			case Module:	 return visit(static_cast<MaybeConst<ast::Module &>>(node));
			default:		 UNREACHABLE();
		}
	}

	virtual T visit(MaybeConst<ast::IntLit> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::FloatLit> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::CharLit> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::BoolLit> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::StringLit> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::ArrayExpr> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::UnaryExpr> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::BinaryExpr> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::FuncCall> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::VarRef> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::BlockStmt> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::IfStmt> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::WhileStmt> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::ReturnStmt> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::VarDef> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::FuncDecl> &) { UNREACHABLE(); }

	virtual T visit(MaybeConst<ast::Module> &) { UNREACHABLE(); }
};

///
/// An interface for all traversing passes over nodes of the Abstract Syntax Tree.
/// The visitor uses double dispatch to call the correct visit method for the
/// corresponding node type. The visited note can not be modified.
///
template<typename T>
using ConstVisitor = Visitor<T, true>; 
