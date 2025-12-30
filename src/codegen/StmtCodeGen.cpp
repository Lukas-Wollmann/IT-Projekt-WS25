#include "StmtCodeGen.h"

#include <llvm/IR/Verifier.h>

#include "CodeGenContext.h"

namespace codegen {
	using namespace type;
	using namespace ast;

	StmtCodeGen::StmtCodeGen(CodeGenContext &ctx, RValueCodeGen &rValueCodeGen,
							 LValueCodeGen &lValueCodeGen)
		: m_Context(ctx)
		, m_RValueCodeGen(rValueCodeGen)
		, m_LValueCodeGen(lValueCodeGen) {}

	void StmtCodeGen::visit(const Assignment &n) {
		auto left = m_LValueCodeGen.dispatch(*n.left);
		auto right = m_RValueCodeGen.dispatch(*n.right);

		m_Context.getIRBuilder().CreateStore(right, left);
	}

	void StmtCodeGen::visit(const BlockStmt &n) {
		auto copyAllocas = m_Context.getAllocas();

		for (auto &stmt : n.stmts) {
			if (m_Context.getIRBuilder().GetInsertBlock()->getTerminator())
				break;

			dispatch(*stmt);
		}

		m_Context.setAllocas(std::move(copyAllocas));
	}

	void StmtCodeGen::visit(const IfStmt &n) {
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

	void StmtCodeGen::visit(const WhileStmt &n) {
		auto &llvmContext = m_Context.getLLVMContext();
		auto &builder = m_Context.getIRBuilder();

		auto func = builder.GetInsertBlock()->getParent();
		VERIFY(func);

		auto condBlock = llvm::BasicBlock::Create(llvmContext, "cond", func);
		auto bodyBlock = llvm::BasicBlock::Create(llvmContext, "body", func);
		auto mergeBlock = llvm::BasicBlock::Create(llvmContext, "merge", func);

		builder.CreateBr(condBlock);
		builder.SetInsertPoint(condBlock);

		auto cond = m_RValueCodeGen.dispatch(*n.cond);

		builder.CreateCondBr(cond, bodyBlock, mergeBlock);
		builder.SetInsertPoint(bodyBlock);

		dispatch(*n.body);

		if (!bodyBlock->getTerminator())
			builder.CreateBr(condBlock);

		builder.SetInsertPoint(mergeBlock);
	}

	void StmtCodeGen::visit(const ReturnStmt &n) {
		auto &builder = m_Context.getIRBuilder();

		if (n.expr.has_value()) {
			auto returnValue = m_RValueCodeGen.dispatch(*n.expr.value());
			builder.CreateRet(returnValue);
		} else {
			// We need to return a Unit,
			auto unitType = llvm::Type::getInt1Ty(m_Context.getLLVMContext());
			builder.CreateRet(llvm::ConstantInt::get(unitType, 0));
		}
	}

	void StmtCodeGen::visit(const VarDef &n) {
		auto type = m_Context.convertType(n.type);

		auto alloca = m_Context.createAlloca(type, n.ident);

		auto varValue = m_RValueCodeGen.dispatch(*n.value);
		m_Context.getIRBuilder().CreateStore(varValue, alloca);
	}

	void StmtCodeGen::visit(const FuncDecl &n) {
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

	void StmtCodeGen::visit(const Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);
	}
}