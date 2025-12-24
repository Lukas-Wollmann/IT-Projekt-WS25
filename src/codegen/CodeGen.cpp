#include "CodeGen.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "TypeConverter.h"
#include "type/CompareVisitor.h"

namespace codegen {
	using namespace type;
	using namespace ast;

	struct CodeGen::Impl {
	public:
		llvm::LLVMContext context;
		Box<llvm::Module> module;
		llvm::IRBuilder<> builder;
		TypeConverter converter;
		std::unordered_map<std::string, llvm::AllocaInst *> allocas;
		llvm::Value *lastValue;

		Impl(const std::string &moduleName)
			: module(std::make_unique<llvm::Module>(moduleName, context))
			, builder(context)
			, converter(context)
			, lastValue(nullptr) {}
	};

	CodeGen::CodeGen(std::ostream &os)
		: m_Impl(std::make_unique<Impl>("module"))
		, m_OStream(os) {}

	CodeGen::~CodeGen() {}

	void CodeGen::visit(const IntLit &n) {
		auto type = llvm::Type::getInt32Ty(m_Impl->context);
		m_Impl->lastValue = llvm::ConstantInt::get(type, n.value);
	}

	void CodeGen::visit(const FloatLit &n) {
		auto type = llvm::Type::getFloatTy(m_Impl->context);
		m_Impl->lastValue = llvm::ConstantFP::get(type, n.value);
	}

	void CodeGen::visit(const BoolLit &n) {
		auto type = llvm::Type::getInt1Ty(m_Impl->context);
		m_Impl->lastValue = llvm::ConstantInt::get(type, n.value);
	}

	void CodeGen::visit(const UnitLit &n) {
		auto type = llvm::Type::getInt1Ty(m_Impl->context);
		m_Impl->lastValue = llvm::ConstantInt::get(type, 0);
	}

	void CodeGen::visit(const CharLit &n) {
		auto type = llvm::Type::getInt8Ty(m_Impl->context);
		m_Impl->lastValue = llvm::ConstantInt::get(type, n.value);
	}

	void CodeGen::visit(const ast::Assignment &n) {
		auto target = getPointerForLValue(*n.left);

		dispatch(*n.right);
		auto right = m_Impl->lastValue;

		m_Impl->builder.CreateStore(right, target);
	}

	void CodeGen::visit(const VarRef &n) {
		auto ident = n.ident.asAscii();
		auto alloca = m_Impl->allocas.at(ident);
		auto type = alloca->getAllocatedType();

		m_Impl->lastValue = m_Impl->builder.CreateLoad(type, alloca, ident);
	}

	void CodeGen::visit(const BlockStmt &n) {
		auto copyScope = m_Impl->allocas;

		for (auto &stmt : n.stmts) {
			if (m_Impl->builder.GetInsertBlock()->getTerminator())
				break;

			dispatch(*stmt);
		}

		m_Impl->allocas = std::move(copyScope);
	}

	void CodeGen::visit(const IfStmt &n) {
		dispatch(*n.cond);
		auto cond = m_Impl->lastValue;
		auto func = m_Impl->builder.GetInsertBlock()->getParent();

		VERIFY(func);

		auto then = llvm::BasicBlock::Create(m_Impl->context, "then", func);
		auto else_ = llvm::BasicBlock::Create(m_Impl->context, "else", func);
		auto merge = llvm::BasicBlock::Create(m_Impl->context, "merge", func);

		m_Impl->builder.CreateCondBr(cond, then, else_);

		m_Impl->builder.SetInsertPoint(then);
		dispatch(*n.then);

		if (!then->getTerminator())
			m_Impl->builder.CreateBr(merge);

		m_Impl->builder.SetInsertPoint(else_);
		dispatch(*n.else_);

		if (!else_->getTerminator())
			m_Impl->builder.CreateBr(merge);

		m_Impl->builder.SetInsertPoint(merge);
		m_Impl->lastValue = nullptr;
	}

	void CodeGen::visit(const UnaryExpr &n) {
		dispatch(*n.operand);
		auto value = m_Impl->lastValue;
		auto type = n.inferredType.value();

		auto &builder = m_Impl->builder;

		using enum UnaryOpKind;

		switch (n.op) {
			case Positive: m_Impl->lastValue = value; return;

			case Negative:
				if (*type == Typename(u8"i32"))
					builder.CreateNeg(value);
				else if (*type == Typename(u8"f32"))
					builder.CreateFNeg(value);
				else
					UNREACHABLE();
				return;

			case Not:
				if (*type == Typename(u8"bool"))
					m_Impl->lastValue = builder.CreateNot(value);
				else if (*type == Typename(u8"i32") || *type == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateNot(value);
				else
					UNREACHABLE();
				return;

			case Dereference: {
				VERIFY(type->isTypeKind(TypeKind::Pointer));

				auto ptrType = std::static_pointer_cast<const PointerType>(type);
				auto pointeeType = m_Impl->converter.dispatch(*ptrType->pointeeType);

				m_Impl->lastValue = m_Impl->builder.CreateLoad(pointeeType, value, "deref");
				return;
			}

			default: UNREACHABLE();
		}
	}

	void CodeGen::visit(const BinaryExpr &n) {
		dispatch(*n.left);
		auto leftValue = m_Impl->lastValue;
		auto leftType = n.left->inferredType.value();

		dispatch(*n.right);
		auto rightValue = m_Impl->lastValue;
		auto rightType = n.right->inferredType.value();

		auto &builder = m_Impl->builder;

		using enum BinaryOpKind;

		switch (n.op) {
			case Addition:
				if (*leftType == Typename(u8"i32"))
					m_Impl->lastValue = builder.CreateAdd(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateAdd(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFAdd(leftValue, rightValue);
				else if (*leftType == Typename(u8"string"))
					UNREACHABLE(); // TODO
				else
					UNREACHABLE();
				return;

			case Subtraction:
				if (*leftType == Typename(u8"i32") || *leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateSub(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFSub(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			case Multiplication:
				if (*leftType == Typename(u8"i32") || *leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateMul(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFMul(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			case Division:
				if (*leftType == Typename(u8"i32"))
					m_Impl->lastValue = builder.CreateSDiv(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateUDiv(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFDiv(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			case Modulo:
				if (*leftType == Typename(u8"i32"))
					m_Impl->lastValue = builder.CreateSRem(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateURem(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			case Equality:
				if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFCmpOEQ(leftValue, rightValue);
				else
					m_Impl->lastValue = builder.CreateICmpEQ(leftValue, rightValue);
				return;

			case Inequality:
				if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFCmpONE(leftValue, rightValue);
				else
					m_Impl->lastValue = builder.CreateICmpNE(leftValue, rightValue);
				return;

			case LessThan:
				if (*leftType == Typename(u8"i32") || *leftType == Typename(u8"char"))
					m_Impl->lastValue = builder.CreateICmpSLT(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateICmpULT(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFCmpOLT(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

            case LessThanOrEqual:
				if (*leftType == Typename(u8"i32") || *leftType == Typename(u8"char"))
					m_Impl->lastValue = builder.CreateICmpSLE(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateICmpULE(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFCmpOLE(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			case GreaterThan:
				if (*leftType == Typename(u8"i32") || *leftType == Typename(u8"char"))
					m_Impl->lastValue = builder.CreateICmpSGT(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateICmpUGT(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFCmpOGT(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			case GreaterThanOrEqual:
				if (*leftType == Typename(u8"i32") || *leftType == Typename(u8"char"))
					m_Impl->lastValue = builder.CreateICmpSGE(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateICmpUGE(leftValue, rightValue);
				else if (*leftType == Typename(u8"f32"))
					m_Impl->lastValue = builder.CreateFCmpOGE(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			case LogicalAnd: m_Impl->lastValue = builder.CreateAnd(leftValue, rightValue); return;

			case LogicalOr:	 m_Impl->lastValue = builder.CreateOr(leftValue, rightValue); return;

			case BitwiseAnd: m_Impl->lastValue = builder.CreateAnd(leftValue, rightValue); return;

			case BitwiseOr:	 m_Impl->lastValue = builder.CreateOr(leftValue, rightValue); return;

			case BitwiseXor: m_Impl->lastValue = builder.CreateXor(leftValue, rightValue); return;

			case LeftShift:	 m_Impl->lastValue = builder.CreateShl(leftValue, rightValue); return;

			case RightShift:
				if (*leftType == Typename(u8"i32"))
					m_Impl->lastValue = builder.CreateAShr(leftValue, rightValue);
				else if (*leftType == Typename(u8"u32"))
					m_Impl->lastValue = builder.CreateLShr(leftValue, rightValue);
				else
					UNREACHABLE();
				return;

			default: UNREACHABLE();
		}
	}

	void CodeGen::visit(const WhileStmt &n) {
		auto func = m_Impl->builder.GetInsertBlock()->getParent();
		VERIFY(func);

		auto cond = llvm::BasicBlock::Create(m_Impl->context, "cond", func);
		auto body = llvm::BasicBlock::Create(m_Impl->context, "body", func);
		auto merge = llvm::BasicBlock::Create(m_Impl->context, "merge", func);

		m_Impl->builder.CreateBr(cond);

		m_Impl->builder.SetInsertPoint(cond);
		dispatch(*n.cond);

		m_Impl->builder.CreateCondBr(m_Impl->lastValue, body, merge);

		m_Impl->builder.SetInsertPoint(body);
		dispatch(*n.body);

		if (!body->getTerminator())
			m_Impl->builder.CreateBr(cond);

		m_Impl->builder.SetInsertPoint(merge);
		m_Impl->lastValue = nullptr;
	}

	void CodeGen::visit(const ReturnStmt &n) {
		if (n.expr.has_value()) {
			dispatch(*n.expr.value());
			m_Impl->builder.CreateRet(m_Impl->lastValue);
		} else {
			// We need to return a Unit
			auto unitType = llvm::Type::getInt1Ty(m_Impl->context);
			m_Impl->builder.CreateRet(llvm::ConstantInt::get(unitType, 0));
		}

		m_Impl->lastValue = nullptr;
	}

	void CodeGen::visit(const VarDef &n) {
		auto type = m_Impl->converter.dispatch(*n.type);
		auto ident = n.ident.asAscii();
		auto alloca = m_Impl->builder.CreateAlloca(type, nullptr, ident);

		m_Impl->allocas[ident] = alloca;

		dispatch(*n.value);
		m_Impl->builder.CreateStore(m_Impl->lastValue, alloca);
	}

	void CodeGen::visit(const FuncDecl &n) {
		Vec<llvm::Type *> argTypes;
		argTypes.reserve(n.params.size());

		for (auto param : n.params)
			argTypes.push_back(m_Impl->converter.dispatch(*param.second));

		auto returnType = m_Impl->converter.dispatch(*n.returnType);
		auto funcType = llvm::FunctionType::get(returnType, argTypes, false);
		auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
										   n.ident.asAscii(), m_Impl->module.get());
		auto entry = llvm::BasicBlock::Create(m_Impl->context, "entry", func);

		m_Impl->builder.SetInsertPoint(entry);
		m_Impl->allocas.clear();

		unsigned idx = 0;
		for (auto &arg : func->args()) {
			arg.setName(n.params[idx].first.asAscii());

			auto alloca = m_Impl->builder.CreateAlloca(arg.getType(), nullptr, arg.getName());

			m_Impl->builder.CreateStore(&arg, alloca);
			m_Impl->allocas[arg.getName().str()] = alloca;
			++idx;
		}

		dispatch(*n.body);
		llvm::verifyFunction(*func);
	}

	void CodeGen::visit(const Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);

		llvm::raw_os_ostream llvmOS(m_OStream);
		m_Impl->module->print(llvmOS, nullptr);
	}

	llvm::Value *CodeGen::getPointerForLValue(const Expr &lhs) {
		if (auto varRef = dynamic_cast<const VarRef *>(&lhs)) {
			auto ident = varRef->ident.asAscii();
			return m_Impl->allocas.at(ident);
		} else if (auto deref = dynamic_cast<const UnaryExpr *>(&lhs)) {
			if (deref->op == ast::UnaryOpKind::Dereference) {
				dispatch(*deref->operand);
				return m_Impl->lastValue;
			}
		}

		UNREACHABLE();
	}
}