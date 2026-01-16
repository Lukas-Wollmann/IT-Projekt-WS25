#pragma once
#include "HIR.h"
#include "Typedef.h"
#include "ast/AST.h"

namespace hir {
	struct LoweringContext {
	private:
		Function &m_Func;
		Block &m_Block;
		std::unordered_map<U8String, LocalID> m_Locals;

	public:
		LoweringContext(Function &func, Block &block);

		LocalID lowerHeapAlloc(const ast::HeapAlloc &n);
		LocalID lowerIntLit(const ast::IntLit &n);
		LocalID lowerUnaryExpr(const ast::UnaryExpr &n);
		LocalID lowerBinaryExpr(const ast::BinaryExpr &n);
		LocalID lowerExpr(const ast::Expr &n);
	};
}