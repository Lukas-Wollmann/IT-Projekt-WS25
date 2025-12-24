#pragma once
#include "ast/Visitor.h"

namespace llvm {
    class Value;
}

namespace codegen {
	struct CodeGen : public ast::ConstVisitor<void> {
	private:
		struct Impl;

		Box<Impl> m_Impl;
        std::ostream &m_OStream;
        
        public:
		CodeGen(std::ostream &os);
		~CodeGen();

        void visit(const ast::Module &n) override;

    private:
		void visit(const ast::IntLit &n) override;
		void visit(const ast::FloatLit &n) override;
		void visit(const ast::CharLit &n) override;
		void visit(const ast::BoolLit &n) override;
		// void visit(const ast::StringLit &n) override;
		void visit(const ast::UnitLit &n) override;
		// void visit(const ast::ArrayExpr &n) override;
		void visit(const ast::UnaryExpr &n) override;
		void visit(const ast::BinaryExpr &n) override;
		void visit(const ast::Assignment &n) override;
		// void visit(const ast::HeapAlloc &n) override;
		// void visit(const ast::FuncCall &n) override;
		void visit(const ast::VarRef &n) override;
		void visit(const ast::BlockStmt &n) override;
		void visit(const ast::IfStmt &n) override;
		void visit(const ast::WhileStmt &n) override;
		void visit(const ast::ReturnStmt &n) override;
		void visit(const ast::VarDef &n) override;
		void visit(const ast::FuncDecl &n) override;

        llvm::Value *getPointerForLValue(const ast::Expr &lhs);
	};
}