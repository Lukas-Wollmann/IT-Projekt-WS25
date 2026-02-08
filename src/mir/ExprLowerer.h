#include "MIR.h"
#include "ast/Visitor.h"
#include "type/Type.h"

namespace mir {
struct ExprResult {
	RegID reg;
	bool isTemp;
	type::TypePtr type;
};

struct TrackedValue {
	RegID reg;
	type::TypePtr type;
};

///
/// This class should lower exactly one expression, in the end it will produce a list of
/// registers that contain values that need to be cleaned up.
///
struct ExprLowerer : ast::ConstVisitor<ExprResult> {
private:
	Function &m_CurrentFunc;
	Vec<Box<Instr>> m_Instrs;
	Vec<TrackedValue> m_CleanupValues;

public:
	explicit ExprLowerer(Function &func);

	void lowerExpr(const ast::Expr &n);

private:
	void emit(Box<Instr> instr);
	void addToCleanup(RegID reg, type::TypePtr type);
	void removeFromCleanup(RegID reg);

	ExprResult visit(const ast::IntLit &n) override;
	ExprResult visit(const ast::BoolLit &n) override;
	ExprResult visit(const ast::CharLit &n) override;
	ExprResult visit(const ast::UnitLit &n) override;
	ExprResult visit(const ast::HeapAlloc &n) override;
	ExprResult visit(const ast::VarRef &n) override;
	ExprResult visit(const ast::UnaryExpr &n) override;
	ExprResult visit(const ast::BinaryExpr &n) override;
	ExprResult visit(const ast::FuncCall &n) override;
	ExprResult visit(const ast::Assignment &n) override;
};
}