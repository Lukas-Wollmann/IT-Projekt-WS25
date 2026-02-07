#include "Lowerer.h"

namespace mir {

Lowerer::Lowerer(U8String moduleName)
	: m_Module(std::move(moduleName)) {}

Module Lowerer::lowerModule(const ast::Module &n) {
	for (const auto &d : n.decls)
		lowerFuncDecl(*d);
	return std::move(m_Module);
}

RegisterID Lowerer::newRegister() {
	return m_CurrentFunc->get().nextRegisterID();
}

void Lowerer::emit(Box<Instr> instr) {
	m_CurrentBlock->get().instrs.push_back(std::move(instr));
}

void Lowerer::emitTerm(Box<Term> term) {
	m_CurrentBlock->get().term = std::move(term);
}

void Lowerer::openScope() {
	m_NamedValues.push_back({});
	m_TrackedValues.push_back({});
}

void Lowerer::closeScope() {
	if (!m_CurrentBlock->get().isTerminated())
		generateScopeCleanup();

	m_NamedValues.pop_back();
	m_TrackedValues.pop_back();
}

void Lowerer::bind(const U8String &name, RegisterID id) {
	m_NamedValues.back().emplace(name, id);
}

Opt<RegisterID> Lowerer::lookup(const U8String &name) {
	for (auto it = m_NamedValues.rbegin(); it != m_NamedValues.rend(); ++it) {
		if (const auto item = it->find(name); item != it->end())
			return item->second;
	}

	return {};
}

void Lowerer::lowerFuncDecl(const ast::FuncDecl &n) {
	m_Module.funcs.push_back(std::make_unique<Function>(n.ident));
	m_CurrentFunc = *m_Module.funcs.back();

	// Create entry block
	auto &entryBlock = m_CurrentFunc->get().createBlock();
	m_CurrentBlock = entryBlock;

	openScope(); // Function scope

	for (const auto &param : n.params) {
		const auto regID = newRegister();
		bind(param.first, regID);
		m_CurrentFunc->get().params.push_back(regID);

		// If param is a pointer, track it for cleanup.
		// NOTE: Assuming caller retains arguments, so we own a reference locally.
		// TODO: we should prob retain here and not at the call site
		if (param.second->isTypeKind(type::TypeKind::Pointer))
			m_TrackedValues.back().push_back({regID, param.second});
	}

	lowerBlockStmt(*n.body);

	// Implicit return for void functions or fallthrough
	if (!m_CurrentBlock->get().isTerminated()) {
		generateScopeCleanup();
		// TODO emit unit lit here
		emitTerm(std::make_unique<Return>(0, nullptr)); // Void return
	}

	// Do not call closeScope() here because lowerBlockStmt already closed its scope
	// We just need to pop the function arguments scope manually or handle it structurally.
	// Since `lowerBlockStmt` opens/closes its OWN scope, the args are in the outer scope.
	m_NamedValues.pop_back();
	m_TrackedValues.pop_back();
}

void Lowerer::lowerBlockStmt(const ast::BlockStmt &n) {
	openScope();
	for (const auto &stmt : n.stmts) {
		lowerStmt(*stmt);
		if (m_CurrentBlock->get().isTerminated())
			break;
	}
	closeScope();
}

void Lowerer::lowerStmt(const ast::Stmt &n) {
	if (m_CurrentBlock->get().isTerminated())
		return;

	switch (n.kind) {
		case ast::NodeKind::VarDef: {
			auto &var = static_cast<const ast::VarDef &>(n);

			// 1. Lower expression
			ExprResult res = lowerExpr(*var.value);
			RegisterID rhs = res.reg;

			// 2. Create variable
			RegisterID lhs = newRegister();
			bind(var.ident, lhs);
			emit(std::make_unique<Assign>(lhs, rhs));

			// 3. Pointer Semantics
			if (var.type->isTypeKind(type::TypeKind::Pointer)) {
				m_TrackedValues.back().push_back({lhs, var.type});

				if (res.isManaged) {
					// MOVE SEMANTICS:
					// The RHS was a temp (Alloc/Call) with RefCount=1.
					// We take ownership. No Retain needed.
					// We MUST remove it from expr temps so it isn't released at end of stmt.
					for (auto it = m_ExprTemps.begin(); it != m_ExprTemps.end(); ++it) {
						if (it->id == rhs) {
							m_ExprTemps.erase(it);
							break;
						}
					}
				} else {
					// COPY SEMANTICS:
					// The RHS was a borrowed ref (VarRef).
					// We are creating a new reference. Must Retain.
					emit(std::make_unique<SPRetain>(lhs, var.type));
				}
			}
			break;
		}
		case ast::NodeKind::IfStmt: lowerIfStmt(static_cast<const ast::IfStmt &>(n)); break;

		case ast::NodeKind::ReturnStmt:
			lowerReturnStmt(static_cast<const ast::ReturnStmt &>(n));
			break;

		case ast::NodeKind::BlockStmt:
			lowerBlockStmt(static_cast<const ast::BlockStmt &>(n));
			break;

		default: {
			ExprResult res = lowerExpr(static_cast<const ast::Expr &>(n));
			// If the expression result is managed (e.g. Call returning ptr),
			// and it is NOT used (ExprStmt), it will stay in m_ExprTemps
			// and be cleaned up by cleanupExprTemps() below.
			break;
		}
	}

	// End of statement cleanup (temporaries)
	cleanupExprTemps();
}

void Lowerer::lowerIfStmt(const ast::IfStmt &n) {
	// Condition
	ExprResult cond = lowerExpr(*n.cond);
	cleanupExprTemps(); // Clean temps used in condition

	auto &func = m_CurrentFunc->get();
	auto &thenBlock = func.createBlock();
	auto &mergeBlock = func.createBlock();

	// Create Else block only if needed
	BlockID elseBlockID = mergeBlock.id;
	BasicBlock *elseBlockPtr = nullptr;

	auto &elseBlock = func.createBlock();
	elseBlockID = elseBlock.id;
	elseBlockPtr = &elseBlock;

	// Branch
	emitTerm(std::make_unique<Branch>(cond.reg, thenBlock.id, elseBlockID));

	// THEN
	m_CurrentBlock = thenBlock;
	lowerStmt(*n.then); // Usually a BlockStmt
	if (!m_CurrentBlock->get().isTerminated())
		emitTerm(std::make_unique<Jump>(mergeBlock.id));

	// ELSE
	m_CurrentBlock = *elseBlockPtr;
	lowerStmt(*n.else_);
	if (!m_CurrentBlock->get().isTerminated())
		emitTerm(std::make_unique<Jump>(mergeBlock.id));

	m_CurrentBlock = mergeBlock;
}

void Lowerer::lowerReturnStmt(const ast::ReturnStmt &n) {
	RegisterID val = 0;
	type::TypePtr type = nullptr;

	ExprResult res = lowerExpr(*n.expr);
	val = res.reg;
	type = n.expr->inferredType.value();

	// RAII for Return Value:
	if (type->isTypeKind(type::TypeKind::Pointer)) {
		// If it's a temp (isManaged), it has +1 ref. We return it as is.
		// If it's a local var (!isManaged), it has +1 ref bound to the var name.
		// However, we are about to destroy the scope, which will release the local var.
		// To return it safely, we must increment ref count so it survives the scope cleanup.
		if (!res.isManaged) {
			emit(std::make_unique<SPRetain>(val, type));
		} else {
			// It was a temp. We are returning it. Remove from temps list so
			// cleanupExprTemps() doesn't kill it before we return.
			for (auto it = m_ExprTemps.begin(); it != m_ExprTemps.end(); ++it) {
				if (it->id == val) {
					m_ExprTemps.erase(it);
					break;
				}
			}
		}
	}

	// Unwind ALL scopes (params, locals, blocks)
	cleanupScopesUntil(0);

	emitTerm(std::make_unique<Return>(val, type));
}

ExprResult Lowerer::lowerExpr(const ast::Expr &n) {
	switch (n.kind) {
		case ast::NodeKind::UnitLit: {
			const auto &lit = static_cast<const ast::UnitLit &>(n);
			RegisterID dest = newRegister();
			emit(std::make_unique<UnitLit>(dest, lit.inferredType.value()));
			return {dest, false};
		}
		case ast::NodeKind::IntLit: {
			const auto &lit = static_cast<const ast::IntLit &>(n);
			RegisterID dest = newRegister();
			emit(std::make_unique<IntLit>(dest, lit.value, lit.inferredType.value()));
			return {dest, false};
		}
		case ast::NodeKind::CharLit: {
			const auto &lit = static_cast<const ast::CharLit &>(n);
			RegisterID dest = newRegister();
			emit(std::make_unique<CharLit>(dest, lit.value, lit.inferredType.value()));
			return {dest, false};
		}
		case ast::NodeKind::HeapAlloc: {
			const auto &alloc = static_cast<const ast::HeapAlloc &>(n);
			RegisterID dest = newRegister();
			auto type = alloc.inferredType.value();
			auto expr = lowerExpr(*alloc.expr);

			if (alloc.expr->inferredType.value()->isTypeKind(type::TypeKind::Pointer)) {
				if (expr.isManaged) {
					// MOVE SEMANTICS:
					// The RHS was a temp (Alloc/Call) with RefCount=1.
					// We take ownership. No Retain needed.
					// We MUST remove it from expr temps so it isn't released at end of stmt.
					for (auto it = m_ExprTemps.begin(); it != m_ExprTemps.end(); ++it) {
						if (it->id == expr.reg) {
							m_ExprTemps.erase(it);
							break;
						}
					}
				} else {
					// COPY SEMANTICS:
					// The RHS was a borrowed ref (VarRef).
					// We are creating a new reference. Must Retain.
					emit(std::make_unique<SPRetain>(expr.reg, alloc.expr->inferredType.value()));
				}
			}

			// Alloc creates refcount=1
			emit(std::make_unique<SPCreate>(dest, type));
			emit(std::make_unique<Assign>(dest, expr.reg));

			// Track as temp
			m_ExprTemps.push_back({dest, type});
			return {dest, true}; // Managed
		}
		case ast::NodeKind::VarRef: {
			const auto &ref = static_cast<const ast::VarRef &>(n);

			// 1. Try to find a local variable/parameter
			if (auto reg = lookup(ref.ident)) {
				return {*reg, false}; // Borrowed local ref
			}

			// 2. If not found locally, assume it is a Global Function
			// Emit an instruction to load the function address into a new register.
			const auto dest = newRegister();

			// Note: We use the VarRef's inferred type (which should be a FunctionType)
			emit(std::make_unique<LoadFunc>(dest, ref.ident, n.inferredType.value()));

			// The result is a pointer/function-ptr, but it's a global constant
			// address, so it doesn't need RAII management (isManaged = false).
			return {dest, false};
		}
		case ast::NodeKind::BinaryExpr: {
			auto &bin = static_cast<const ast::BinaryExpr &>(n);
			ExprResult l = lowerExpr(*bin.left);
			ExprResult r = lowerExpr(*bin.right);
			RegisterID dest = newRegister();
			emit(std::make_unique<BinaryOp>(dest, l.reg, r.reg, bin.op, n.inferredType.value()));
			return {dest, false};
		}
		case ast::NodeKind::UnaryExpr: {
			auto &un = static_cast<const ast::UnaryExpr &>(n);
			ExprResult op = lowerExpr(*un.operand);
			RegisterID dest = newRegister();
			auto type = un.inferredType.value();

			if (un.op == UnaryOpKind::Dereference) {
				emit(std::make_unique<Load>(dest, op.reg, type));
			} else {
				emit(std::make_unique<UnaryOp>(dest, op.reg, un.op, type));
			}
			return {dest, false};
		}
		case ast::NodeKind::FuncCall: {
			const auto &call = static_cast<const ast::FuncCall &>(n);
			ExprResult callee = lowerExpr(*call.expr); // Typically a function ptr or just address

			Vec<RegisterID> args;
			for (const auto &arg : call.args)
				args.push_back(lowerExpr(*arg).reg);

			RegisterID dest = newRegister();
			auto type = call.inferredType.value();

			emit(std::make_unique<Call>(dest, callee.reg, args, type));

			// If function returns a pointer, it implies ownership transfer (usually)
			// or at least a +1 ref count from the creation inside the function.
			if (type->isTypeKind(type::TypeKind::Pointer)) {
				m_ExprTemps.push_back({dest, type});
				return {dest, true}; // Managed
			}
			return {dest, false};
		}
		case ast::NodeKind::Assignment: {
			const auto &assign = static_cast<const ast::Assignment &>(n);

			LValue lhs = lowerLValue(*assign.left);
			ExprResult rhs = lowerExpr(*assign.right); // lowerExpr tells us if it's managed

			auto type = assign.left->inferredType.value();

			if (type->isTypeKind(type::TypeKind::Pointer)) {
				// 1. Handle RHS (The "Retain" step)
				if (!rhs.isManaged) {
					// RHS is an existing variable (x = y).
					// We must increment the count because 'y' still owns it too.
					emit(std::make_unique<SPRetain>(rhs.reg, type));
				} else {
					// RHS is a temporary (x = alloc Foo).
					// It already has +1. We "take" that +1.
					// We must remove it from temps so closeScope() doesn't release it!
					for (auto it = m_ExprTemps.begin(); it != m_ExprTemps.end(); ++it) {
						if (it->id == rhs.reg) {
							m_ExprTemps.erase(it);
							break;
						}
					}
				}

				// 2. Handle LHS (The "Release" step)
				if (lhs.isMemory) {
					// *ptr = ...
					// We must release the pointer currently sitting in that memory
					RegisterID oldVal = m_CurrentFunc->get().nextRegisterID();
					emit(std::make_unique<Load>(oldVal, lhs.reg, type));
					emit(std::make_unique<SPRelease>(oldVal));
					emit(std::make_unique<Assign>(lhs.reg, rhs.reg));
				} else {
					// x = ...
					// Release the pointer currently held in the variable's register
					emit(std::make_unique<SPRelease>(lhs.reg));
					emit(std::make_unique<Assign>(lhs.reg, rhs.reg));
				}
			} else {
				// Standard scalar assignment (Int/Char)
				emit(std::make_unique<Assign>(lhs.reg, rhs.reg));
			}

			RegisterID dest = newRegister();
			emit(std::make_unique<UnitLit>(dest, assign.inferredType.value()));
			return {dest, false};
		}
		default: UNREACHABLE(); // Unknown Expr
	}
}

LValue Lowerer::lowerLValue(const ast::Expr &n) {
	switch (n.kind) {
		case ast::NodeKind::VarRef: {
			const auto &ref = static_cast<const ast::VarRef &>(n);
			// Local variables are just registers in our MIR
			return {*lookup(ref.ident), false};
		}
		case ast::NodeKind::UnaryExpr: {
			auto &un = static_cast<const ast::UnaryExpr &>(n);
			if (un.op == UnaryOpKind::Dereference) {
				// For *ptr, the LValue is the address held in the pointer
				ExprResult addr = lowerExpr(*un.operand);
				return {addr.reg, true};
			}
			break;
		}
		default: break;
	}
	UNREACHABLE();
}

void Lowerer::generateScopeCleanup() {
	auto &currentScope = m_TrackedValues.back();
	// Iterate in reverse order (stack discipline)
	for (auto it = currentScope.rbegin(); it != currentScope.rend(); ++it) {
		emit(std::make_unique<SPRelease>(it->id));
	}
}

void Lowerer::cleanupScopesUntil(size_t depth) {
	// Cleanup from current scope down to depth
	for (size_t i = m_TrackedValues.size(); i > depth; --i) {
		auto &scope = m_TrackedValues[i - 1];
		for (auto it = scope.rbegin(); it != scope.rend(); ++it) {
			emit(std::make_unique<SPRelease>(it->id));
		}
	}
}

void Lowerer::cleanupExprTemps() {
	// Release any temporaries generated in the statement that weren't consumed
	for (const auto &tmp : m_ExprTemps) {
		emit(std::make_unique<SPRelease>(tmp.id));
	}
	m_ExprTemps.clear();
}

}