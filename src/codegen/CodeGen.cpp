#include "CodeGen.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>

#include "CodeGenContext.h"
#include "core/DefaultDecls.h"
#include "type/Compare.h"

namespace gen {
using namespace type;
using namespace ast;

void CodeGen::generate(std::ostream &os, const ast::Module &module) {
	CodeGenContext ctx(module.name);
	CodeGen gen(ctx);

	gen.dispatch(module);

	llvm::raw_os_ostream llvmOS(os);
	ctx.getLLVMModule().print(llvmOS, nullptr);
}

CodeGen::CodeGen(CodeGenContext &ctx)
	: m_Context(ctx)
	, m_LValueCodeGen(*this)
	, m_LastValue(nullptr) {}

void CodeGen::visit(const Assignment &n) {
	using enum AssignmentKind;

	auto left = m_LValueCodeGen.dispatch(*n.left);
	dispatch(*n.left);
	auto rvalueLeft = m_LastValue;

	dispatch(*n.right);
	auto right = m_LastValue;

	auto leftType = n.left->inferredType.value();
	auto &builder = m_Context.getIRBuilder();

	llvm::Value *res = nullptr;

	switch (n.assignmentKind) {
		case Simple: res = right; break;

		case Addition:
			if (*leftType == Typename(u8"i32"))
				res = builder.CreateAdd(rvalueLeft, right);
			else
				UNREACHABLE();
			break;

		case Subtraction:
			if (*leftType == Typename(u8"i32"))
				res = builder.CreateSub(rvalueLeft, right);
			else
				UNREACHABLE();
			break;

		case Multiplication:
			if (*leftType == Typename(u8"i32"))
				res = builder.CreateMul(rvalueLeft, right);
			else
				UNREACHABLE();
			break;

		case Division:
			if (*leftType == Typename(u8"i32"))
				res = builder.CreateSDiv(rvalueLeft, right);
			else
				UNREACHABLE();
			break;

		case Modulo:
			if (*leftType == Typename(u8"i32"))
				res = builder.CreateSRem(rvalueLeft, right);
			else
				UNREACHABLE();
			break;

		case BitwiseAnd:
		case BitwiseOr:
		case BitwiseXor:
		case LeftShift:
		case RightShift: UNREACHABLE(); // TODO: Implement this stuff

		default:		 UNREACHABLE();
	}

	builder.CreateStore(res, left);
}

void CodeGen::visit(const BlockStmt &n) {
	auto copyAllocas = m_Context.getAllocas();

	for (auto &stmt : n.stmts) {
		if (m_Context.getIRBuilder().GetInsertBlock()->getTerminator())
			break;

		dispatch(*stmt);
	}

	m_Context.setAllocas(std::move(copyAllocas));
}

void CodeGen::visit(const IfStmt &n) {
	auto &llvmContext = m_Context.getLLVMContext();
	auto &builder = m_Context.getIRBuilder();

	dispatch(*n.cond);
	auto cond = m_LastValue;
	auto func = builder.GetInsertBlock()->getParent();

	VERIFY(func);

	auto then = llvm::BasicBlock::Create(llvmContext, "then", func);
	auto else_ = llvm::BasicBlock::Create(llvmContext, "else", func);
	auto merge = llvm::BasicBlock::Create(llvmContext, "merge", func);

	builder.CreateCondBr(cond, then, else_);

	builder.SetInsertPoint(then);
	dispatch(*n.then);

	if (!then->getTerminator())
		builder.CreateBr(merge);

	builder.SetInsertPoint(else_);
	dispatch(*n.else_);

	if (!else_->getTerminator())
		builder.CreateBr(merge);

	builder.SetInsertPoint(merge);
}

void CodeGen::visit(const WhileStmt &n) {
	auto &llvmContext = m_Context.getLLVMContext();
	auto &builder = m_Context.getIRBuilder();

	auto func = builder.GetInsertBlock()->getParent();
	VERIFY(func);

	auto condBlock = llvm::BasicBlock::Create(llvmContext, "cond", func);
	auto bodyBlock = llvm::BasicBlock::Create(llvmContext, "body", func);
	auto afterBlock = llvm::BasicBlock::Create(llvmContext, "after", func);

	builder.CreateBr(condBlock);
	builder.SetInsertPoint(condBlock);

	dispatch(*n.cond);
	auto cond = m_LastValue;

	builder.CreateCondBr(cond, bodyBlock, afterBlock);
	builder.SetInsertPoint(bodyBlock);

	dispatch(*n.body);

	if (!builder.GetInsertBlock()->getTerminator())
		builder.CreateBr(condBlock);

	builder.SetInsertPoint(afterBlock);
}

void CodeGen::visit(const ReturnStmt &n) {
	auto &builder = m_Context.getIRBuilder();
	dispatch(*n.expr);
	auto returnValue = m_LastValue;
	builder.CreateRet(returnValue);
}

void CodeGen::visit(const VarDef &n) {
	auto type = m_Context.convertType(n.type);
	auto alloca = m_Context.createAlloca(type, n.ident);

	dispatch(*n.value);
	auto varValue = m_LastValue;
	m_Context.getIRBuilder().CreateStore(varValue, alloca);
}

void CodeGen::visit(const FuncDecl &n) {
	auto &builder = m_Context.getIRBuilder();

	llvm::Function *func = m_Context.getLLVMModule().getFunction(n.ident.asAscii());
	auto entry = llvm::BasicBlock::Create(m_Context.getLLVMContext(), "entry", func);

	builder.SetInsertPoint(entry);
	m_Context.setAllocas({});

	uint idx = 0;
	for (auto &arg : func->args()) {
		arg.setName(n.params[idx].first.asAscii());

		auto alloca = m_Context.createAlloca(arg.getType(), n.params[idx].first);
		builder.CreateStore(&arg, alloca);

		++idx;
	}

	dispatch(*n.body);

	if (!builder.GetInsertBlock()->getTerminator()) {
		builder.CreateRet(llvm::Constant::getNullValue(func->getReturnType()));
	}

	llvm::verifyFunction(*func);
}

void CodeGen::visit(const Module &n) {
	for (const auto &[name, type] : s_DefaultDecls) {
		auto returnType = m_Context.convertType(type->returnType);

		Vec<llvm::Type *> argTypes;
		for (auto &param : type->paramTypes)
			argTypes.push_back(m_Context.convertType(param));

		auto funcType = llvm::FunctionType::get(returnType, argTypes, false);

		llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name.asAscii(),
							   m_Context.getLLVMModule());
	}

	for (auto &decl : n.decls) {
		auto returnType = m_Context.convertType(decl->returnType);

		Vec<llvm::Type *> argTypes;
		for (auto &param : decl->params)
			argTypes.push_back(m_Context.convertType(param.second));

		auto funcType = llvm::FunctionType::get(returnType, argTypes, false);

		llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, decl->ident.asAscii(),
							   m_Context.getLLVMModule());
	}

	for (auto &d : n.decls)
		dispatch(*d);
}

void CodeGen::visit(const IntLit &n) {
	auto type = llvm::Type::getInt32Ty(m_Context.getLLVMContext());
	m_LastValue = llvm::ConstantInt::get(type, n.value);
}

void CodeGen::visit(const BoolLit &n) {
	auto type = llvm::Type::getInt1Ty(m_Context.getLLVMContext());
	m_LastValue = llvm::ConstantInt::get(type, n.value);
}

void CodeGen::visit(const UnitLit &n) {
	auto type = llvm::Type::getInt1Ty(m_Context.getLLVMContext());
	m_LastValue = llvm::ConstantInt::get(type, 0);
}

void CodeGen::visit(const CharLit &n) {
	auto type = llvm::Type::getInt32Ty(m_Context.getLLVMContext());
	m_LastValue = llvm::ConstantInt::get(type, n.value);
}

void CodeGen::visit(const UnaryExpr &n) {
	dispatch(*n.operand);
	auto value = m_LastValue;
	auto type = n.inferredType.value();
	auto &builder = m_Context.getIRBuilder();

	using enum UnaryOpKind;

	switch (n.op) {
		case Positive: m_LastValue = value; return;

		case Negative:
			if (*type == Typename(u8"i32")) {
				m_LastValue = builder.CreateNeg(value);
				return;
			}
			UNREACHABLE();

		case LogicalNot:
			if (*type == Typename(u8"bool")) {
				m_LastValue = builder.CreateNot(value);
				return;
			}
			UNREACHABLE();

		case BitwiseNot:
			if (*type == Typename(u8"i32")) {
				m_LastValue = builder.CreateNot(value);
				return;
			}
			UNREACHABLE();

		case Dereference: {
			VERIFY(type->isTypeKind(TypeKind::Pointer));

			auto ptrType = std::static_pointer_cast<const PointerType>(type);
			auto pointeeType = m_Context.convertType(ptrType->pointeeType);

			m_LastValue = builder.CreateLoad(pointeeType, value);
			return;
		}

		default: UNREACHABLE();
	}
}

void CodeGen::visit(const BinaryExpr &n) {
	dispatch(*n.left);
	auto leftValue = m_LastValue;
	auto leftType = n.left->inferredType.value();

	dispatch(*n.right);
	auto rightValue = m_LastValue;
	auto rightType = n.right->inferredType.value();
	auto &builder = m_Context.getIRBuilder();

	using enum BinaryOpKind;

	switch (n.op) {
		case Addition:
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
}

void CodeGen::visit(const ast::VarRef &n) {
	if (auto alloca = m_Context.getAlloca(n.ident)) {
		auto type = alloca->getAllocatedType();

		m_LastValue = m_Context.getIRBuilder().CreateLoad(type, alloca);
		return;
	}

	if (auto func = m_Context.getLLVMModule().getFunction(n.ident.asAscii())) {
		m_LastValue = func;
		return;
	}

	UNREACHABLE();
}

void CodeGen::visit(const ast::FuncCall &n) {
	auto &builder = m_Context.getIRBuilder();

	// 1. Get the signature (FunctionType)
	// This works regardless of how the function is called
	auto exprType = n.expr->inferredType.value();
	llvm::FunctionType *llvmFuncType =
			static_cast<llvm::FunctionType *>(m_Context.convertType(exprType));

	// 2. Get the callee Value* (the address to jump to)
	// We dispatch n.expr. If it's a VarRef to 'a', m_LastValue becomes @a.
	// If it's a VarRef to 'my_ptr', m_LastValue becomes the loaded pointer.
	dispatch(*n.expr);
	llvm::Value *callee = m_LastValue;

	// 3. Evaluate arguments
	Vec<llvm::Value *> args;
	args.reserve(n.args.size());
	for (auto &argExpr : n.args) {
		dispatch(*argExpr);
		args.push_back(m_LastValue);
	}

	// 4. Create the call
	// If callee is an llvm::Function, it's a direct call.
	// If callee is a loaded pointer, it's an indirect call.
	m_LastValue = builder.CreateCall(llvmFuncType, callee, args);
}
}