#pragma once
#include "MIR.h"

namespace mir {
struct TrackedValue {
	RegID reg;
	type::TypePtr type;
};

struct LoweringContext {
public:
	Vec<Box<Instr>> m_Instrs;
	Vec<TrackedValue> m_CleanupValues;
	Vec<Vec<TrackedValue>> m_TrackedValues;

public:
	void emit(Box<Instr> instr) {
		m_Instrs.push_back(std::move(instr));
	}

	void addToCleanup(RegID reg, type::TypePtr type) {
		m_CleanupValues.push_back({reg, std::move(type)});
	}

	void removeFromCleanup(RegID reg) {
		const auto cond = [reg](const TrackedValue &val) -> bool { return val.reg == reg; };
		std::remove_if(m_CleanupValues.begin(), m_CleanupValues.end(), cond);
	}
};
}