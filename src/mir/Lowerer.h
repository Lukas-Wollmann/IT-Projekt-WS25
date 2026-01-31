#include <map>

#include "ast/AST.h"
#include "mir/MIR.h"

namespace mir {

class Lowerer {
	mir::Module module;
	mir::Function *curFunc = nullptr;
	mir::BasicBlock *curBB = nullptr;

	// Structure to track both ID and Type for RAII cleanup
	struct TrackedVar {
		Id id;
		type::TypePtr type;
	};

	// The RAII Stack: Now tracks types so we only release pointers
	Vec<Vec<TrackedVar>> scopeStack;

	// Symbol Table: Maps source names (u8"a") -> unique MIR IDs (u8"a_0")
	Vec<Map<U8String, Id>> nameMapStack;
	u32 uniqueCounter = 0;

public:
	explicit Lowerer(U8String moduleName)
		: module(std::move(moduleName)) {}

	mir::Module lower(const ast::Module &astModule) {
		for (auto &decl : astModule.decls) {
			lower_function(*decl);
		}
		return std::move(module);
	}

private:
	// --- Name Resolution Helpers ---

	Id resolve_name(const U8String &name) {
		for (auto it = nameMapStack.rbegin(); it != nameMapStack.rend(); ++it) {
			if (it->count(name))
				return (*it)[name];
		}
		return name;
	}

	Id create_unique_id(const U8String &name) {
		Id unique = name + u8"_" + std::to_string(uniqueCounter++);
		nameMapStack.back()[name] = unique;
		return unique;
	}

	// --- Lowering Logic ---

	void lower_function(const ast::FuncDecl &decl) {
		auto mFunc = std::make_unique<mir::Function>(decl.ident, decl.returnType);
		curFunc = mFunc.get();

		scopeStack.clear();
		nameMapStack.clear();

		scopeStack.push_back({});
		nameMapStack.push_back({});

		for (auto &param : decl.params) {
			Id uniqueParam = create_unique_id(param.first);
			mFunc->params.push_back(uniqueParam);
			// Register parameter for cleanup (only if it's a pointer)
			scopeStack.back().push_back({uniqueParam, param.second});
		}

		curBB = create_bb(u8"entry");
		lower_block(*decl.body);

		if (!curBB->isTerminated()) {
			unwind_all_scopes();
			emit(InstKind::Return, {}, {u8"unit_val"}, nullptr);
		}

		module.functions.push_back(std::move(mFunc));
	}

	void lower_stmt(const ast::Stmt &stmt) {
		if (curBB->isTerminated())
			return;

		switch (stmt.kind) {
			case ast::NodeKind::VarDef: {
				auto &def = static_cast<const ast::VarDef &>(stmt);
				Id rhs = lower_expr(*def.value);
				Id uniqueId = create_unique_id(def.ident);

				emit(InstKind::Assign, uniqueId, {rhs}, def.type);
				// Track ID and Type for RAII
				scopeStack.back().push_back({uniqueId, def.type});
				break;
			}

			case ast::NodeKind::Assignment: {
				auto &assign = static_cast<const ast::Assignment &>(stmt);
				if (assign.left->kind == ast::NodeKind::VarRef) {
					auto &lhsRef = static_cast<const ast::VarRef &>(*assign.left);
					Id lhsId = resolve_name(lhsRef.ident);
					Id rhsId = lower_expr(*assign.right);

					if (is_pointer(assign.left->inferredType.value())) {
						emit(InstKind::Retain, {}, {rhsId}, assign.left->inferredType.value());
						emit(InstKind::Release, {}, {lhsId}, nullptr);
					}
					emit(InstKind::Assign, lhsId, {rhsId}, assign.left->inferredType.value());
				}
				break;
			}

			case ast::NodeKind::ReturnStmt: {
				auto &ret = static_cast<const ast::ReturnStmt &>(stmt);
				Id result = lower_expr(*ret.expr);

				unwind_all_scopes(result);
				emit(InstKind::Return, {}, {result}, nullptr);
				break;
			}

			case ast::NodeKind::IfStmt: {
				auto &if_ = static_cast<const ast::IfStmt &>(stmt);
				Id cond = lower_expr(*if_.cond);

				auto *thenBB = create_bb(u8"if_then");
				auto *elseBB = create_bb(u8"if_else");
				auto *mergeBB = create_bb(u8"if_merge");

				emit(InstKind::Branch, {}, {cond, thenBB->label, elseBB->label}, nullptr);

				curBB = thenBB;
				lower_block(*if_.then);
				if (!curBB->isTerminated())
					emit(InstKind::Jump, {}, {mergeBB->label}, nullptr);

				curBB = elseBB;
				lower_block(*if_.else_);
				if (!curBB->isTerminated())
					emit(InstKind::Jump, {}, {mergeBB->label}, nullptr);

				curBB = mergeBB;
				break;
			}

			case ast::NodeKind::BlockStmt:
				lower_block(static_cast<const ast::BlockStmt &>(stmt));
				break;

			default: break;
		}
	}

	void lower_block(const ast::BlockStmt &block) {
		scopeStack.push_back({});
		nameMapStack.push_back({});

		for (auto &s : block.stmts) {
			lower_stmt(*s);
		}

		if (!curBB->isTerminated()) {
			cleanup_current_scope();
		}

		scopeStack.pop_back();
		nameMapStack.pop_back();
	}

	Id lower_expr(const ast::Expr &expr) {
		switch (expr.kind) {
			case ast::NodeKind::IntLit: {
				Id t = curFunc->nextTemp();
				emit(InstKind::LoadLit, t,
					 {std::to_string(static_cast<const ast::IntLit &>(expr).value)},
					 expr.inferredType.value());
				return t;
			}

			case ast::NodeKind::VarRef: {
				auto &ref = static_cast<const ast::VarRef &>(expr);
				Id resolved = resolve_name(ref.ident);

				if (is_pointer(expr.inferredType.value())) {
					emit(InstKind::Retain, {}, {resolved}, expr.inferredType.value());
				}
				return resolved;
			}

			case ast::NodeKind::FuncCall: {
				auto &call = static_cast<const ast::FuncCall &>(expr);
				Id funcPtr = lower_expr(*call.expr);
				Vec<Id> argIds;
				for (auto &arg : call.args)
					argIds.push_back(lower_expr(*arg));

				Id res = curFunc->nextTemp();
				emit(InstKind::Call, res, argIds, expr.inferredType.value());

				if (is_pointer(expr.inferredType.value())) {
					// Temporaries from function calls must also be tracked for release
					scopeStack.back().push_back({res, expr.inferredType.value()});
				}
				return res;
			}

			case ast::NodeKind::BinaryExpr: {
				auto &bin = static_cast<const ast::BinaryExpr &>(expr);
				Id lhs = lower_expr(*bin.left);
				Id rhs = lower_expr(*bin.right);
				Id res = curFunc->nextTemp();
				emit(InstKind::BinaryOp, res, {lhs, rhs}, expr.inferredType.value());
				return res;
			}

			default: return u8"";
		}
	}

	// --- RAII Helpers (Type-Aware) ---

	void cleanup_current_scope() {
		auto &currentVars = scopeStack.back();
		for (auto it = currentVars.rbegin(); it != currentVars.rend(); ++it) {
			// FIX: Only emit RELEASE for pointers
			if (is_pointer(it->type)) {
				emit(InstKind::Release, {}, {it->id}, it->type);
			}
		}
	}

	void unwind_all_scopes(const Opt<Id> &excludeId = std::nullopt) {
		for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
			for (auto varIt = it->rbegin(); varIt != it->rend(); ++varIt) {
				if (excludeId && varIt->id == *excludeId)
					continue;

				// FIX: Only emit RELEASE for pointers
				if (is_pointer(varIt->type)) {
					emit(InstKind::Release, {}, {varIt->id}, varIt->type);
				}
			}
		}
	}

	mir::BasicBlock *create_bb(const U8String &prefix) {
		static u32 bbCounter = 0;
		Id label = prefix + u8"_" + std::to_string(bbCounter++);
		auto bb = std::make_unique<mir::BasicBlock>(label);
		curFunc->blocks.push_back(std::move(bb));
		return curFunc->blocks.back().get();
	}

	void emit(InstKind k, Opt<Id> dest, Vec<Id> args, type::TypePtr t) {
		curBB->instructions.push_back({k, std::move(dest), std::move(args), std::move(t)});
	}

	bool is_pointer(const type::TypePtr &t) {
		return t && t->kind == type::TypeKind::Pointer;
	}
};

} // namespace mir
