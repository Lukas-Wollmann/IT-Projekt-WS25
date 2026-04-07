#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "ast/Visitor.h"
#include "codegen/TypeConverter.h"

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
	Map<U8String, TrackedValue> m_Allocas;

public:
	LoweringContext(const U8String &moduleName)
		: m_LLVMModule(std::make_unique<llvm::Module>(moduleName.asAscii(), m_LLVMContext))
		, m_IRBuilder(m_LLVMContext)
		, m_Converter(m_LLVMContext) {}

	llvm::IRBuilder<> &getIRBuilder() {
		return m_IRBuilder;
	}

	llvm::LLVMContext &getLLVMContext() {
		return m_LLVMContext;
	}

	llvm::Module &getLLVMModule() {
		return *m_LLVMModule;
	}

	gen::TypeConverter &getTypeConverter() {
		return m_Converter;
	}

	void addAlloca(const U8String &ident, TrackedValue value) {
		m_Allocas.emplace(ident, value);
	}

	Opt<TrackedValue> getAlloca(const U8String &ident) {
		const auto it = m_Allocas.find(ident);

		if (it != m_Allocas.end()) {
			return it->second;
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
	ExprLowerer(LoweringContext &ctx)
		: m_LoweringContext(ctx)
		, m_LLVMContext(ctx.getLLVMContext())
		, m_LLVMModule(ctx.getLLVMModule())
		, m_LLVMBuilder(ctx.getIRBuilder())
		, m_Converter(ctx.getTypeConverter()) {}

	ExprResult lowerExpr(const ast::Expr &n) {
		return dispatch(n);
	}

	void addToExprCleanup(llvm::Value *value, type::TypePtr type) {
		m_ExprCleanup.push_back({value, type});
	}

	void removeFromExprCleanup(llvm::Value *value) {
		const auto cond = [value](const TrackedValue &t) { return t.value == value; };
		const auto it = std::remove_if(m_ExprCleanup.begin(), m_ExprCleanup.end(), cond);
		m_ExprCleanup.erase(it);
	}

private:
	llvm::Value *copyValue(llvm::Value *value, type::TypePtr type) {
		if (type->isTypeKind(type::TypeKind::Unit))
			return value;

		if (type->isTypeKind(type::TypeKind::Typename))
			return value;

		if (type->isTypeKind(type::TypeKind::Pointer)) {
			// For pointers we need to increment the reference count
			const auto func = m_LLVMModule.getFunction("__sp_copy");
			VERIFY(func);
			const auto result = m_LLVMBuilder.CreateCall(func, {value});

			return result;
		}

		UNREACHABLE();
	}

	llvm::Value *getDtor(type::TypePtr type) {
		const auto intType = llvm::Type::getInt8Ty(m_LLVMContext);
		const auto voidPtrType = llvm::PointerType::getUnqual(intType);
		const auto nullVoidPtr = llvm::ConstantPointerNull::get(voidPtrType);

		if (type->isTypeKind(type::TypeKind::Unit))
			return nullVoidPtr;

		if (type->isTypeKind(type::TypeKind::Typename))
			return nullVoidPtr;

		if (type->isTypeKind(type::TypeKind::Pointer)) {
			const auto func = m_LLVMModule.getFunction("__sp_destroy");
			VERIFY(func);

			return llvm::ConstantExpr::getBitCast(func, voidPtrType);
		}

		UNREACHABLE();
	}

	llvm::Value *sizeOf(type::TypePtr type) {
		const auto llvmType = m_Converter.convertType(type);
		const auto sizeType = llvm::Type::getInt64Ty(m_LLVMContext);
		const auto &layout = m_LLVMModule.getDataLayout();
		const auto size = layout.getTypeAllocSize(llvmType);

		return llvm::ConstantInt::get(sizeType, size);
	}

	ExprResult visit(const ast::IntLit &n) override {
		const auto &type = n.inferredType.value();
		const auto llvmType = m_Converter.convertType(type);
		const auto value = llvm::ConstantInt::get(llvmType, n.value);

		return {value, type, true};
	}

	ExprResult visit(const ast::BoolLit &n) override {
		const auto &type = n.inferredType.value();
		const auto llvmType = m_Converter.convertType(type);
		const auto value = llvm::ConstantInt::get(llvmType, n.value);

		return {value, type, true};
	}

	ExprResult visit(const ast::CharLit &n) override {
		const auto &type = n.inferredType.value();
		const auto llvmType = m_Converter.convertType(type);
		const auto value = llvm::ConstantInt::get(llvmType, n.value);

		return {value, type, true};
	}

	ExprResult visit(const ast::UnitLit &n) override {
		const auto &type = n.inferredType.value();
		const auto llvmType = m_Converter.convertType(type);
		const auto value = llvm::ConstantInt::get(llvmType, 0);

		return {value, type, true};
	}

	ExprResult visit(const ast::HeapAlloc &n) override {
		const auto expr = lowerExpr(*n.expr);
		const auto &type = n.inferredType.value();

		// If the value is a temporary, we can just move it into the heap allocation.
		// If it's not a temporary, we need to copy it first so that the heap allocation
		// owns the data that was put inside it.
		if (expr.isTemp) {
			removeFromExprCleanup(expr.value);
		} else {
			copyValue(expr.value, expr.type);
		}

		// Create a new shared pointer on via the runtime
		const auto func = m_LLVMModule.getFunction("__sp_create");
		VERIFY(func);
		const auto size = sizeOf(expr.type);
		const auto dtor = getDtor(expr.type);
		const auto sp = m_LLVMBuilder.CreateCall(func, {size, dtor});

		// Store the value into the shared pointer
		const auto elemType = m_Converter.convertType(expr.type);
		const auto destPointerType = llvm::PointerType::getUnqual(elemType);
		const auto dest = m_LLVMBuilder.CreateBitCast(sp, destPointerType);
		m_LLVMBuilder.CreateStore(expr.value, dest);

		// Now the pointer is owned by the current expression, so we need to clean it up after
		// the expression is done. The pointer might still be moved by an outer node.
		addToExprCleanup(sp, type);

		return {sp, type, true};
	}

	ExprResult visit(const ast::VarRef &n) override {
		const auto &type = n.inferredType.value();
		const auto llvmType = m_Converter.convertType(type);

		if (const auto alloc = m_LoweringContext.getAlloca(n.ident)) {
			const auto ptrToValue = alloc.value().value;
			const auto value = m_LLVMBuilder.CreateLoad(llvmType, ptrToValue);

			// Return local variable as a borrow
			return {value, type, true};
		}

		if (const auto func = m_LLVMModule.getFunction(n.ident.asAscii())) {
			// Return a function pointer
			return {func, type, false};
		}

		UNREACHABLE();
	}

	ExprResult visit(const ast::UnaryExpr &n) override {
		const auto operand = lowerExpr(*n.operand);
		const auto &type = n.inferredType.value();

		switch (n.op) {
			case UnaryOpKind::Addition:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateAdd(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case Subtraction:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateSub(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case Multiplication:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateMul(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case Division:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateSDiv(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case Modulo:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateSRem(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case Equality:
				if (*leftType == Typename(u8"char")) {
					m_LastValue = builder.CreateICmpEQ(leftValue, rightValue);
					return;
				}
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateICmpEQ(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case Inequality:
				if (*leftType == Typename(u8"char")) {
					m_LastValue = builder.CreateICmpNE(leftValue, rightValue);
					return;
				}
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateICmpNE(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case LessThan:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateICmpSLT(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case LessThanOrEqual:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateICmpSLE(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case GreaterThan:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateICmpSGT(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case GreaterThanOrEqual:
				if (*leftType == Typename(u8"i32")) {
					m_LastValue = builder.CreateICmpSGE(leftValue, rightValue);
					return;
				}
				UNREACHABLE();

			case LogicalAnd: m_LastValue = builder.CreateAnd(leftValue, rightValue); return;

			case LogicalOr:	 m_LastValue = builder.CreateOr(leftValue, rightValue); return;

			case BitwiseAnd: m_LastValue = builder.CreateAnd(leftValue, rightValue); return;

			case BitwiseOr:	 m_LastValue = builder.CreateOr(leftValue, rightValue); return;

			case BitwiseXor: m_LastValue = builder.CreateXor(leftValue, rightValue); return;

			case LeftShift:	 m_LastValue = builder.CreateShl(leftValue, rightValue); return;

			case RightShift:
				if (*leftType == Typename(u8"i32"))
					m_LastValue = builder.CreateAShr(leftValue, rightValue);
				return;
				UNREACHABLE();

			default: UNREACHABLE();
		}

		return {dest, true, type};
	}

	ExprResult visit(const ast::BinaryExpr &n) override {}

	ExprResult visit(const ast::FuncCall &n) override {}

	ExprResult visit(const ast::Assignment &n) override {}
};