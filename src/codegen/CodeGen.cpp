#include "CodeGen.h"

#include <fstream>

namespace gen {
CodeGen::CodeGen(CodeGenContext &ctx)
	: m_Context(ctx)
	, m_AllocManager(ctx) {}

void CodeGen::generate(std::ofstream &out, const ast::Module &n) {
	CodeGenContext ctx(n.name);
	CodeGen lowerer(ctx);

	lowerer.visitNode(n);

	llvm::raw_os_ostream llvmOS(out);
	ctx.llvmModule.print(llvmOS, nullptr);
}

void CodeGen::visitNode(const ast::Node &n) {
	// TODO: This is a bit hacky, we might want to refactor this later.
	// We would need a ExprStmt Node or smth like that in the future.

	if (dynamic_cast<const ast::Expr *>(&n)) {
		ExprLowerer exprLowerer(m_Context, m_AllocManager);
		exprLowerer.lowerExpr(static_cast<const ast::Expr &>(n));
		exprLowerer.emitExprCleanup();

		return;
	}

	dispatch(n);
}

void CodeGen::visit(const ast::Module &n) {
	// First pass: create opaque struct types
	for (const auto &structDecl : n.structs) {
		llvm::StructType::create(m_Context.llvmContext, structDecl->ident.asAscii());
	}

	// Second pass: set struct field types
	for (const auto &structDecl : n.structs) {
		auto *structType =
				llvm::StructType::getTypeByName(m_Context.llvmContext, structDecl->ident.asAscii());

		Vec<llvm::Type *> fieldTypes;
		for (const auto &[fieldName, fieldType] : structDecl->fields) {
			fieldTypes.push_back(m_Context.typeConverter.convert(fieldType));
		}

		structType->setBody(fieldTypes);
	}

	// Forward declare default function decls
	for (const auto &[name, type] : s_DefaultDecls) {
		Vec<llvm::Type *> paramTypes;
		paramTypes.reserve(type->paramTypes.size());
		for (auto *paramType : type->paramTypes) {
			paramTypes.push_back(m_Context.typeConverter.convert(paramType));
		}

		auto *returnType = m_Context.typeConverter.convert(type->returnType);
		auto *funcType = llvm::FunctionType::get(returnType, paramTypes, false);

		llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, name.asAscii(),
							   m_Context.llvmModule);

		// Forward declare struct destructors
		auto *structDtorType = m_Context.getDestructorType();
		for (const auto &decl : n.structs) {
			auto dtorName = getStructDtorName(decl->ident);
			if (!m_Context.llvmModule.getFunction(dtorName.asAscii())) {
				llvm::Function::Create(structDtorType, llvm::Function::ExternalLinkage,
									   dtorName.asAscii(), m_Context.llvmModule);
			}
		}
	}

	// Emit struct destructors
	for (const auto &decl : n.structs) {
		auto dtorName = getStructDtorName(decl->ident);
		auto *fn = m_Context.llvmModule.getFunction(dtorName.asAscii());
		VERIFY(fn && fn->empty());

		auto *entry = llvm::BasicBlock::Create(m_Context.llvmContext, "entry", fn);
		m_Context.irBuilder.SetInsertPoint(entry);

		auto *payload = fn->arg_begin();
		auto *llvmStructType =
				llvm::StructType::getTypeByName(m_Context.llvmContext, decl->ident.asAscii());

		for (u32 i = 0; i < decl->fields.size(); ++i) {
			const auto &[_, fieldType] = decl->fields[i];
			auto dtor = m_Context.getDestructor(fieldType);

			if (dtor.has_value()) {
				auto *fieldPtr = m_Context.irBuilder.CreateStructGEP(llvmStructType, payload, i);
				auto *voidPtr =
						m_Context.irBuilder.CreateBitCast(fieldPtr, m_Context.irBuilder.getPtrTy());
				m_Context.irBuilder.CreateCall(m_Context.getDestructorType(), dtor.value(),
											   {voidPtr});
			}
		}

		m_Context.irBuilder.CreateRetVoid();
	}

	// Forward declare user defined functions decls
	for (auto &decl : n.funcs) {
		auto returnType = m_Context.typeConverter.convert(decl->returnType);

		Vec<llvm::Type *> argTypes;
		for (auto &param : decl->params) {
			argTypes.push_back(m_Context.typeConverter.convert(param.second));
		}

		auto funcType = llvm::FunctionType::get(returnType, argTypes, false);

		llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, decl->ident.asAscii(),
							   m_Context.llvmModule);
	}

	for (auto &d : n.funcs) {
		visitNode(*d);
	}
}

void CodeGen::visit(const ast::FuncDecl &n) {
	const auto func = m_Context.llvmModule.getFunction(n.ident.asAscii());
	VERIFY(func);
	auto entry = llvm::BasicBlock::Create(m_Context.llvmContext, "entry", func);

	m_Context.irBuilder.SetInsertPoint(entry);
	m_AllocManager.clearAllocas();
	m_AllocManager.openScope();

	u32 i = 0;
	for (auto &arg : func->args()) {
		const auto &name = n.params[i].first;
		const auto &type = n.params[i].second;
		auto alloca = m_AllocManager.createAlloca(type, name);

		arg.setName(name.asAscii());
		m_Context.irBuilder.CreateStore(&arg, alloca);

		++i;
	}

	visitNode(*n.body);

	if (!m_Context.irBuilder.GetInsertBlock()->getTerminator()) {
		m_AllocManager.emitFullScopeCleanup();
		m_Context.irBuilder.CreateRet(llvm::Constant::getNullValue(func->getReturnType()));
	}

	m_AllocManager.closeScope();
	llvm::verifyFunction(*func);
}

void CodeGen::visit(const ast::BlockStmt &n) {
	m_AllocManager.openScope();

	for (auto &stmt : n.stmts) {
		if (m_Context.irBuilder.GetInsertBlock()->getTerminator()) {
			break;
		}

		visitNode(*stmt);
	}

	if (!m_Context.irBuilder.GetInsertBlock()->getTerminator()) {
		m_AllocManager.emitCurrentScopeCleanup();
	}

	m_AllocManager.closeScope();
}

void CodeGen::visit(const ast::IfStmt &n) {
	ExprLowerer exprLowerer(m_Context, m_AllocManager);

	const auto cond = exprLowerer.lowerExpr(*n.cond);
	exprLowerer.emitExprCleanup();

	auto func = m_Context.irBuilder.GetInsertBlock()->getParent();
	VERIFY(func);

	auto then = llvm::BasicBlock::Create(m_Context.llvmContext, "then", func);
	auto else_ = llvm::BasicBlock::Create(m_Context.llvmContext, "else", func);
	auto merge = llvm::BasicBlock::Create(m_Context.llvmContext, "merge", func);

	m_Context.irBuilder.CreateCondBr(cond.value, then, else_);
	m_Context.irBuilder.SetInsertPoint(then);

	visitNode(*n.then);

	auto *thenExit = m_Context.irBuilder.GetInsertBlock();
	if (!thenExit->getTerminator()) {
		m_Context.irBuilder.CreateBr(merge);
	}

	m_Context.irBuilder.SetInsertPoint(else_);

	visitNode(*n.else_);

	auto *elseExit = m_Context.irBuilder.GetInsertBlock();
	if (!elseExit->getTerminator()) {
		m_Context.irBuilder.CreateBr(merge);
	}

	m_Context.irBuilder.SetInsertPoint(merge);
}

void CodeGen::visit(const ast::WhileStmt &n) {
	auto func = m_Context.irBuilder.GetInsertBlock()->getParent();
	auto condBlock = llvm::BasicBlock::Create(m_Context.llvmContext, "while.cond", func);
	auto bodyBlock = llvm::BasicBlock::Create(m_Context.llvmContext, "while.body", func);
	auto afterBlock = llvm::BasicBlock::Create(m_Context.llvmContext, "while.after", func);

	m_Context.irBuilder.CreateBr(condBlock);
	m_Context.irBuilder.SetInsertPoint(condBlock);

	// Re-evaluate condition inside the condBlock!
	ExprLowerer exprLowerer(m_Context, m_AllocManager);
	const auto cond = exprLowerer.lowerExpr(*n.cond);
	exprLowerer.emitExprCleanup();

	m_Context.irBuilder.CreateCondBr(cond.value, bodyBlock, afterBlock);
	m_Context.irBuilder.SetInsertPoint(bodyBlock);

	visitNode(*n.body);

	if (!m_Context.irBuilder.GetInsertBlock()->getTerminator()) {
		m_Context.irBuilder.CreateBr(condBlock);
	}

	m_Context.irBuilder.SetInsertPoint(afterBlock);
}

void CodeGen::visit(const ast::ReturnStmt &n) {
	ExprLowerer exprLowerer(m_Context, m_AllocManager);
	const auto [value, type, isTemp] = exprLowerer.lowerExpr(*n.expr);

	if (isTemp) {
		exprLowerer.removeFromExprCleanup(value);
	} else {
		m_Context.copyValue(value, type);
	}

	m_AllocManager.emitFullScopeCleanup();
	m_Context.irBuilder.CreateRet(value);
}

void CodeGen::visit(const ast::VarDef &n) {
	ExprLowerer exprLowerer(m_Context, m_AllocManager);
	// Handle default-initialized variables without calling lowerExpr on DefaultInit
	llvm::Value *value = nullptr;
	Type valueType = nullptr;
	bool isTemp = true;

	if (n.value->kind == ast::NodeKind::DefaultInit) {
		valueType = n.type;
		auto *const llvmType = m_Context.typeConverter.convert(valueType);

		if (valueType->isTypeKind(TypeKind::Pointer)) {
			value = llvm::ConstantPointerNull::get(static_cast<llvm::PointerType *>(llvmType));
		} else if (valueType->isTypeKind(TypeKind::Primitive) ||
				   valueType->isTypeKind(TypeKind::Unit)) {
			value = llvm::ConstantInt::get(llvmType, 0);
		} else {
			value = llvm::ConstantAggregateZero::get(llvmType);
		}

		isTemp = true;
	} else {
		const auto [v, t, tmp] = exprLowerer.lowerExpr(*n.value);
		value = v;
		valueType = t;
		isTemp = tmp;
	}

	auto alloca = m_AllocManager.createAlloca(n.type, n.ident);

	if (isTemp) {
		exprLowerer.removeFromExprCleanup(value);
	} else {
		m_Context.copyValue(value, valueType);
	}

	m_Context.irBuilder.CreateStore(value, alloca);
	exprLowerer.emitExprCleanup();
}
}