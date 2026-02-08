#include "ExprLowerer.h"

namespace mir {
ExprLowerer::ExprLowerer(Function &func)
	: m_CurrentFunc(func) {}

void ExprLowerer::emit(Box<Instr> instr) {
	m_Instrs.push_back(std::move(instr));
}

void ExprLowerer::addToCleanup(RegID reg, type::TypePtr type) {
	m_CleanupValues.push_back({reg, std::move(type)});
}

void ExprLowerer::removeFromCleanup(RegID reg) {
	const auto cond = [reg](const TrackedValue &val) -> bool { return val.reg == reg; };
	std::remove_if(m_CleanupValues.begin(), m_CleanupValues.end(), cond);
}

ExprResult ExprLowerer::visit(const ast::IntLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	emit(std::make_unique<IntLit>(reg, n.value));

	return {reg, true, type};
}

ExprResult ExprLowerer::visit(const ast::BoolLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	emit(std::make_unique<BoolLit>(reg, n.value));

	return {reg, true, type};
}

ExprResult ExprLowerer::visit(const ast::CharLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	emit(std::make_unique<CharLit>(reg, n.value));

	return {reg, true, type};
}

ExprResult ExprLowerer::visit(const ast::UnitLit &n) {
	const auto reg = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	emit(std::make_unique<UnitLit>(reg));

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
			removeFromCleanup(expr.reg);
		} else {
			// Non temporary values need to be copied
			emit(std::make_unique<SPRetain>(expr.reg, expr.type));
		}
	}

	// Create the actual requested pointer
	emit(std::make_unique<SPCreate>(dest, expr.type));
	emit(std::make_unique<Assign>(dest, expr.reg));

	addToCleanup(dest, type);

	return {dest, true, type};
}

ExprResult ExprLowerer::visit(const ast::VarRef &n) {
	UNREACHABLE();
	// Lookup the register assigned to this variable name
	// const auto reg = m_CurrentFunc.lookup(n.name);
	// const auto &type = n.inferredType.value();
	// return {reg, false, type};
}

ExprResult ExprLowerer::visit(const ast::UnaryExpr &n) {}

ExprResult ExprLowerer::visit(const ast::BinaryExpr &n) {
	const auto left = dispatch(*n.left);
	const auto right = dispatch(*n.right);
	const auto dest = m_CurrentFunc.nextRegID();
	const auto &type = n.inferredType.value();

	emit(std::make_unique<BinaryOp>(dest, left.reg, right.reg, n.op));

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
				emit(std::make_unique<SPRetain>(result.reg, result.type));
			} else {
				// It's a temporary; the callee takes over the cleanup
				removeFromCleanup(result.reg);
			}
		}
	}

	const auto dest = m_CurrentFunc.nextRegID();
	emit(std::make_unique<Call>(dest, callee.reg, argRegs));

	// If the return type is a pointer, the callee gives us a +1 reference
	if (funcReturnType->isTypeKind(type::TypeKind::Pointer)) {
		addToCleanup(dest, funcReturnType);
	}

	return {dest, true, funcReturnType};
}

ExprResult ExprLowerer::visit(const ast::Assignment &n) {}
}