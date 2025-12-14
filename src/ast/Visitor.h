#pragma once
#include "AST.h"
#include "Macros.h"
#include "Typedef.h"

namespace ast {
	///
	/// An interface for all traversing passes over nodes of the Abstract Syntax Tree.
	/// The visitor uses a dispatch method to call the correct visit method for the
	/// corresponding node kind. The visited note can be modified.
	///
	template <typename T, bool IsConst = false>
	struct Visitor {
	public:
		virtual ~Visitor() = default;

		template <typename U>
		using Constness = std::conditional_t<IsConst, const U, U>;

		T dispatch(Constness<Node> &node) {
			switch (node.kind) {
				case NodeKind::IntLit:	   return visit(static_cast<Constness<IntLit> &>(node));
				case NodeKind::FloatLit:   return visit(static_cast<Constness<FloatLit> &>(node));
				case NodeKind::CharLit:	   return visit(static_cast<Constness<CharLit> &>(node));
				case NodeKind::BoolLit:	   return visit(static_cast<Constness<BoolLit> &>(node));
				case NodeKind::StringLit:  return visit(static_cast<Constness<StringLit> &>(node));
				case NodeKind::UnitLit:	   return visit(static_cast<Constness<UnitLit> &>(node));
				case NodeKind::ArrayExpr:  return visit(static_cast<Constness<ArrayExpr> &>(node));
				case NodeKind::UnaryExpr:  return visit(static_cast<Constness<UnaryExpr> &>(node));
				case NodeKind::BinaryExpr: return visit(static_cast<Constness<BinaryExpr> &>(node));
				case NodeKind::HeapAlloc:  return visit(static_cast<Constness<HeapAlloc> &>(node));
				case NodeKind::Assignment: return visit(static_cast<Constness<Assignment> &>(node));
				case NodeKind::FuncCall:   return visit(static_cast<Constness<FuncCall> &>(node));
				case NodeKind::VarRef:	   return visit(static_cast<Constness<VarRef> &>(node));
				case NodeKind::BlockStmt:  return visit(static_cast<Constness<BlockStmt> &>(node));
				case NodeKind::IfStmt:	   return visit(static_cast<Constness<IfStmt> &>(node));
				case NodeKind::WhileStmt:  return visit(static_cast<Constness<WhileStmt> &>(node));
				case NodeKind::ReturnStmt: return visit(static_cast<Constness<ReturnStmt> &>(node));
				case NodeKind::VarDef:	   return visit(static_cast<Constness<VarDef> &>(node));
				case NodeKind::FuncDecl:   return visit(static_cast<Constness<FuncDecl> &>(node));
				case NodeKind::Module:	   return visit(static_cast<Constness<Module> &>(node));
				default:				   UNREACHABLE();
			}
		}

		virtual T visit(Constness<IntLit> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<FloatLit> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<CharLit> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<BoolLit> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<StringLit> &) {
			UNREACHABLE();
		}

        virtual T visit(Constness<UnitLit> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<ArrayExpr> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<UnaryExpr> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<BinaryExpr> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<HeapAlloc> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<Assignment> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<FuncCall> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<VarRef> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<BlockStmt> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<IfStmt> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<WhileStmt> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<ReturnStmt> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<VarDef> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<FuncDecl> &) {
			UNREACHABLE();
		}

		virtual T visit(Constness<Module> &) {
			UNREACHABLE();
		}
	};

	///
	/// An interface for all traversing passes over nodes of the Abstract Syntax Tree.
	/// The visitor usesa dispatch method to call the correct visit method for the
	/// corresponding node kind. The visited note can not be modified.
	///
	template <typename T>
	using ConstVisitor = Visitor<T, true>;
}