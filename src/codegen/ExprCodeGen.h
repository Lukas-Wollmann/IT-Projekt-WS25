#pragma once
#include "ast/Visitor.h"

namespace llvm {
class Value;
}

namespace gen {
struct CodeGen;

struct LValueCodeGen : public ast::ConstVisitor<llvm::Value *> {
private:
	CodeGen &m_CodeGen;

public:
	LValueCodeGen(CodeGen &gen);

private:
	llvm::Value *visit(const ast::VarRef &n) override;
	llvm::Value *visit(const ast::UnaryExpr &n) override;
};
}