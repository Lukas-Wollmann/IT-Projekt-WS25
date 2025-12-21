#include "CodeGen.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>

#include "type/CompareVisitor.h"
#include "TypeConverter.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace codegen {
	using namespace llvm;

	struct CodeGen::Impl {
		LLVMContext context;
		Box<Module> module;
		IRBuilder<> builder;
		TypeConverter converter;
        std::unordered_map<std::string, AllocaInst *> namedValues;
		Value *lastValue;

		Impl(const std::string &moduleName)
			: module(std::make_unique<Module>(moduleName, context))
			, builder(context)
            , converter(context)
			, lastValue(nullptr) {}
	};

	CodeGen::CodeGen(std::ostream &os)
		: m_Impl(std::make_unique<Impl>("module"))
        , m_OStream(os) {}

	CodeGen::~CodeGen() {}

	void CodeGen::visit(const ast::IntLit &n) {
		m_Impl->lastValue = ConstantInt::get(Type::getInt64Ty(m_Impl->context), n.value);
	}

	void CodeGen::visit(const ast::FloatLit &n) {
		m_Impl->lastValue = ConstantFP::get(Type::getDoubleTy(m_Impl->context), n.value);
	}

	void CodeGen::visit(const ast::BoolLit &n) {
		m_Impl->lastValue = ConstantInt::get(Type::getInt1Ty(m_Impl->context), n.value);
	}

	void CodeGen::visit(const ast::CharLit &n) {
		m_Impl->lastValue = ConstantInt::get(Type::getInt8Ty(m_Impl->context), n.value);
	}

	void CodeGen::visit(const ast::VarRef &n) {
		AllocaInst *alloca = m_Impl->namedValues.at(n.ident.str());
		llvm::Type *type = alloca->getAllocatedType();

		m_Impl->lastValue = m_Impl->builder.CreateLoad(type, alloca, n.ident.str());
	}

	void CodeGen::visit(const ast::BlockStmt &n) {
		auto oldScope = m_Impl->namedValues;

		for (auto &stmt : n.stmts) {
			if (m_Impl->builder.GetInsertBlock()->getTerminator())
				break;

			dispatch(*stmt);
		}

		m_Impl->namedValues = std::move(oldScope);
	}

	void CodeGen::visit(const ast::UnaryExpr &n) {
		dispatch(*n.operand);

		Value *v = m_Impl->lastValue;
		type::TypePtr type = n.inferredType.value();

		using enum ast::UnaryOpKind;

		switch (n.op) {
			case Not:
				if (*type == type::Typename(u8"i32") || *type == type::Typename(u8"bool")) {
					m_Impl->lastValue = m_Impl->builder.CreateNot(v, "not");
					return;
				}
				UNREACHABLE();

			case Negative:
				if (*type == type::Typename(u8"i32")) {
					m_Impl->lastValue = m_Impl->builder.CreateNeg(v, "neg");
					return;
				}
				UNREACHABLE();

			case Positive:
				if (*type == type::Typename(u8"i32")) {
					m_Impl->lastValue = m_Impl->builder.CreateNeg(v, "neg");
				}
				return;

			case Dereference:
                UNREACHABLE();

			default: UNREACHABLE();
		}
	}

	void CodeGen::visit(const ast::BinaryExpr &n) {
		dispatch(*n.left);
		Value *l = m_Impl->lastValue;

		dispatch(*n.right);
		Value *r = m_Impl->lastValue;

		if (n.op == ast::BinaryOpKind::Addition)
			m_Impl->lastValue = m_Impl->builder.CreateAdd(l, r, "addtmp");
		else if (n.op == ast::BinaryOpKind::Subtraction)
			m_Impl->lastValue = m_Impl->builder.CreateSub(l, r, "subtmp");
		else if (n.op == ast::BinaryOpKind::Multiplication)
			m_Impl->lastValue = m_Impl->builder.CreateMul(l, r, "multmp");
		else if (n.op == ast::BinaryOpKind::Division)
			m_Impl->lastValue = m_Impl->builder.CreateSDiv(l, r, "divtmp");
	}

	void CodeGen::visit(const ast::ReturnStmt &n) {
		if (n.expr.has_value()) {
			dispatch(*n.expr.value());
			Value *retVal = m_Impl->lastValue;
			m_Impl->builder.CreateRet(retVal);
		} else {
			m_Impl->builder.CreateRetVoid();
		}

		m_Impl->lastValue = nullptr;
	}

	void CodeGen::visit(const ast::VarDef &n) {
		AllocaInst *alloca = m_Impl->builder.CreateAlloca(Type::getInt64Ty(m_Impl->context),
														  nullptr, n.ident.str());

		m_Impl->namedValues[n.ident.str()] = alloca;

		dispatch(*n.value);
		m_Impl->builder.CreateStore(m_Impl->lastValue, alloca);
	}

	void CodeGen::visit(const ast::FuncDecl &n) {
		Vec<Type *> argTypes(n.params.size(), Type::getInt64Ty(m_Impl->context));

		FunctionType *ft = FunctionType::get(Type::getInt64Ty(m_Impl->context), argTypes, false);

		Function *f = Function::Create(ft, Function::ExternalLinkage, n.ident.str(),
									   m_Impl->module.get());

		BasicBlock *entry = BasicBlock::Create(m_Impl->context, "entry", f);

		m_Impl->builder.SetInsertPoint(entry);
		m_Impl->namedValues.clear();

		unsigned idx = 0;
		for (auto &arg : f->args()) {
			arg.setName(n.params[idx].first.str());

			AllocaInst *Alloca = m_Impl->builder.CreateAlloca(Type::getInt64Ty(m_Impl->context),
															  nullptr, arg.getName());

			m_Impl->builder.CreateStore(&arg, Alloca);
			m_Impl->namedValues[arg.getName().str()] = Alloca;
			++idx;
		}

		dispatch(*n.body);

		verifyFunction(*f);
	}

	void CodeGen::visit(const ast::Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);

        raw_os_ostream llvmOS(m_OStream);
		m_Impl->module->print(llvmOS, nullptr);
	}
}