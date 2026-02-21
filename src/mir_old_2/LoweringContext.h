#pragma once
#include "MIR.h"

namespace mir {
struct TrackedValue {
	RegID reg;
	type::TypePtr type;
};

struct LoweringContext {
private:
	Ptr<Function> m_CurrentFunc;
	Ptr<BasicBlock> m_CurrentBlock;
	Vec<TrackedValue> m_ExprCleanup;
	Vec<Vec<TrackedValue>> m_ScopeCleanup;
	Vec<Map<U8String, RegID>> m_NamedValues;

public:
	LoweringContext(Ptr<Function> func, Ptr<BasicBlock> block)
		: m_CurrentFunc(func)
		, m_CurrentBlock(block) {}

	RegID nextRegID() {
		return m_CurrentFunc->nextRegID();
	}

	Ptr<BasicBlock> createBlock() {
		return m_CurrentFunc->createBlock();
	}

	Ptr<BasicBlock> getCurrentBlock() {
		return m_CurrentBlock;
	}

	void openScope() {
		m_NamedValues.push_back({});
		m_ScopeCleanup.push_back({});
	}

	void closeScope(bool emitCleanup = true) {
		if (emitCleanup)
			emitScopeCleanup();

		m_NamedValues.pop_back();
		m_ScopeCleanup.pop_back();
	}

	void emit(Box<Instr> instr) {
		m_CurrentBlock->instrs.push_back(std::move(instr));
	}

	void emitTerm(Box<Term> term) {
		m_CurrentBlock->term = std::move(term);
	}

	void setCurrentBlock(Ptr<BasicBlock> block) {
		m_CurrentBlock = std::move(block);
	}

	void addToExprCleanup(RegID reg, type::TypePtr type) {
		m_ExprCleanup.push_back({reg, std::move(type)});
	}

	void removeFromExprCleanup(RegID reg) {
		const auto cond = [reg](const TrackedValue &val) -> bool { return val.reg == reg; };

		m_ExprCleanup.erase(std::remove_if(m_ExprCleanup.begin(), m_ExprCleanup.end(), cond),
							m_ExprCleanup.end());
	}

	void emitExprCleanup() {
		for (const auto &tmp : m_ExprCleanup) {
			emit(std::make_unique<Destruct>(tmp.reg, tmp.type));
		}

		m_ExprCleanup.clear();
	}

	void addToScopeCleanup(RegID reg, type::TypePtr type) {
		m_ScopeCleanup.back().push_back({reg, std::move(type)});
	}

	void emitScopeCleanup() {
		auto &currentScope = m_ScopeCleanup.back();

		for (auto it = currentScope.rbegin(); it != currentScope.rend(); ++it) {
			emit(std::make_unique<Destruct>(it->reg, it->type));
		}
	}

	Opt<RegID> lookup(const U8String &ident) const {
		for (auto it = m_NamedValues.rbegin(); it != m_NamedValues.rend(); ++it) {
			if (const auto item = it->find(ident); item != it->end())
				return item->second;
		}

		return {};
	}

	void bind(const U8String &ident, RegID reg) {
		m_NamedValues.back().emplace(ident, reg);
	}
};
}