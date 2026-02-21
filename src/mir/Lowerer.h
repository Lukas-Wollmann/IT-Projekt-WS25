#pragma once

#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_os_ostream.h>

#include "ExprLowerer.h"
#include "core/DefaultDecls.h"

struct Lowerer : ast::ConstVisitor<void> {
private:
	const ast::Module &m_Module;
	LoweringContext m_LoweringContext;
	gen::TypeConverter m_Converter;
	llvm::Module &m_LLVMModule;
	llvm::IRBuilder<> &m_Builder;
	llvm::LLVMContext &m_LLVMContext;

public:
	Lowerer(const ast::Module &n)
		: m_Module(n)
		, m_LoweringContext(n.name)
		, m_Converter(m_LoweringContext.getTypeConverter())
		, m_LLVMModule(m_LoweringContext.getLLVMModule())
		, m_Builder(m_LoweringContext.getIRBuilder())
		, m_LLVMContext(m_LoweringContext.getLLVMContext()) {
		// TODO: move this somewhere else
		llvm::Type *size_t_Ty = m_Builder.getIntPtrTy(m_LLVMModule.getDataLayout());
		llvm::Type *ptrTy = m_Builder.getPtrTy();
		llvm::Type *voidTy = m_Builder.getVoidTy();

		llvm::FunctionType *spCreateTy = llvm::FunctionType::get(ptrTy, {size_t_Ty, ptrTy}, false);
		llvm::FunctionType *spCopyTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
		llvm::FunctionType *spDropTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);

		m_LLVMModule.getOrInsertFunction("__sp_create", spCreateTy);
		m_LLVMModule.getOrInsertFunction("__sp_copy", spCopyTy);
		m_LLVMModule.getOrInsertFunction("__sp_drop", spDropTy);
	}

	static void lower(std::ostream &os, const ast::Module &module) {
		Lowerer lowerer(module);
		lowerer.dispatch(module);

		llvm::raw_os_ostream llvmOS(os);
		lowerer.m_LLVMModule.print(llvmOS, nullptr);
	}

	void lowerNode(const ast::Node &n) {
		// TODO: This is a bit hacky, we might want to refactor this later.
		// We would need a ExprStmt Node or smth like that in the future.

		if (dynamic_cast<const ast::Expr *>(&n)) {
			ExprLowerer exprLowerer(m_LoweringContext);
			exprLowerer.lowerExpr(static_cast<const ast::Expr &>(n));
			exprLowerer.emitExprCleanup();

			return;
		}

		dispatch(n);
	}

	void visit(const ast::Module &n) override {
		// Forward declare default function decls
		for (const auto &[name, type] : s_DefaultDecls) {
			auto funcType = static_cast<llvm::FunctionType *>(m_Converter.convertType(type));

			llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name.asAscii(),
								   m_LLVMModule);
		}

		// Forward declare user defined functions decls
		for (auto &decl : n.decls) {
			auto returnType = m_Converter.convertType(decl->returnType);

			Vec<llvm::Type *> argTypes;
			for (auto &param : decl->params) {
				argTypes.push_back(m_Converter.convertType(param.second));
			}

			auto funcType = llvm::FunctionType::get(returnType, argTypes, false);

			llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, decl->ident.asAscii(),
								   m_LLVMModule);
		}

		for (auto &d : n.decls) {
			lowerNode(*d);
		}
	}

	void visit(const ast::FuncDecl &n) override {
		const auto func = m_LLVMModule.getFunction(n.ident.asAscii());
		VERIFY(func);
		auto entry = llvm::BasicBlock::Create(m_LLVMContext, "entry", func);

		m_Builder.SetInsertPoint(entry);
		m_LoweringContext.clearAllocas();
		m_LoweringContext.openScope();

		u32 i = 0;
		for (auto &arg : func->args()) {
			const auto &name = n.params[i].first;
			const auto &type = n.params[i].second;
			auto alloca = m_LoweringContext.createAlloca(type, name);

			arg.setName(name.asAscii());
			m_Builder.CreateStore(&arg, alloca);

			++i;
		}

		lowerNode(*n.body);

		if (!m_Builder.GetInsertBlock()->getTerminator()) {
			m_LoweringContext.emitFullScopeCleanup();
			m_Builder.CreateRet(llvm::Constant::getNullValue(func->getReturnType()));
		}

		m_LoweringContext.closeScope();
		llvm::verifyFunction(*func);
	}

	void visit(const ast::BlockStmt &n) override {
		m_LoweringContext.openScope();

		for (auto &stmt : n.stmts) {
			if (m_Builder.GetInsertBlock()->getTerminator()) {
				break;
			}

			lowerNode(*stmt);
		}

		if (!m_Builder.GetInsertBlock()->getTerminator()) {
			m_LoweringContext.emitScopeCleanup();
		}

		m_LoweringContext.closeScope();
	}

	void visit(const ast::IfStmt &n) override {
		ExprLowerer exprLowerer(m_LoweringContext);

		const auto cond = exprLowerer.lowerExpr(*n.cond);
		exprLowerer.emitExprCleanup();

		auto func = m_Builder.GetInsertBlock()->getParent();
		VERIFY(func);

		auto then = llvm::BasicBlock::Create(m_LLVMContext, "then", func);
		auto else_ = llvm::BasicBlock::Create(m_LLVMContext, "else", func);
		auto merge = llvm::BasicBlock::Create(m_LLVMContext, "merge", func);

		m_Builder.CreateCondBr(cond.value, then, else_);
		m_Builder.SetInsertPoint(then);

		lowerNode(*n.then);

		if (!then->getTerminator()) {
			m_Builder.CreateBr(merge);
		}

		m_Builder.SetInsertPoint(else_);

		lowerNode(*n.else_);

		if (!else_->getTerminator()) {
			m_Builder.CreateBr(merge);
		}

		m_Builder.SetInsertPoint(merge);
	}

	void visit(const ast::WhileStmt &n) override {
		auto func = m_Builder.GetInsertBlock()->getParent();
		auto condBlock = llvm::BasicBlock::Create(m_LLVMContext, "while.cond", func);
		auto bodyBlock = llvm::BasicBlock::Create(m_LLVMContext, "while.body", func);
		auto afterBlock = llvm::BasicBlock::Create(m_LLVMContext, "while.after", func);

		m_Builder.CreateBr(condBlock);
		m_Builder.SetInsertPoint(condBlock);

		// Re-evaluate condition inside the condBlock!
		ExprLowerer exprLowerer(m_LoweringContext);
		const auto cond = exprLowerer.lowerExpr(*n.cond);
		exprLowerer.emitExprCleanup();

		m_Builder.CreateCondBr(cond.value, bodyBlock, afterBlock);
		m_Builder.SetInsertPoint(bodyBlock);

		lowerNode(*n.body);

		if (!m_Builder.GetInsertBlock()->getTerminator()) {
			m_Builder.CreateBr(condBlock);
		}

		m_Builder.SetInsertPoint(afterBlock);
	}

	void visit(const ast::ReturnStmt &n) override {
		ExprLowerer exprLowerer(m_LoweringContext);
		const auto [value, type, isTemp] = exprLowerer.lowerExpr(*n.expr);

		if (isTemp) {
			exprLowerer.removeFromExprCleanup(value);
		} else {
			m_LoweringContext.copyValue(value, type);
		}

		m_LoweringContext.emitFullScopeCleanup();
		m_Builder.CreateRet(value);
	}

	void visit(const ast::VarDef &n) override {
		auto alloca = m_LoweringContext.createAlloca(n.type, n.ident);

		ExprLowerer exprLowerer(m_LoweringContext);
		const auto [value, type, isTemp] = exprLowerer.lowerExpr(*n.value);

		if (isTemp) {
			exprLowerer.removeFromExprCleanup(value);
		} else {
			m_LoweringContext.copyValue(value, type);
		}

		m_Builder.CreateStore(value, alloca);
	}
};