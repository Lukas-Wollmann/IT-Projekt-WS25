#include "LoweringContext.h"
#include "ast/Visitor.h"
#include "type/Type.h"

namespace mir {
struct ExprResult {
	RegID reg;
	bool isTemp;
	type::TypePtr type;
};

struct LValue {
	RegID reg;
	bool isMemory;
};

///
/// This class should lower exactly one expression-statement, in the end it will produce a
/// list of registers that contain values that need to be cleaned up.
///
struct ExprLowerer : ast::ConstVisitor<ExprResult> {
private:
	LoweringContext &m_Context;

public:
	ExprLowerer(LoweringContext &ctx);

	ExprResult lowerExpr(const ast::Expr &n);

private:
	LValue lowerLValue(const ast::Expr &n);

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