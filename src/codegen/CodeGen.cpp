#include "CodeGen.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>

#include "CodeGenContext.h"
#include "type/Compare.h"

namespace codegen {
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
		, m_RValueCodeGen(m_Context)
		, m_LValueCodeGen(m_Context) {}

	// a[1] =a[1] *  1;

	void CodeGen::visit(const Assignment &n) {
		using enum AssignmentKind;

		auto left = m_LValueCodeGen.dispatch(*n.left);
		auto rvalueLeft = m_RValueCodeGen.dispatch(*n.left);

		auto right = m_RValueCodeGen.dispatch(*n.right);

		auto leftType = n.left->inferredType.value();
		auto &builder = m_Context.getIRBuilder();

		llvm::Value *res = nullptr;

		switch (n.assignmentKind) {
			case Simple: res = left; break;

			case Addition:
				if (*leftType == Typename(u8"i32"))
					res = builder.CreateAdd(rvalueLeft, right);
				else if (*leftType == Typename(u8"u32"))
					res = builder.CreateAdd(rvalueLeft, right);
				else if (*leftType == Typename(u8"f32"))
					res = builder.CreateFAdd(rvalueLeft, right);
				else if (*leftType == Typename(u8"string"))
					UNREACHABLE(); // TODO: Implement string concatination here
				else
					UNREACHABLE();
				break;

			case Subtraction:
				if (*leftType == Typename(u8"i32"))
					res = builder.CreateSub(rvalueLeft, right);
				else if (*leftType == Typename(u8"u32"))
					res = builder.CreateSub(rvalueLeft, right);
				else if (*leftType == Typename(u8"f32"))
					res = builder.CreateFSub(rvalueLeft, right);
				else
					UNREACHABLE();
				break;

			case Multiplication:
				if (*leftType == Typename(u8"i32"))
					res = builder.CreateMul(rvalueLeft, right);
				else if (*leftType == Typename(u8"i32"))
					res = builder.CreateMul(rvalueLeft, right);
				else if (*leftType == Typename(u8"f32"))
					res = builder.CreateFMul(rvalueLeft, right);
				else
					UNREACHABLE();
				break;

			case Division:
				if (*leftType == Typename(u8"i32"))
					res = builder.CreateSDiv(rvalueLeft, right);
				else if (*leftType == Typename(u8"u32"))
					res = builder.CreateUDiv(rvalueLeft, right);
				else if (*leftType == Typename(u8"f32"))
					res = builder.CreateFDiv(rvalueLeft, right);
				else
					UNREACHABLE();
				break;

			case Modulo:
				if (*leftType == Typename(u8"i32"))
					res = builder.CreateSRem(rvalueLeft, right);
				else if (*leftType == Typename(u8"u32"))
					res = builder.CreateURem(rvalueLeft, right);
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

		auto cond = m_RValueCodeGen.dispatch(*n.cond);
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

		auto cond = m_RValueCodeGen.dispatch(*n.cond);

		builder.CreateCondBr(cond, bodyBlock, afterBlock);
		builder.SetInsertPoint(bodyBlock);

		dispatch(*n.body);

		if (!builder.GetInsertBlock()->getTerminator())
			builder.CreateBr(condBlock);

		builder.SetInsertPoint(afterBlock);
	}

	void CodeGen::visit(const ReturnStmt &n) {
		auto &builder = m_Context.getIRBuilder();
		auto returnValue = m_RValueCodeGen.dispatch(*n.expr);
		builder.CreateRet(returnValue);
	}

	void CodeGen::visit(const VarDef &n) {
		auto type = m_Context.convertType(n.type);

		auto alloca = m_Context.createAlloca(type, n.ident);

		auto varValue = m_RValueCodeGen.dispatch(*n.value);
		m_Context.getIRBuilder().CreateStore(varValue, alloca);
	}

	void CodeGen::visit(const FuncDecl &n) {
		auto &builder = m_Context.getIRBuilder();

		Vec<llvm::Type *> argTypes;
		argTypes.reserve(n.params.size());

		for (auto param : n.params)
			argTypes.push_back(m_Context.convertType(param.second));

		auto returnType = m_Context.convertType(n.returnType);
		auto funcType = llvm::FunctionType::get(returnType, argTypes, false);
		auto func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage,
										   n.ident.asAscii(), m_Context.getLLVMModule());
		auto entry = llvm::BasicBlock::Create(m_Context.getLLVMContext(), "entry", func);

		builder.SetInsertPoint(entry);
		m_Context.setAllocas({});

		unsigned idx = 0;
		for (auto &arg : func->args()) {
			arg.setName(n.params[idx].first.asAscii());

			auto alloca = m_Context.createAlloca(arg.getType(), n.params[idx].first);
			builder.CreateStore(&arg, alloca);

			++idx;
		}

		dispatch(*n.body);
		llvm::verifyFunction(*func);
	}

	void CodeGen::visit(const Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);
	}
}