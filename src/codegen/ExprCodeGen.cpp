#include "ExprCodeGen.h"

#include <algorithm>

#include "type/TypeFactory.h"

namespace gen {
namespace {
llvm::Value *coerceNullToPointer(gen::CodeGenContext &ctx, llvm::Value *value, Type sourceType,
								 Type targetType) {
	if (!sourceType->isTypeKind(TypeKind::Null) || !targetType->isTypeKind(TypeKind::Pointer)) {
		return value;
	}

	auto *targetLlvmType = ctx.typeConverter.convert(targetType);
	if (value->getType() == targetLlvmType) {
		return value;
	}

	return ctx.irBuilder.CreateBitCast(value, targetLlvmType);
}

void emitNullDerefTrap(gen::CodeGenContext &ctx, llvm::Value *ptr) {
	auto *ptrTy = llvm::dyn_cast<llvm::PointerType>(ptr->getType());
	VERIFY(ptrTy);

	auto *func = ctx.irBuilder.GetInsertBlock()->getParent();
	VERIFY(func);

	auto *isNull = ctx.irBuilder.CreateICmpEQ(ptr, llvm::ConstantPointerNull::get(ptrTy));
	auto *panicBB = llvm::BasicBlock::Create(ctx.llvmContext, "deref.null.panic", func);
	auto *contBB = llvm::BasicBlock::Create(ctx.llvmContext, "deref.cont", func);

	ctx.irBuilder.CreateCondBr(isNull, panicBB, contBB);

	ctx.irBuilder.SetInsertPoint(panicBB);
	auto *panicType = llvm::FunctionType::get(ctx.irBuilder.getVoidTy(), false);
	auto panic = ctx.llvmModule.getOrInsertFunction("__panic_null_deref", panicType);
	ctx.irBuilder.CreateCall(panic, {});
	ctx.irBuilder.CreateUnreachable();

	ctx.irBuilder.SetInsertPoint(contBB);
}
}

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
		m_Context.dropValue(value, type);
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

			// We must return the address at which the pointee lives, not the loaded pointee value.
			llvm::Value *ptrValue = nullptr;
			Type operandType;

			// If operand is a plain VarRef, get its alloca and load it to obtain the pointer value.
			if (unaryExpr.operand->kind == ast::NodeKind::VarRef) {
				const auto &varRef = static_cast<const ast::VarRef &>(*unaryExpr.operand);
				const auto reg = m_AllocManager.getAlloca(varRef.ident);
				VERIFY(reg.has_value());

				auto *const llvmOperandType = m_Context.typeConverter.convert(reg.value().type);
				ptrValue = m_Context.irBuilder.CreateLoad(llvmOperandType, reg.value().value);
				operandType = reg.value().type;
			} else {
				// For other operands (heap alloc, function returns pointer, complex expr),
				// lowerExpr should produce the pointer value directly.
				const auto [val, ty, _] = lowerExpr(*unaryExpr.operand);
				ptrValue = val;
				operandType = ty;
			}

			// operandType should be a pointer type; dereferencing yields the pointee type.
			const auto ptrType = static_cast<PointerType *>(operandType);
			VERIFY(ptrType);

			const auto pointeeType = ptrType->pointeeType;

			// Return the pointer (address) to the pointee as the lvalue.
			// isTemp=false because this represents an addressable location.
			emitNullDerefTrap(m_Context, ptrValue);
			return {.value = ptrValue, .type = pointeeType, .isTemp = false};
		}

		case ast::NodeKind::FieldAccess: {
			const auto &fieldAccess = static_cast<const ast::FieldAccess &>(n);
			const auto [baseAddr, baseType, _] = lowerLValue(*fieldAccess.base);
			VERIFY(baseType->isTypeKind(TypeKind::Struct));

			auto *structType = static_cast<StructType *>(baseType);
			u32 fieldIndex = 0;
			Type fieldType = nullptr;
			bool found = false;

			for (u32 i = 0; i < structType->orderedFields.size(); ++i) {
				const auto &[fieldName, type] = structType->orderedFields[i];
				if (fieldName == fieldAccess.field) {
					fieldIndex = i;
					fieldType = type;
					found = true;
					break;
				}
			}

			VERIFY(found && fieldType);

			auto *const llvmStructType =
					static_cast<llvm::StructType *>(m_Context.typeConverter.convert(structType));
			auto *const fieldPtr =
					m_Context.irBuilder.CreateStructGEP(llvmStructType, baseAddr, fieldIndex);

			return {.value = fieldPtr, .type = fieldType, .isTemp = false};
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

ExprResult ExprLowerer::visit(const ast::NullLit &n) {
	const auto &type = n.inferredType.value();
	auto *llvmType = static_cast<llvm::PointerType *>(m_Context.typeConverter.convert(type));
	auto *value = llvm::ConstantPointerNull::get(llvmType);

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

ExprResult ExprLowerer::visit(const ast::FieldAccess &n) {
	const auto [fieldAddr, fieldType, _] = lowerLValue(n);
	auto *const llvmFieldType = m_Context.typeConverter.convert(fieldType);
	auto *const value = m_Context.irBuilder.CreateLoad(llvmFieldType, fieldAddr);

	return {.value = value, .type = fieldType, .isTemp = false};
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
			emitNullDerefTrap(m_Context, value);

			const auto actualPointeeType = ptrType->pointeeType;
			auto *const llvmPointeeType = m_Context.typeConverter.convert(actualPointeeType);
			auto *const result = m_Context.irBuilder.CreateLoad(llvmPointeeType, value);

			return {.value = result, .type = actualPointeeType, .isTemp = false};
		}

		default: UNREACHABLE();
	}
}

ExprResult ExprLowerer::visit(const ast::BinaryExpr &n) {
	auto [left, leftType, isLeftTemp] = lowerExpr(*n.left);
	auto [right, rightType, isRightTemp] = lowerExpr(*n.right);

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
			if (leftType == TypeFactory::getChar() || leftType == TypeFactory::getI32() ||
				leftType == TypeFactory::getBool()) {
				auto *const val = m_Context.irBuilder.CreateICmpEQ(left, right);
				return {.value = val, .type = TypeFactory::getBool(), .isTemp = true};
			}

			if ((leftType->isTypeKind(TypeKind::Pointer) && rightType->isTypeKind(TypeKind::Null)) ||
				(leftType->isTypeKind(TypeKind::Null) && rightType->isTypeKind(TypeKind::Pointer)) ||
				(leftType->isTypeKind(TypeKind::Pointer) && rightType->isTypeKind(TypeKind::Pointer))) {
				if (leftType->isTypeKind(TypeKind::Null) && rightType->isTypeKind(TypeKind::Pointer)) {
					left = coerceNullToPointer(m_Context, left, leftType, rightType);
				}

				if (rightType->isTypeKind(TypeKind::Null) && leftType->isTypeKind(TypeKind::Pointer)) {
					right = coerceNullToPointer(m_Context, right, rightType, leftType);
				}

				auto *const val = m_Context.irBuilder.CreateICmpEQ(left, right);
				return {.value = val, .type = TypeFactory::getBool(), .isTemp = true};
			}
			UNREACHABLE();

		case Inequality:
			if (leftType == TypeFactory::getI32() || leftType == TypeFactory::getChar() ||
				leftType == TypeFactory::getBool()) {
				auto *const val = m_Context.irBuilder.CreateICmpNE(left, right);
				return {.value = val, .type = TypeFactory::getBool(), .isTemp = true};
			}

			if ((leftType->isTypeKind(TypeKind::Pointer) && rightType->isTypeKind(TypeKind::Null)) ||
				(leftType->isTypeKind(TypeKind::Null) && rightType->isTypeKind(TypeKind::Pointer)) ||
				(leftType->isTypeKind(TypeKind::Pointer) && rightType->isTypeKind(TypeKind::Pointer))) {
				if (leftType->isTypeKind(TypeKind::Null) && rightType->isTypeKind(TypeKind::Pointer)) {
					left = coerceNullToPointer(m_Context, left, leftType, rightType);
				}

				if (rightType->isTypeKind(TypeKind::Null) && leftType->isTypeKind(TypeKind::Pointer)) {
					right = coerceNullToPointer(m_Context, right, rightType, leftType);
				}

				auto *const val = m_Context.irBuilder.CreateICmpNE(left, right);
				return {.value = val, .type = TypeFactory::getBool(), .isTemp = true};
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
	// Check if this is a struct constructor call (e.g., Point { ... })
	// by seeing if the expression is a bare identifier that refers to a struct type
	bool isStructConstructor = false;
	if (const auto *varRef = dynamic_cast<const ast::VarRef *>(n.expr.get())) {
		if (n.inferredType.value()->isTypeKind(TypeKind::Struct)) {
			const auto *structType = static_cast<StructType *>(n.inferredType.value());
			if (structType->name == varRef->ident) {
				isStructConstructor = true;
			}
		}
	}

	if (isStructConstructor) {
		const auto resultType = n.inferredType.value();
		auto *const llvmResultType =
				static_cast<llvm::StructType *>(m_Context.typeConverter.convert(resultType));
		llvm::Value *aggregate = llvm::UndefValue::get(llvmResultType);

		for (u32 i = 0; i < n.args.size(); ++i) {
			const auto &[resValue, resType, resIsTemp] = lowerExpr(*n.args[i]);
			const auto *structType = static_cast<StructType *>(resultType);
			const auto &fieldType = structType->orderedFields[i].second;

			auto *valueToInsert = coerceNullToPointer(m_Context, resValue, resType, fieldType);
			if (!resIsTemp) {
				valueToInsert = m_Context.copyValue(valueToInsert, fieldType);
			} else {
				removeFromExprCleanup(resValue);
			}

			aggregate = m_Context.irBuilder.CreateInsertValue(aggregate, valueToInsert, {i});
		}

		addToExprCleanup(aggregate, resultType);

		return {.value = aggregate, .type = resultType, .isTemp = true};
	}

	const auto &[callee, type, isTemp] = lowerExpr(*n.expr);
	VERIFY(type->isTypeKind(TypeKind::Function));
	const auto funcType = static_cast<FunctionType *>(type);

	Vec<llvm::Value *> args;
	args.reserve(n.args.size());

	for (u32 i = 0; i < n.args.size(); ++i) {
		const auto &arg = n.args[i];
		const auto &[resValue, resType, resIsTemp] = lowerExpr(*arg);
		auto *argValue = coerceNullToPointer(m_Context, resValue, resType, funcType->paramTypes[i]);

		if (resIsTemp) {
			removeFromExprCleanup(resValue);
			args.push_back(argValue);
		} else {
			args.push_back(m_Context.copyValue(argValue, funcType->paramTypes[i]));
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
	auto *adjustedRight = coerceNullToPointer(m_Context, right, rightType, leftType);
	const auto &llvmLeftType = m_Context.typeConverter.convert(n.left->inferredType.value());
	const auto &left = m_Context.irBuilder.CreateLoad(llvmLeftType, leftLValue);

	// First retain the right side
	if (isRightTemp) {
		removeFromExprCleanup(right);
	} else {
		m_Context.copyValue(adjustedRight, leftType);
	}

	// If left is temp the expression cleanup will take care of it
	if (!isLeftTemp) {
		m_Context.dropValue(left, leftType);
	}

	if (n.assignmentKind == AssignmentKind::Simple) {
		m_Context.irBuilder.CreateStore(adjustedRight, leftLValue);
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
