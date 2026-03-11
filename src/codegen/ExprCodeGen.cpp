#include "ExprCodeGen.h"

#include <algorithm>

#include "type/TypeFactory.h"

namespace gen {
ExprLowerer::ExprLowerer(CodeGenContext &ctx, AllocManager &allocManager)
	: m_Context(ctx)
	, m_AllocManager(allocManager) {}

ExprResult ExprLowerer::lowerExpr(const ast::Expr &n) {
	return dispatch(n);
}

void ExprLowerer::addToExprCleanup(llvm::Value *value, Type type) {
	m_ExprCleanup.emplace_back(value, type);
}

void ExprLowerer::emitExprCleanup() {
	for (const auto &[value, type] : m_ExprCleanup) {
		if (auto dtor = m_Context.getDestructor(type)) {
			const auto dtorType = m_Context.getDestructorType();
			m_Context.irBuilder.CreateCall(dtorType, dtor.value(), {value});
		}
	}
	m_ExprCleanup.clear();
}

void ExprLowerer::removeFromExprCleanup(llvm::Value *value) {
	const auto cond = [value](const TrackedValue &tracked) { return tracked.value == value; };
	const auto it = std::remove_if(m_ExprCleanup.begin(), m_ExprCleanup.end(), cond);
	m_ExprCleanup.erase(it, m_ExprCleanup.end());
}

ExprResult ExprLowerer::lowerLValue(const ast::Expr &n) {
	switch (n.kind) {
		case ast::NodeKind::VarRef: {
			const auto &varRef = static_cast<const ast::VarRef &>(n);
			const auto reg = m_AllocManager.getAlloca(varRef.ident);

			VERIFY(reg.has_value());

			return {.value = reg.value().value, .type = reg.value().type, .isTemp = false};
		}

		case ast::NodeKind::UnaryExpr: {
			const auto &unaryExpr = static_cast<const ast::UnaryExpr &>(n);
			VERIFY(unaryExpr.op == UnaryOpKind::Dereference);

			const auto expr = lowerExpr(*unaryExpr.operand);

			return expr;
		}

		default: UNREACHABLE();
	}
}

ExprResult ExprLowerer::visit(const ast::IntLit &n) {
	const auto &type = n.inferredType.value();
	auto *const llvmType = m_Context.typeConverter.convert(type);
	auto *const value = llvm::ConstantInt::get(llvmType, n.value);

	return {.value = value, .type = type, .isTemp = true};
}

ExprResult ExprLowerer::visit(const ast::BoolLit &n) {
	const auto &type = n.inferredType.value();
	auto *const llvmType = m_Context.typeConverter.convert(type);
	auto *const value = llvm::ConstantInt::get(llvmType, static_cast<uint64_t>(n.value));

	return {.value = value, .type = type, .isTemp = true};
}

ExprResult ExprLowerer::visit(const ast::CharLit &n) {
	const auto &type = n.inferredType.value();
	auto *const llvmType = m_Context.typeConverter.convert(type);
	auto *const value = llvm::ConstantInt::get(llvmType, n.value);

	return {.value = value, .type = type, .isTemp = true};
}

ExprResult ExprLowerer::visit(const ast::UnitLit &n) {
	const auto &type = n.inferredType.value();
	auto *const llvmType = m_Context.typeConverter.convert(type);
	auto *const value = llvm::ConstantInt::get(llvmType, 0);

	return {.value = value, .type = type, .isTemp = true};
}

ExprResult ExprLowerer::visit(const ast::HeapAlloc &n) {
	const auto [value, type, isTemp] = lowerExpr(*n.expr);
	const auto &ptrType = n.inferredType.value();

	// If the value is a temporary, we can just move it into the heap allocation.
	// If it's not a temporary, we need to copy it first so that the heap allocation
	// owns the data that was put inside it.
	if (isTemp) {
		removeFromExprCleanup(value);
	} else {
		m_Context.copyValue(value, type);
	}

	// Create a new shared pointer via the runtime
	auto *const func = m_Context.llvmModule.getFunction(CodeGenContext::sharedPtrCreate);
	VERIFY(func);
	auto *const size = m_Context.sizeOf(type);
	const auto nullDtor = m_Context.getNullDestructor();
	auto *const dtor = m_Context.getDestructor(type).value_or(nullDtor);
	auto *const sharedPtr = m_Context.irBuilder.CreateCall(func, {size, dtor});

	// Store the value into the shared pointer
	auto *const elemType = m_Context.typeConverter.convert(type);
	auto *const destPointerType = llvm::PointerType::getUnqual(elemType);
	auto *const dest = m_Context.irBuilder.CreateBitCast(sharedPtr, destPointerType);
	m_Context.irBuilder.CreateStore(value, dest);

	// Now the pointer is owned by the current expression, so we need to clean it up after
	// the expression is done. The pointer might still be moved by an outer node.
	addToExprCleanup(sharedPtr, ptrType);

	return {.value = sharedPtr, .type = ptrType, .isTemp = true};
}

ExprResult ExprLowerer::visit(const ast::VarRef &n) {
	const auto &type = n.inferredType.value();
	auto *const llvmType = m_Context.typeConverter.convert(type);

	if (const auto alloc = m_AllocManager.getAlloca(n.ident)) {
		auto *const ptrToValue = alloc.value().value;
		auto *const value = m_Context.irBuilder.CreateLoad(llvmType, ptrToValue);

		// Return local variable as a borrow
		return {.value = value, .type = type, .isTemp = false};
	}

	if (auto *const func = m_Context.llvmModule.getFunction(n.ident.asAscii())) {
		// Return a function pointer
		return {.value = func, .type = type, .isTemp = false};
	}

	UNREACHABLE();
}

ExprResult ExprLowerer::visit(const ast::UnaryExpr &n) {
	const auto [value, type, _] = lowerExpr(*n.operand);

	switch (n.op) {
		case UnaryOpKind::Positive: {
			return {.value = value, .type = type, .isTemp = true};
		}

		case UnaryOpKind::Negative: {
			auto *const result = m_Context.irBuilder.CreateNeg(value);
			return {.value = result, .type = type, .isTemp = true};
		}

		case UnaryOpKind::LogicalNot: {
			auto *const result = m_Context.irBuilder.CreateNot(value);
			return {.value = result, .type = type, .isTemp = true};
		}

		case UnaryOpKind::BitwiseNot: {
			auto *const result = m_Context.irBuilder.CreateNot(value);
			return {.value = result, .type = type, .isTemp = true};
		}

		case UnaryOpKind::Dereference: {
			const auto ptrType = static_cast<PointerType *>(type);
			VERIFY(ptrType);

			const auto actualPointeeType = ptrType->pointeeType;
			auto *const llvmPointeeType = m_Context.typeConverter.convert(actualPointeeType);
			auto *const result = m_Context.irBuilder.CreateLoad(llvmPointeeType, value);

			return {.value = result, .type = actualPointeeType, .isTemp = false};
		}

		default: UNREACHABLE();
	}
}

ExprResult ExprLowerer::visit(const ast::BinaryExpr &n) {
	const auto &[left, leftType, isLeftTemp] = lowerExpr(*n.left);
	const auto &[right, rightType, isRightTemp] = lowerExpr(*n.right);

	using enum BinaryOpKind;
	switch (n.op) {
		case Addition:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateAdd(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case Subtraction:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateSub(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case Multiplication:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateMul(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case Division:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateSDiv(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case Modulo:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateSRem(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case Equality:
			if (leftType == TypeFactory::getChar() || leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateICmpEQ(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case Inequality:
			if (leftType == TypeFactory::getI32() || leftType == TypeFactory::getChar()) {
				auto *const val = m_Context.irBuilder.CreateICmpNE(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case LessThan:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateICmpSLT(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case LessThanOrEqual:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateICmpSLE(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case GreaterThan:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateICmpSGT(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case GreaterThanOrEqual:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateICmpSGE(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case LogicalAnd:
			if (leftType == TypeFactory::getBool()) {
				auto *const val = m_Context.irBuilder.CreateAnd(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case LogicalOr:
			if (leftType == TypeFactory::getBool()) {
				auto *const val = m_Context.irBuilder.CreateOr(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case BitwiseAnd:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateAnd(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case BitwiseOr:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateOr(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case BitwiseXor:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateXor(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case LeftShift:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateShl(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		case RightShift:
			if (leftType == TypeFactory::getI32()) {
				auto *const val = m_Context.irBuilder.CreateAShr(left, right);
				return {.value = val, .type = leftType, .isTemp = true};
			}
			UNREACHABLE();

		default: UNREACHABLE();
	}
}

ExprResult ExprLowerer::visit(const ast::FuncCall &n) {
	const auto &[callee, type, isTemp] = lowerExpr(*n.expr);
	VERIFY(type->isTypeKind(TypeKind::Function));
	const auto funcType = static_cast<FunctionType *>(type);

	Vec<llvm::Value *> args;
	args.reserve(n.args.size());

	for (const auto &arg : n.args) {
		const auto &[resValue, resType, resIsTemp] = lowerExpr(*arg);

		if (resIsTemp) {
			removeFromExprCleanup(resValue);
			args.push_back(resValue);
		} else {
			args.push_back(m_Context.copyValue(resValue, resType));
		}
	}
	auto *llvmFuncType = static_cast<llvm::FunctionType *>(m_Context.typeConverter.convert(type));

	const auto &call = m_Context.irBuilder.CreateCall(llvmFuncType, callee, args);

	addToExprCleanup(call, funcType->returnType);

	return {.value = call, .type = n.inferredType.value(), .isTemp = true};
}

ExprResult ExprLowerer::visit(const ast::Assignment &n) {
	const auto &[leftLValue, leftType, isLeftTemp] = lowerLValue(*n.left);
	const auto &[right, rightType, isRightTemp] = lowerExpr(*n.right);
	const auto &llvmLeftType = m_Context.typeConverter.convert(n.left->inferredType.value());
	const auto &left = m_Context.irBuilder.CreateLoad(llvmLeftType, leftLValue);

	// First retain the right side
	if (isRightTemp) {
		removeFromExprCleanup(right);
	} else {
		m_Context.copyValue(right, rightType);
	}

	// If left is temp the expression cleanup will take care of it
	if (!isLeftTemp) {
		m_Context.dropValue(left, leftType);
	}

	if (n.assignmentKind == AssignmentKind::Simple) {
		m_Context.irBuilder.CreateStore(right, leftLValue);
		return {.value = llvm::ConstantInt::getNullValue(m_Context.irBuilder.getInt8Ty()),
				.type = leftType,
				.isTemp = true};
	}

	llvm::Value *res = nullptr;

	using enum AssignmentKind;
	switch (n.assignmentKind) {
		case Addition:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateAdd(left, right);
				break;
			}
			UNREACHABLE();

		case Subtraction:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateSub(left, right);
				break;
			}
			UNREACHABLE();

		case Multiplication:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateMul(left, right);
				break;
			}
			UNREACHABLE();

		case Division:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateSDiv(left, right);
				break;
			}
			UNREACHABLE();

		case Modulo:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateSRem(left, right);
				break;
			}
			UNREACHABLE();

		case BitwiseAnd:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateAnd(left, right);
				break;
			}
			UNREACHABLE();

		case BitwiseOr:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateOr(left, right);
				break;
			}
			UNREACHABLE();

		case BitwiseXor:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateXor(left, right);
				break;
			}
			UNREACHABLE();

		case LeftShift:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateShl(left, right);
				break;
			}
			UNREACHABLE();

		case RightShift:
			if (leftType == TypeFactory::getI32()) {
				res = m_Context.irBuilder.CreateAShr(left, right);
				break;
			}
			UNREACHABLE();

		default: UNREACHABLE();
	}

	m_Context.irBuilder.CreateStore(res, leftLValue);

	return {.value = llvm::ConstantInt::getNullValue(m_Context.irBuilder.getInt8Ty()),
			.type = leftType,
			.isTemp = true};
}
}
