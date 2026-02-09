#include "MIR.h"

namespace mir {
Instr::Instr(const InstrKind kind)
	: kind(kind) {}

IntLit::IntLit(const RegID dest, const i32 value)
	: Instr(InstrKind::IntLit)
	, dest(dest)
	, value(value) {}

BoolLit::BoolLit(const RegID dest, const bool value)
	: Instr(InstrKind::BoolLit)
	, dest(dest)
	, value(value) {}

CharLit::CharLit(const RegID dest, const char32_t value)
	: Instr(InstrKind::CharLit)
	, dest(dest)
	, value(value) {}

UnitLit::UnitLit(const RegID dest)
	: Instr(InstrKind::UnitLit)
	, dest(dest) {}

LoadFunc::LoadFunc(const RegID dest, U8String funcName, type::TypePtr type)
	: Instr(InstrKind::LoadFunc)
	, dest(dest)
	, funcName(std::move(funcName))
	, type(std::move(type)) {}

Alloc::Alloc(const RegID dest, type::TypePtr type)
	: Instr(InstrKind::Alloc)
	, dest(dest)
	, type(std::move(type)) {}

Load::Load(const RegID dest, const RegID addr, type::TypePtr type)
	: Instr(InstrKind::Load)
	, dest(dest)
	, addr(addr)
	, type(std::move(type)) {}

Store::Store(const RegID dest, const RegID src)
	: Instr(InstrKind::Assign)
	, dest(dest)
	, src(src) {}

Call::Call(RegID dest, RegID callee, Vec<RegID> args)
	: Instr(InstrKind::Call)
	, dest(dest)
	, callee(callee)
	, args(std::move(args)) {}

BinaryOp::BinaryOp(const RegID dest, const RegID left, const RegID right, const BinaryOpKind op)
	: Instr(InstrKind::BinaryOp)
	, dest(dest)
	, left(left)
	, right(right)
	, op(op) {}

UnaryOp::UnaryOp(const RegID dest, const RegID operand, const UnaryOpKind op)
	: Instr(InstrKind::UnaryOp)
	, dest(dest)
	, operand(operand)
	, op(op) {}

SPCreate::SPCreate(const RegID reg, type::TypePtr type)
	: Instr(InstrKind::SPCreate)
	, reg(reg)
	, type(std::move(type)) {}

SPRetain::SPRetain(const RegID reg, type::TypePtr type)
	: Instr(InstrKind::SPRetain)
	, reg(reg)
	, type(std::move(type)) {}

SPRelease::SPRelease(const RegID reg)
	: Instr(InstrKind::SPRelease)
	, reg(reg) {}

Term::Term(const TermKind kind)
	: kind(kind) {}

Jump::Jump(const BlockID target)
	: Term(TermKind::Jump)
	, target(target) {}

Branch::Branch(const RegID cond, const BlockID then, const BlockID else_)
	: Term(TermKind::Branch)
	, cond(cond)
	, then(then)
	, else_(else_) {}

Return::Return(const RegID val, type::TypePtr type)
	: Term(TermKind::Return)
	, val(val)
	, type(std::move(type)) {}

BasicBlock::BasicBlock(const BlockID id)
	: id(id) {}

Function::Function(U8String name)
	: name(std::move(name)) {}

Opt<RegID> Function::lookup(const U8String &ident) const {
	for (auto it = m_NamedValues.rbegin(); it != m_NamedValues.rend(); ++it) {
		if (const auto item = it->find(name); item != it->end())
			return item->second;
	}

	return {};
}

void Function::bind(const U8String &ident, RegID reg) {
	m_NamedValues.back().emplace(ident, reg);
}

RegID Function::nextRegID() {
	return ++nextRegId;
}

BasicBlock &Function::createBlock() {
	blocks.push_back(std::make_unique<BasicBlock>(++nextBlockId));
	return *blocks.back();
}

Module::Module(U8String name)
	: name(std::move(name)) {}
}