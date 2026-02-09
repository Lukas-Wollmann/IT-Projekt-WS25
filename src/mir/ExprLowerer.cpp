#include "ExprLowerer.h"

namespace mir {
ExprLowerer::ExprLowerer(Function &func, LoweringContext &ctx)
	: m_CurrentFunc(func)
	, m_Context(ctx) {}

ExprResult ExprLowerer::lowerExpr(const ast::Expr &n) {
	return dispatch(n);
}

LValue ExprLowerer::lowerLValue(const ast::Expr &n) {
	switch (n.kind) {
		case ast::NodeKind::VarRef: {
			const auto &varRef = static_cast<const ast::VarRef &>(n);
			const auto reg = m_CurrentFunc.lookup(varRef.ident);

			VERIFY(reg.has_value());

			return {reg.value(), false};
		}
		case ast::NodeKind::UnaryExpr: {
			const auto &unaryExpr = static_cast<const ast::UnaryExpr &>(n);

			VERIFY(unaryExpr.op == UnaryOpKind::Dereference);

			const auto expr = dispatch(*unaryExpr.operand);

			return {expr.reg, true};
		}
		default: UNREACHABLE();
	}
}

ExprResult ExprLowerer::visit(const ast::IntLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	m_Context.emit(std::make_unique<IntLit>(reg, n.value));

	return {reg, true, type};
}

ExprResult ExprLowerer::visit(const ast::BoolLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	m_Context.emit(std::make_unique<BoolLit>(reg, n.value));

	return {reg, true, type};
}

ExprResult ExprLowerer::visit(const ast::CharLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	m_Context.emit(std::make_unique<CharLit>(reg, n.value));

	return {reg, true, type};
}

ExprResult ExprLowerer::visit(const ast::UnitLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	m_Context.emit(std::make_unique<UnitLit>(reg));

	return {reg, true, type};
}

ExprResult ExprLowerer::visit(const ast::HeapAlloc &n) {
	const auto expr = dispatch(*n.expr);
	const auto &type = n.inferredType.value();
	const auto dest = m_CurrentFunc.nextRegID();

	// Right now the only types that are RAII managed are pointers
	if (expr.type->isTypeKind(type::TypeKind::Pointer)) {
		if (expr.isTemp) {
			// Temporarys can just be moved, they don't clean them up after the expression
			m_Context.removeFromCleanup(expr.reg);
		} else {
			// Non temporary values need to be copied
			m_Context.emit(std::make_unique<SPRetain>(expr.reg, expr.type));
		}
	}

	// Create the actual requested pointer
	m_Context.emit(std::make_unique<SPCreate>(dest, expr.type));
	m_Context.emit(std::make_unique<Store>(dest, expr.reg));

	m_Context.addToCleanup(dest, type);

	return {dest, true, type};
}

ExprResult ExprLowerer::visit(const ast::VarRef &n) {
	const auto &type = n.inferredType.value();

	// Return a local variable as a borrow
	if (const auto reg = m_CurrentFunc.lookup(n.ident))
		return {reg.value(), false, type};

	// If not found locally, assume it is a global function
	const auto dest = m_CurrentFunc.nextRegID();
	m_Context.emit(std::make_unique<LoadFunc>(dest, n.ident, type));

	return {dest, false, type};
}

ExprResult ExprLowerer::visit(const ast::UnaryExpr &n) {
	const auto operand = dispatch(*n.operand);
	const auto dest = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	m_Context.emit(std::make_unique<UnaryOp>(dest, operand.reg, n.op));

	return {dest, true, type};
}

ExprResult ExprLowerer::visit(const ast::BinaryExpr &n) {
	const auto left = dispatch(*n.left);
	const auto right = dispatch(*n.right);
	const auto dest = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	m_Context.emit(std::make_unique<BinaryOp>(dest, left.reg, right.reg, n.op));

	return {dest, true, type};
}

ExprResult ExprLowerer::visit(const ast::FuncCall &n) {
	// This will result into a pointer to a function
	const auto callee = dispatch(*n.expr);
	const auto &funcReturnType = n.inferredType.value();
	Vec<RegID> argRegs;

	// The caller (here) will make sure that all the arguments are correctly retained,
	// the called function will then only clean them up.
	for (const auto &arg : n.args) {
		const auto result = dispatch(*arg);

		if (result.type->isTypeKind(type::TypeKind::Pointer)) {
			if (!result.isTemp) {
				// It's a variable; we must increment for the callee
				m_Context.emit(std::make_unique<SPRetain>(result.reg, result.type));
			} else {
				// It's a temporary; the callee takes over the cleanup
				m_Context.removeFromCleanup(result.reg);
			}
		}
	}

	const auto dest = m_CurrentFunc.nextRegID();
	m_Context.emit(std::make_unique<Call>(dest, callee.reg, argRegs));

	// If the return type is a pointer, the callee gives us a +1 reference
	if (funcReturnType->isTypeKind(type::TypeKind::Pointer)) {
		m_Context.addToCleanup(dest, funcReturnType);
	}

	return {dest, true, funcReturnType};
}

ExprResult ExprLowerer::visit(const ast::Assignment &n) {
	const auto left = lowerLValue(*n.left);
	const auto right = dispatch(*n.right);
	const auto dest = m_CurrentFunc.nextRegID();
	const auto &leftType = n.left->inferredType.value();
	const auto &type = n.inferredType.value();

	if (leftType->isTypeKind(type::TypeKind::Pointer)) {
		if (!right.isTemp) {
			// It's a variable; we must increment for the callee
			m_Context.emit(std::make_unique<SPRetain>(right.reg, right.type));
		} else {
			// It's a temporary; the callee takes over the cleanup
			m_Context.removeFromCleanup(right.reg);
		}

		if (left.isMemory) {
			// We must release the pointer currently sitting in that memory
			const auto oldReg = m_CurrentFunc.nextRegID();
			m_Context.emit(std::make_unique<Load>(oldReg, left.reg, type));
			m_Context.emit(std::make_unique<SPRelease>(oldReg));
		} else {
			// Release the pointer currently held in the variable's register
			m_Context.emit(std::make_unique<SPRelease>(left.reg));
		}
	}

	m_Context.emit(std::make_unique<Store>(left.reg, right.reg));
	m_Context.emit(std::make_unique<UnitLit>(dest));

	return {dest, false, type};
}
}