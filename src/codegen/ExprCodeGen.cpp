#include "ExprCodeGen.h"

#include "CodeGen.h"
#include "CodeGenContext.h"
#include "type/Compare.h"

namespace gen {
using namespace ast;
using namespace type;

LValueCodeGen::LValueCodeGen(CodeGen &gen)
	: m_CodeGen(gen) {}

llvm::Value *LValueCodeGen::visit(const VarRef &n) {
	return m_CodeGen.m_Context.getAlloca(n.ident);
}

llvm::Value *LValueCodeGen::visit(const UnaryExpr &n) {
	VERIFY(n.op == UnaryOpKind::Dereference);

	m_CodeGen.dispatch(*n.operand);
	return m_CodeGen.m_LastValue;
}
}
