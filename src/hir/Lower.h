#pragma once
#include "HIR.h"
#include "Typedef.h"
#include "ast/AST.h"

namespace hir {
	struct LowerContext {
	private:
		size_t m_NextLocalID;
		Box<FuncDecl> m_Func;

		Slot newSlot(type::TypePtr type, U8String name = u8"");

		Slot lowerExpr(const ast::Expr &n);
		Slot lowerIntLit(const ast::IntLit &n);
		Slot lowerVarRef(const ast::VarRef &n);
		Slot lowerUnaryExpr(const ast::UnaryExpr &n);
		Slot lowerBinaryExpr(const ast::BinaryExpr &n);
	};
}