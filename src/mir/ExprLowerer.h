#pragma once
#include <algorithm>

#include "LoweringContext.h"

struct ExprResult {
	llvm::Value *value;
	type::TypePtr type;
	bool isTemp;
};

struct ExprLowerer : ast::ConstVisitor<ExprResult> {
private:
	LoweringContext &m_LoweringContext;
	llvm::LLVMContext &m_LLVMContext;
	llvm::Module &m_LLVMModule;
	llvm::IRBuilder<> &m_LLVMBuilder;
	gen::TypeConverter &m_Converter;
	Vec<TrackedValue> m_ExprCleanup;

public:
	explicit ExprLowerer(LoweringContext &ctx)
		: m_LoweringContext(ctx)
		, m_LLVMContext(ctx.getLLVMContext())
		, m_LLVMModule(ctx.getLLVMModule())
		, m_LLVMBuilder(ctx.getIRBuilder())
		, m_Converter(ctx.getTypeConverter()) {}

	ExprResult lowerExpr(const ast::Expr &n) {
		return dispatch(n);
	}

	void addToExprCleanup(llvm::Value *value, const type::TypePtr &type) {
		m_ExprCleanup.emplace_back(value, type);
	}

	void emitExprCleanup() {
		for (const auto &[value, type] : m_ExprCleanup) {
			if (auto dtor = m_LoweringContext.getDtor(type)) {
				const auto dtorType = m_LoweringContext.getDtorType();
				m_LLVMBuilder.CreateCall(dtorType, dtor.value(), {value});
			}
		}
	}

	void removeFromExprCleanup(llvm::Value *value) {
		const auto cond = [value](const TrackedValue &tracked) { return tracked.value == value; };
		const auto [first, last] = std::ranges::remove_if(m_ExprCleanup, cond);
		m_ExprCleanup.erase(first, last);
	}

	ExprResult lowerLValue(const ast::Expr &n) {
		switch (n.kind) {
			case ast::NodeKind::VarRef: {
				const auto &varRef = static_cast<const ast::VarRef &>(n);
				const auto reg = m_LoweringContext.getAlloca(varRef.ident);

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

	ExprResult visit(const ast::IntLit &n) override {
		const auto &type = n.inferredType.value();
		auto *const llvmType = m_Converter.convertType(type);
		auto *const value = llvm::ConstantInt::get(llvmType, n.value);

		return {.value = value, .type = type, .isTemp = true};
	}

	ExprResult visit(const ast::BoolLit &n) override {
		const auto &type = n.inferredType.value();
		auto *const llvmType = m_Converter.convertType(type);
		auto *const value = llvm::ConstantInt::get(llvmType, static_cast<uint64_t>(n.value));

		return {.value = value, .type = type, .isTemp = true};
	}

	ExprResult visit(const ast::CharLit &n) override {
		const auto &type = n.inferredType.value();
		auto *const llvmType = m_Converter.convertType(type);
		auto *const value = llvm::ConstantInt::get(llvmType, n.value);

		return {.value = value, .type = type, .isTemp = true};
	}

	ExprResult visit(const ast::UnitLit &n) override {
		const auto &type = n.inferredType.value();
		auto *const llvmType = m_Converter.convertType(type);
		auto *const value = llvm::ConstantInt::get(llvmType, 0);

		return {.value = value, .type = type, .isTemp = true};
	}

	ExprResult visit(const ast::HeapAlloc &n) override {
		const auto [value, type, isTemp] = lowerExpr(*n.expr);
		const auto &ptrType = n.inferredType.value();

		// If the value is a temporary, we can just move it into the heap allocation.
		// If it's not a temporary, we need to copy it first so that the heap allocation
		// owns the data that was put inside it.
		if (isTemp) {
			removeFromExprCleanup(value);
		} else {
			m_LoweringContext.copyValue(value, type);
		}

		// Create a new shared pointer via the runtime
		auto *const func = m_LLVMModule.getFunction("__sp_create");
		VERIFY(func);
		auto *const size = m_LoweringContext.sizeOf(type);
		const auto nullDtor = m_LoweringContext.getNullDtor();
		auto *const dtor = m_LoweringContext.getDtor(type).value_or(nullDtor);
		auto *const sharedPtr = m_LLVMBuilder.CreateCall(func, {size, dtor});

		// Store the value into the shared pointer
		auto *const elemType = m_Converter.convertType(type);
		auto *const destPointerType = llvm::PointerType::getUnqual(elemType);
		auto *const dest = m_LLVMBuilder.CreateBitCast(sharedPtr, destPointerType);
		m_LLVMBuilder.CreateStore(value, dest);

		// Now the pointer is owned by the current expression, so we need to clean it up after
		// the expression is done. The pointer might still be moved by an outer node.
		addToExprCleanup(sharedPtr, ptrType);

		return {.value = sharedPtr, .type = ptrType, .isTemp = true};
	}

	ExprResult visit(const ast::VarRef &n) override {
		const auto &type = n.inferredType.value();
		auto *const llvmType = m_Converter.convertType(type);

		if (const auto alloc = m_LoweringContext.getAlloca(n.ident)) {
			auto *const ptrToValue = alloc.value().value;
			auto *const value = m_LLVMBuilder.CreateLoad(llvmType, ptrToValue);

			// Return local variable as a borrow
			return {.value = value, .type = type, .isTemp = false};
		}

		if (auto *const func = m_LLVMModule.getFunction(n.ident.asAscii())) {
			// Return a function pointer
			return {.value = func, .type = type, .isTemp = false};
		}

		UNREACHABLE();
	}

	ExprResult visit(const ast::UnaryExpr &n) override {
		const auto [value, type, _] = lowerExpr(*n.operand);

		switch (n.op) {
			case UnaryOpKind::Positive: {
				return {.value = value, .type = type, .isTemp = true};
			}

			case UnaryOpKind::Negative: {
				auto *const result = m_LLVMBuilder.CreateNeg(value);
				return {.value = result, .type = type, .isTemp = true};
			}

			case UnaryOpKind::LogicalNot: {
				auto *const result = m_LLVMBuilder.CreateNot(value);
				return {.value = result, .type = type, .isTemp = true};
			}

			case UnaryOpKind::BitwiseNot: {
				auto *const result = m_LLVMBuilder.CreateNot(value);
				return {.value = result, .type = type, .isTemp = true};
			}

			case UnaryOpKind::Dereference: {
				const auto ptrType = std::dynamic_pointer_cast<const type::PointerType>(type);
				VERIFY(ptrType);

				const auto actualPointeeType = ptrType->pointeeType;
				auto *const llvmPointeeType = m_Converter.convertType(actualPointeeType);
				auto *const result = m_LLVMBuilder.CreateLoad(llvmPointeeType, value);

				return {.value = result, .type = actualPointeeType, .isTemp = false};
			}

			default: UNREACHABLE();
		}
	}

	ExprResult visit(const ast::BinaryExpr &n) override {
		const auto &[left, leftType, isLeftTemp] = lowerExpr(*n.left);
		const auto &[right, rightType, isRightTemp] = lowerExpr(*n.right);

		using enum BinaryOpKind;
		switch (n.op) {
			case Addition:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateAdd(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case Subtraction:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateSub(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case Multiplication:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateMul(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case Division:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateSDiv(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case Modulo:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateSRem(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case Equality:
				if (*leftType == type::Typename(u8"char") || *leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateICmpEQ(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case Inequality:
				if (*leftType == type::Typename(u8"i32") || *leftType == type::Typename(u8"char")) {
					auto *const val = m_LLVMBuilder.CreateICmpNE(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case LessThan:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateICmpSLT(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case LessThanOrEqual:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateICmpSLE(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case GreaterThan:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateICmpSGT(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case GreaterThanOrEqual:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateICmpSGE(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case LogicalAnd:
				if (*leftType == type::Typename(u8"bool")) {
					auto *const val = m_LLVMBuilder.CreateAnd(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case LogicalOr:
				if (*leftType == type::Typename(u8"bool")) {
					auto *const val = m_LLVMBuilder.CreateOr(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case BitwiseAnd:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateAnd(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case BitwiseOr:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateOr(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case BitwiseXor:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateXor(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case LeftShift:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateShl(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			case RightShift:
				if (*leftType == type::Typename(u8"i32")) {
					auto *const val = m_LLVMBuilder.CreateAShr(left, right);
					return {.value = val, .type = leftType, .isTemp = true};
				}
				UNREACHABLE();

			default: UNREACHABLE();
		}
	}

	ExprResult visit(const ast::FuncCall &n) override {
		const auto &[callee, type, isTemp] = lowerExpr(*n.expr);
		VERIFY(type->isTypeKind(type::TypeKind::Function));
		const auto funcType = std::static_pointer_cast<const type::FunctionType>(type);

		Vec<llvm::Value *> args;
		args.reserve(n.args.size());

		for (const auto &arg : n.args) {
			const auto &[resValue, resType, resIsTemp] = lowerExpr(*arg);

			if (resIsTemp) {
				removeFromExprCleanup(resValue);
				args.push_back(resValue);
			} else {
				args.push_back(m_LoweringContext.copyValue(resValue, resType));
			}
		}
		auto *llvmFuncType = static_cast<llvm::FunctionType *>(m_Converter.convertType(type));

		const auto &call = m_LLVMBuilder.CreateCall(llvmFuncType, callee, args);

		addToExprCleanup(call, funcType->returnType);

		return {.value = call, .type = n.inferredType.value(), .isTemp = true};
	}

	ExprResult visit(const ast::Assignment &n) override {
		const auto &[leftLValue, leftType, isLeftTemp] = lowerLValue(*n.left);
		const auto &[right, rightType, isRightTemp] = lowerExpr(*n.right);
		const auto &llvmLeftType = m_Converter.convertType(n.left->inferredType.value());
		const auto &left = m_LLVMBuilder.CreateLoad(llvmLeftType, leftLValue);

		// First retain the right side
		if (isRightTemp) {
			removeFromExprCleanup(right);
		} else {
			m_LoweringContext.copyValue(right, rightType);
		}

		// If left is temp the expression cleanup will take care of it
		if (!isLeftTemp) {
			m_LoweringContext.dropValue(left, leftType);
		}

		if (n.assignmentKind == AssignmentKind::Simple) {
			m_LLVMBuilder.CreateStore(right, leftLValue);
			return {.value = llvm::ConstantInt::getNullValue(m_LLVMBuilder.getInt8Ty()),
					.type = leftType,
					.isTemp = true};
		}

		llvm::Value *res = nullptr;

		using enum AssignmentKind;
		switch (n.assignmentKind) {
			case Addition:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateAdd(left, right);
					break;
				}
				UNREACHABLE();

			case Subtraction:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateSub(left, right);
					break;
				}
				UNREACHABLE();

			case Multiplication:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateMul(left, right);
					break;
				}
				UNREACHABLE();

			case Division:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateSDiv(left, right);
					break;
				}
				UNREACHABLE();

			case Modulo:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateSRem(left, right);
					break;
				}
				UNREACHABLE();

			case BitwiseAnd:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateAnd(left, right);
					break;
				}
				UNREACHABLE();

			case BitwiseOr:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateOr(left, right);
					break;
				}
				UNREACHABLE();

			case BitwiseXor:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateXor(left, right);
					break;
				}
				UNREACHABLE();

			case LeftShift:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateShl(left, right);
					break;
				}
				UNREACHABLE();

			case RightShift:
				if (*leftType == type::Typename(u8"i32")) {
					res = m_LLVMBuilder.CreateAShr(left, right);
					break;
				}
				UNREACHABLE();

			default: UNREACHABLE();
		}

		const auto &store = m_LLVMBuilder.CreateStore(res, leftLValue);
		return {.value = llvm::ConstantInt::getNullValue(m_LLVMBuilder.getInt8Ty()),
				.type = leftType,
				.isTemp = true};
	}
};
