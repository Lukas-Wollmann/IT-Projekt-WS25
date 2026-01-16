#include "LoweringContext.h"

namespace hir {
	LoweringContext::LoweringContext(Function &func, Block &block)
		: m_Func(func)
		, m_Block(block) {}

	LocalID LoweringContext::lowerHeapAlloc(const ast::HeapAlloc &n) {
		const auto value = lowerExpr(*n.expr);
		const auto type = n.inferredType.value();
		const auto ptrType = std::make_shared<type::PointerType>(type);
		const auto ptrLocal = m_Func.newLocal(ptrType);

		m_Block.addStmt(
				std::make_unique<HeapAlloc>(ptrLocal, std::make_unique<LocalRef>(value), type));

		return ptrLocal;
	}

	LocalID LoweringContext::lowerIntLit(const ast::IntLit &n) {
		const auto dest = m_Func.newLocal(n.inferredType.value());

		m_Block.addStmt(std::make_unique<StackAlloc>(dest, n.inferredType.value()));
		m_Block.addStmt(std::make_unique<Copy>(dest, std::make_unique<IntLit>(n.value)));

		return dest;
	}

	LocalID LoweringContext::lowerUnaryExpr(const ast::UnaryExpr &n) {
		const auto src = lowerExpr(*n.operand);
		const auto dest = m_Func.newLocal(n.inferredType.value());

		m_Block.addStmt(std::make_unique<UnaryOp>(dest, std::make_unique<LocalRef>(src), n.op));

		return dest;
	}

	LocalID LoweringContext::lowerBinaryExpr(const ast::BinaryExpr &n) {
		const auto left = lowerExpr(*n.left);
		const auto right = lowerExpr(*n.right);
		const auto dest = m_Func.newLocal(n.inferredType.value());

		m_Block.addStmt(std::make_unique<BinaryOp>(dest, std::make_unique<LocalRef>(left),
												   std::make_unique<LocalRef>(right), n.op));

		return dest;
	}

	LocalID LoweringContext::lowerExpr(const ast::Expr &n) {
		switch (n.kind) {
			case ast::NodeKind::IntLit: return lowerIntLit(static_cast<const ast::IntLit &>(n));
			case ast::NodeKind::UnaryExpr:
				return lowerUnaryExpr(static_cast<const ast::UnaryExpr &>(n));
			case ast::NodeKind::BinaryExpr:
				return lowerBinaryExpr(static_cast<const ast::BinaryExpr &>(n));
			case ast::NodeKind::HeapAlloc:
				return lowerHeapAlloc(static_cast<const ast::HeapAlloc &>(n));

			default: UNREACHABLE();
		}
	}
}