#include "ExprCodeGen.h"

#include "CodeGenContext.h"
#include "type/CompareVisitor.h"

namespace codegen {
	using namespace ast;
	using namespace type;

	RValueCodeGen::RValueCodeGen(CodeGenContext &ctx)
		: m_Context(ctx) {}

	llvm::Value *RValueCodeGen::visit(const IntLit &n) {
		auto type = llvm::Type::getInt32Ty(m_Context.getLLVMContext());
		return llvm::ConstantInt::get(type, n.value);
	}

	llvm::Value *RValueCodeGen::visit(const FloatLit &n) {
		auto type = llvm::Type::getFloatTy(m_Context.getLLVMContext());
		return llvm::ConstantFP::get(type, n.value);
	}

	llvm::Value *RValueCodeGen::visit(const BoolLit &n) {
		auto type = llvm::Type::getInt1Ty(m_Context.getLLVMContext());
		return llvm::ConstantInt::get(type, n.value);
	}

	llvm::Value *RValueCodeGen::visit(const UnitLit &n) {
		auto type = llvm::Type::getInt1Ty(m_Context.getLLVMContext());
		return llvm::ConstantInt::get(type, 0);
	}

	llvm::Value *RValueCodeGen::visit(const CharLit &n) {
		auto type = llvm::Type::getInt32Ty(m_Context.getLLVMContext());
		return llvm::ConstantInt::get(type, n.value);
	}

	llvm::Value *RValueCodeGen::visit(const UnaryExpr &n) {
		auto value = dispatch(*n.operand);
		auto type = n.inferredType.value();
		auto &builder = m_Context.getIRBuilder();

		using enum UnaryOpKind;

		switch (n.op) {
			case Positive: return value;

			case Negative:
				if (*type == Typename(u8"i32"))
					return builder.CreateNeg(value);
				if (*type == Typename(u8"f32"))
					return builder.CreateFNeg(value);
				UNREACHABLE();

			case LogicalNot:
				if (*type == Typename(u8"bool"))
					return builder.CreateNot(value);
                UNREACHABLE();

            case BitwiseNot:
				if (*type == Typename(u8"i32"))
					return builder.CreateNot(value);
				if (*type == Typename(u8"u32"))
					return builder.CreateNot(value);
				UNREACHABLE();

			case Dereference: {
				VERIFY(type->isTypeKind(TypeKind::Pointer));

				auto ptrType = std::static_pointer_cast<const PointerType>(type);
				auto pointeeType = m_Context.convertType(ptrType->pointeeType);

				return builder.CreateLoad(pointeeType, value, "deref");
			}

			default: UNREACHABLE();
		}
	}

	llvm::Value *RValueCodeGen::visit(const BinaryExpr &n) {
		auto leftValue = dispatch(*n.left);
		auto leftType = n.left->inferredType.value();
		auto rightValue = dispatch(*n.right);
		auto rightType = n.right->inferredType.value();
		auto &builder = m_Context.getIRBuilder();

		using enum BinaryOpKind;

		switch (n.op) {
			case Addition:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateAdd(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateAdd(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFAdd(leftValue, rightValue);
				if (*leftType == Typename(u8"string"))
					UNREACHABLE(); // TODO: Implement string concatination here
				UNREACHABLE();

			case Subtraction:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateSub(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateSub(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFSub(leftValue, rightValue);
				UNREACHABLE();

			case Multiplication:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateMul(leftValue, rightValue);
				if (*leftType == Typename(u8"i32"))
					return builder.CreateMul(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFMul(leftValue, rightValue);
				UNREACHABLE();

			case Division:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateSDiv(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateUDiv(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFDiv(leftValue, rightValue);
				UNREACHABLE();

			case Modulo:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateSRem(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateURem(leftValue, rightValue);
				UNREACHABLE();

			case Equality:
				if (*leftType == Typename(u8"char"))
					return builder.CreateICmpEQ(leftValue, rightValue);
				if (*leftType == Typename(u8"i32"))
					return builder.CreateICmpEQ(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateICmpEQ(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFCmpOEQ(leftValue, rightValue);
				UNREACHABLE();

			case Inequality:
				if (*leftType == Typename(u8"char"))
					return builder.CreateICmpNE(leftValue, rightValue);
				if (*leftType == Typename(u8"i32"))
					return builder.CreateICmpNE(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateICmpNE(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFCmpONE(leftValue, rightValue);
				UNREACHABLE();

			case LessThan:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateICmpSLT(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateICmpULT(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFCmpOLT(leftValue, rightValue);
				UNREACHABLE();

			case LessThanOrEqual:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateICmpSLE(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateICmpULE(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFCmpOLE(leftValue, rightValue);
				UNREACHABLE();

			case GreaterThan:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateICmpSGT(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateICmpUGT(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFCmpOGT(leftValue, rightValue);
				UNREACHABLE();

			case GreaterThanOrEqual:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateICmpSGE(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateICmpUGE(leftValue, rightValue);
				if (*leftType == Typename(u8"f32"))
					return builder.CreateFCmpOGE(leftValue, rightValue);
				UNREACHABLE();

			case LogicalAnd: return builder.CreateAnd(leftValue, rightValue);

			case LogicalOr:	 return builder.CreateOr(leftValue, rightValue);

			case BitwiseAnd: return builder.CreateAnd(leftValue, rightValue);

			case BitwiseOr:	 return builder.CreateOr(leftValue, rightValue);

			case BitwiseXor: return builder.CreateXor(leftValue, rightValue);

			case LeftShift:	 return builder.CreateShl(leftValue, rightValue);

			case RightShift:
				if (*leftType == Typename(u8"i32"))
					return builder.CreateAShr(leftValue, rightValue);
				if (*leftType == Typename(u8"u32"))
					return builder.CreateLShr(leftValue, rightValue);
				UNREACHABLE();

			default: UNREACHABLE();
		}
	}

	llvm::Value *RValueCodeGen::visit(const VarRef &n) {
		auto alloca = m_Context.getAlloca(n.ident);
		auto type = alloca->getAllocatedType();

		return m_Context.getIRBuilder().CreateLoad(type, alloca);
	}

	LValueCodeGen::LValueCodeGen(CodeGenContext &ctx)
		: m_Context(ctx) {}

	llvm::Value *LValueCodeGen::visit(const VarRef &n) {
		return m_Context.getAlloca(n.ident);
	}

	llvm::Value *LValueCodeGen::visit(const UnaryExpr &n) {
		VERIFY(n.op == UnaryOpKind::Dereference);

		return RValueCodeGen(m_Context).dispatch(*n.operand);
	}
}
