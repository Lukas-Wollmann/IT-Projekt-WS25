#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <algorithm>

#include "ast/Visitor.h"
#include "codegen/TypeConverter.h"
#include "type/Compare.h"

struct TrackedValue {
	llvm::Value *value;
	type::TypePtr type;
};

struct LoweringContext {
private:
	llvm::LLVMContext m_LLVMContext;
	Box<llvm::Module> m_LLVMModule;
	llvm::IRBuilder<> m_IRBuilder;
	gen::TypeConverter m_Converter;
	Map<U8String, TrackedValue> m_Allocations;

public:
	explicit LoweringContext(const U8String &moduleName)
		: m_LLVMContext()
		, m_LLVMModule(std::make_unique<llvm::Module>(moduleName.asAscii(), m_LLVMContext))
		, m_IRBuilder(m_LLVMContext)
		, m_Converter(m_LLVMContext) {}

	llvm::IRBuilder<> &getIRBuilder() {
		return m_IRBuilder;
	}

	llvm::LLVMContext &getLLVMContext() {
		return m_LLVMContext;
	}

	llvm::Module &getLLVMModule() const {
		return *m_LLVMModule;
	}

	gen::TypeConverter &getTypeConverter() {
		return m_Converter;
	}

	void addAlloca(const U8String &ident, const TrackedValue &value) {
		m_Allocations.emplace(ident, value);
	}

	Opt<TrackedValue> getAlloca(const U8String &ident) {
		const auto iter = m_Allocations.find(ident);

		if (iter != m_Allocations.end()) {
			return iter->second;
		}
		return {};
	}
};

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
			if (auto dtor = getDtor(type)) {
				m_LLVMBuilder.CreateCall(getDtorType(), dtor.value(), {value});
			}
		}
	}

	void removeFromExprCleanup(llvm::Value *value) {
		const auto cond = [value](const TrackedValue &tracked) { return tracked.value == value; };
		const auto [first, last] = std::ranges::remove_if(m_ExprCleanup, cond);
		m_ExprCleanup.erase(first, last);
	}

	// private:
	llvm::Value *copyValue(llvm::Value *value, const type::TypePtr &type) const {
		if (type->isTypeKind(type::TypeKind::Unit)) {
			return value;
		}

		if (type->isTypeKind(type::TypeKind::Typename)) {
			return value; // TODO emit copy ctor
		}

		if (type->isTypeKind(type::TypeKind::Pointer)) {
			// For pointers, we need to increment the reference count
			auto *const func = m_LLVMModule.getFunction("__sp_copy");
			VERIFY(func);
			auto *const result = m_LLVMBuilder.CreateCall(func, {value});

			return result;
		}

		UNREACHABLE();
	}

	[[nodiscard]] constexpr llvm::FunctionType *getDtorType() const {
		llvm::Type *voidType = llvm::Type::getVoidTy(m_LLVMContext);
		auto *const i8Type = llvm::Type::getInt8Ty(m_LLVMContext);
		auto *const voidPtrType = llvm::PointerType::getUnqual(i8Type); // i8*
		return llvm::FunctionType::get(voidType, {voidPtrType}, false);
	}

	[[nodiscard]] constexpr llvm::Value *getNullDtor() const {
		auto *functionType = getDtorType();
		auto *functionPtrType = llvm::PointerType::getUnqual(functionType);
		return llvm::ConstantPointerNull::get(functionPtrType);
	}

	[[nodiscard]] Opt<llvm::Value *> getDtor(const type::TypePtr &type) const {
		if (type->isTypeKind(type::TypeKind::Unit)) {
			return {};
		}

		if (type->isTypeKind(type::TypeKind::Typename)) {
			return {}; // TODO recursive for structs / emit dtor
		}

		if (type->isTypeKind(type::TypeKind::Pointer)) {
			auto *const dtor = m_LLVMModule.getFunction("__sp_drop");
			VERIFY(dtor);
			return dtor;
		}

		UNREACHABLE();
	}

	[[nodiscard]] llvm::Value *sizeOf(const type::TypePtr &type) const {
		auto *const llvmType = m_Converter.convertType(type);
		const auto &layout = m_LLVMModule.getDataLayout();
		auto *const sizeType = layout.getIntPtrType(m_LLVMContext);
		const auto size = layout.getTypeAllocSize(llvmType);

		return llvm::ConstantInt::get(sizeType, size);
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
			copyValue(value, type);
		}

		// Create a new shared pointer via the runtime
		auto *const func = m_LLVMModule.getFunction("__sp_create");
		VERIFY(func);
		auto *const size = sizeOf(type);
		auto *const dtor = getDtor(type).value_or(getNullDtor());
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

		Vec<llvm::Value *> args;
		args.reserve(n.args.size());

		for (const auto &arg : n.args) {
			const auto &[resValue, resType, resIsTemp] = lowerExpr(*arg);

			if (resIsTemp) {
				removeFromExprCleanup(resValue);
				args.push_back(resValue);
			} else {
				args.push_back(copyValue(resValue, resType));
			}
		}
		auto *funcType = static_cast<llvm::FunctionType *>(m_Converter.convertType(type));

		const auto &call = m_LLVMBuilder.CreateCall(funcType, callee, args);
		return {.value = call, .type = n.inferredType.value(), .isTemp = true};
	}

	ExprResult visit(const ast::Assignment &n) override {
		const auto &[leftLValue, leftType] = lowerLValue(*n.left);
		const auto &[right, rightType, isRightTemp] = lowerExpr(*n.right);
		const auto &llvmLeftType = m_Converter.convertType(n.left->inferredType.value());

		if (n.assignmentKind == AssignmentKind::Simple) {
			const auto &store = m_LLVMBuilder.CreateStore(right, leftLValue);
			return {.value = store,
					.type = leftType,
					.isTemp = true}; // TODO test and change TypeChecker
		}

		llvm::Value *res = nullptr;
		const auto &left = m_LLVMBuilder.CreateLoad(llvmLeftType, leftLValue);

		using enum AssignmentKind;
		switch (n.assignmentKind) { // TODO drop Pointers
			case Addition:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateAdd(left, right);
				else
					UNREACHABLE();
				break;

			case Subtraction:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateSub(left, right);
				else
					UNREACHABLE();
				break;

			case Multiplication:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateMul(left, right);
				else
					UNREACHABLE();
				break;

			case Division:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateSDiv(left, right);
				else
					UNREACHABLE();
				break;

			case Modulo:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateSRem(left, right);
				else
					UNREACHABLE();
				break;

			case BitwiseAnd:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateAnd(left, right);
				else
					UNREACHABLE();
				break;
			case BitwiseOr:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateOr(left, right);
				else
					UNREACHABLE();
				break;
			case BitwiseXor:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateXor(left, right);
				else
					UNREACHABLE();
				break;
			case LeftShift:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateShl(left, right);
				else
					UNREACHABLE();
				break;
			case RightShift:
				if (*leftType == type::Typename(u8"i32"))
					res = m_LLVMBuilder.CreateAShr(left, right);
				else
					UNREACHABLE();
				break;

			default: UNREACHABLE();
		}
		const auto &store = m_LLVMBuilder.CreateStore(res, leftLValue);
		return {.value = store,
				.type = leftType,
				.isTemp = true}; // TODO test and change TypeChecker
	}

	TrackedValue lowerLValue(const ast::Expr &n) {
		switch (n.kind) {
			case ast::NodeKind::VarRef: {
				const auto &varRef = static_cast<const ast::VarRef &>(n);
				const auto reg = m_LoweringContext.getAlloca(varRef.ident);

				VERIFY(reg.has_value());

				return reg.value();
			}
			case ast::NodeKind::UnaryExpr: {
				const auto &unaryExpr = static_cast<const ast::UnaryExpr &>(n);

				VERIFY(unaryExpr.op == UnaryOpKind::Dereference);

				const auto expr = lowerExpr(*unaryExpr.operand);

				return {.value = expr.value, .type = expr.type};
			}
			default: UNREACHABLE();
		}
	}
};
