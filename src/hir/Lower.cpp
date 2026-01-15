#include "Lower.h"

namespace hir {

	Slot LowerContext::newSlot(type::TypePtr type, U8String name) {
		return {m_NextLocalID++, std::move(type), std::move(name)};
	}

	Slot LowerContext::lowerExpr(const ast::Expr &n) {
		using enum ast::NodeKind;

		switch (n.kind) {
			case IntLit:	 return lowerIntLit(static_cast<const ast::IntLit &>(n));
			case VarRef:	 return lowerVarRef(static_cast<const ast::VarRef &>(n));
			case UnaryExpr:	 return lowerUnaryExpr(static_cast<const ast::UnaryExpr &>(n));
			case BinaryExpr: return lowerBinaryExpr(static_cast<const ast::BinaryExpr &>(n));
			default:		 UNREACHABLE();
		}
	}

	Slot LowerContext::lowerIntLit(const ast::IntLit &n) {
		auto slot = newSlot(n.inferredType.value());

		m_Func->blocks.back()
				.stmts
				.push_back()

						ctx.emit(std::make_unique<Assignment>(
								slot, std::make_unique<hir::IntLiteral>(lit->value, lit->type)));

		return slot;
	}

	Slot LowerContext::lowerVarRef(const ast::VarRef &n) {}

	Slot LowerContext::lowerUnaryExpr(const ast::UnaryExpr &n) {}

	Slot LowerContext::lowerBinaryExpr(const ast::BinaryExpr &n) {}
}

/*
// =======================
// HIR DEFINITIONS
// =======================

#pragma once
#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace type {
	struct Type {
		bool needs_drop = false;
	};
	using TypePtr = Type*;
}

namespace hir {

using LocalID = size_t;

struct Slot {
	LocalID id;
	type::TypePtr type;
};

// ---- RValues ----

struct RValue {
	type::TypePtr type;
	virtual ~RValue() = default;
};

struct IntLiteral : RValue {
	int value;
};

enum class UnaryOpKind { Neg };
enum class BinaryOpKind { Add, Sub, Mul, Div, Lt };

struct UnaryOp : RValue {
	UnaryOpKind op;
	Slot operand;
};

struct BinaryOp : RValue {
	BinaryOpKind op;
	Slot lhs;
	Slot rhs;
};

// ---- Statements ----

struct Stmt {
	virtual ~Stmt() = default;
};

struct Assignment : Stmt {
	Slot dst;
	std::unique_ptr<RValue> value;
};

struct Drop : Stmt {
	Slot slot;
};

// ---- Terminators ----

struct Terminator {
	virtual ~Terminator() = default;
};

struct Goto : Terminator {
	size_t target;
};

struct If : Terminator {
	Slot cond;
	size_t then_blk;
	size_t else_blk;
};

struct Return : Terminator {
	std::optional<Slot> value;
};

// ---- Block / Function ----

struct Block {
	std::vector<std::unique_ptr<Stmt>> stmts;
	std::unique_ptr<Terminator> term;
};

struct Function {
	std::string name;
	std::vector<Block> blocks;
};

} // namespace hir

// =======================
// AST (MINIMAL)
// =======================

enum class AstExprKind { Int, Var, Unary, Binary };

struct AstExpr {
	AstExprKind kind;
	type::TypePtr type;
	virtual ~AstExpr() = default;
};

struct AstInt : AstExpr {
	int value;
};

struct AstVar : AstExpr {
	std::string name;
};

struct AstUnary : AstExpr {
	hir::UnaryOpKind op;
	AstExpr* expr;
};

struct AstBinary : AstExpr {
	hir::BinaryOpKind op;
	AstExpr* lhs;
	AstExpr* rhs;
};

enum class AstStmtKind { Let, Expr, If, Return };

struct AstStmt {
	AstStmtKind kind;
	virtual ~AstStmt() = default;
};

struct AstLet : AstStmt {
	std::string name;
	AstExpr* init;
};

struct AstExprStmt : AstStmt {
	AstExpr* expr;
};

struct AstIf : AstStmt {
	AstExpr* cond;
	std::vector<AstStmt*> then_body;
	std::vector<AstStmt*> else_body;
};

struct AstReturn : AstStmt {
	AstExpr* value;
};

// =======================
// LOWERING CONTEXT
// =======================

struct Env {
	std::unordered_map<std::string, hir::Slot> map;

	void bind(const std::string& name, hir::Slot slot) {
		map[name] = slot;
	}

	hir::Slot lookup(const std::string& name) {
		return map.at(name);
	}
};

struct Scope {
	std::vector<hir::Slot> locals;
};

struct LowerCtx {
	hir::Function* fn;
	hir::Block* block;
	Env env;
	std::vector<Scope> scopes;
	hir::LocalID next_id = 0;

	hir::Slot fresh_slot(type::TypePtr type) {
		return { next_id++, type };
	}

	void emit(std::unique_ptr<hir::Stmt> s) {
		block->stmts.push_back(std::move(s));
	}

	void emit_assign(hir::Slot dst, std::unique_ptr<hir::RValue> rv) {
		emit(std::make_unique<hir::Assignment>(
			hir::Assignment{ dst, std::move(rv) }
		));
	}

	void emit_drop(hir::Slot s) {
		if (s.type->needs_drop) {
			emit(std::make_unique<hir::Drop>(hir::Drop{ s }));
		}
	}

	void enter_scope() {
		scopes.emplace_back();
	}

	void exit_scope() {
		for (auto& s : scopes.back().locals)
			emit_drop(s);
		scopes.pop_back();
	}

	void bind(const std::string& name, hir::Slot s) {
		env.bind(name, s);
		scopes.back().locals.push_back(s);
	}
};

// =======================
// HELPERS
// =======================

size_t new_block(LowerCtx& ctx) {
	ctx.fn->blocks.emplace_back();
	return ctx.fn->blocks.size() - 1;
}

// =======================
// EXPRESSION LOWERING
// =======================

hir::Slot lower_expr(LowerCtx& ctx, AstExpr* e);

hir::Slot lower_expr(LowerCtx& ctx, AstExpr* e) {
	switch (e->kind) {
		case AstExprKind::Int: {
			auto* i = static_cast<AstInt*>(e);
			hir::Slot s = ctx.fresh_slot(e->type);
			auto lit = std::make_unique<hir::IntLiteral>();
			lit->value = i->value;
			lit->type = e->type;
			ctx.emit_assign(s, std::move(lit));
			return s;
		}

		case AstExprKind::Var: {
			auto* v = static_cast<AstVar*>(e);
			return ctx.env.lookup(v->name);
		}

		case AstExprKind::Unary: {
			auto* u = static_cast<AstUnary*>(e);
			hir::Slot op = lower_expr(ctx, u->expr);
			hir::Slot res = ctx.fresh_slot(e->type);

			auto rv = std::make_unique<hir::UnaryOp>();
			rv->op = u->op;
			rv->operand = op;
			rv->type = e->type;

			ctx.emit_assign(res, std::move(rv));
			ctx.emit_drop(op);
			return res;
		}

		case AstExprKind::Binary: {
			auto* b = static_cast<AstBinary*>(e);
			hir::Slot lhs = lower_expr(ctx, b->lhs);
			hir::Slot rhs = lower_expr(ctx, b->rhs);
			hir::Slot res = ctx.fresh_slot(e->type);

			auto rv = std::make_unique<hir::BinaryOp>();
			rv->op = b->op;
			rv->lhs = lhs;
			rv->rhs = rhs;
			rv->type = e->type;

			ctx.emit_assign(res, std::move(rv));
			ctx.emit_drop(lhs);
			ctx.emit_drop(rhs);
			return res;
		}
	}
	std::abort();
}

// =======================
// STATEMENT LOWERING
// =======================

void lower_stmt(LowerCtx& ctx, AstStmt* s);

void lower_stmt(LowerCtx& ctx, AstStmt* s) {
	switch (s->kind) {
		case AstStmtKind::Let: {
			auto* l = static_cast<AstLet*>(s);
			hir::Slot rhs = lower_expr(ctx, l->init);
			hir::Slot dst = ctx.fresh_slot(rhs.type);
			ctx.bind(l->name, dst);

			auto rv = std::make_unique<hir::UnaryOp>();
			rv->op = hir::UnaryOpKind::Neg; // dummy "move"
			rv->operand = rhs;
			rv->type = rhs.type;

			ctx.emit_assign(dst, std::move(rv));
			ctx.emit_drop(rhs);
			break;
		}

		case AstStmtKind::Expr: {
			auto* es = static_cast<AstExprStmt*>(s);
			hir::Slot tmp = lower_expr(ctx, es->expr);
			ctx.emit_drop(tmp);
			break;
		}

		case AstStmtKind::Return: {
			auto* r = static_cast<AstReturn*>(s);
			hir::Slot v = lower_expr(ctx, r->value);
			ctx.block->term = std::make_unique<hir::Return>(
				hir::Return{ v }
			);
			break;
		}

		case AstStmtKind::If: {
			auto* i = static_cast<AstIf*>(s);
			hir::Slot cond = lower_expr(ctx, i->cond);

			size_t then_blk = new_block(ctx);
			size_t else_blk = new_block(ctx);
			size_t join_blk = new_block(ctx);

			ctx.block->term = std::make_unique<hir::If>(
				hir::If{ cond, then_blk, else_blk }
			);
			ctx.emit_drop(cond);

			// then
			ctx.block = &ctx.fn->blocks[then_blk];
			ctx.enter_scope();
			for (auto* st : i->then_body)
				lower_stmt(ctx, st);
			ctx.exit_scope();
			ctx.block->term = std::make_unique<hir::Goto>(
				hir::Goto{ join_blk }
			);

			// else
			ctx.block = &ctx.fn->blocks[else_blk];
			ctx.enter_scope();
			for (auto* st : i->else_body)
				lower_stmt(ctx, st);
			ctx.exit_scope();
			ctx.block->term = std::make_unique<hir::Goto>(
				hir::Goto{ join_blk }
			);

			ctx.block = &ctx.fn->blocks[join_blk];
			break;
		}
	}
}

// =======================
// FUNCTION LOWERING
// =======================

struct AstFunction {
	std::string name;
	std::vector<std::pair<std::string, type::TypePtr>> params;
	std::vector<AstStmt*> body;
	type::TypePtr return_type;
};

hir::Function lower_function(AstFunction* ast) {
	hir::Function fn;
	fn.name = ast->name;

	// Create entry block
	fn.blocks.emplace_back();

	LowerCtx ctx;
	ctx.fn = &fn;
	ctx.block = &fn.blocks[0];

	// Enter function scope
	ctx.enter_scope();

	// Lower parameters as slots
	for (auto& [name, type] : ast->params) {
		hir::Slot param = ctx.fresh_slot(type);
		ctx.bind(name, param);
	}

	// Lower body
	for (auto* stmt : ast->body) {
		lower_stmt(ctx, stmt);

		// stop if block already terminated
		if (ctx.block->term)
			break;
	}

	// Implicit return (void)
	if (!ctx.block->term) {
		ctx.exit_scope();
		ctx.block->term = std::make_unique<hir::Return>(
			hir::Return{ std::nullopt }
		);
	}

	return fn;
}

*/