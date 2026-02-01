#include "MIR.h"

namespace mir {
Instr::Instr(const InstrKind kind)
	: kind(kind) {}

IntLit::IntLit(const RegisterID dest, const i32 value, type::TypePtr type)
	: Instr(InstrKind::IntLit)
	, dest(dest)
	, value(value)
	, type(std::move(type)) {}

LoadFunc::LoadFunc(const RegisterID dest, U8String funcName, type::TypePtr type)
	: Instr(InstrKind::LoadFunc)
	, dest(dest)
	, funcName(std::move(funcName))
	, type(std::move(type)) {}

CharLit::CharLit(const RegisterID dest, const char32_t value, type::TypePtr type)
	: Instr(InstrKind::CharLit)
	, dest(dest)
	, value(value)
	, type(std::move(type)) {}

UnitLit::UnitLit(const RegisterID dest, type::TypePtr type)
	: Instr(InstrKind::UnitLit)
	, dest(dest)
	, type(std::move(type)) {}

Alloc::Alloc(const RegisterID dest, type::TypePtr type)
	: Instr(InstrKind::Alloc)
	, dest(dest)
	, type(std::move(type)) {}

Load::Load(const RegisterID dest, const RegisterID addr, type::TypePtr type)
	: Instr(InstrKind::Load)
	, dest(dest)
	, addr(addr)
	, type(std::move(type)) {}

Assign::Assign(const RegisterID dest, const RegisterID src)
	: Instr(InstrKind::Assign)
	, dest(dest)
	, src(src) {}

Call::Call(RegisterID dest, RegisterID callee, Vec<RegisterID> args, type::TypePtr type)
	: Instr(InstrKind::Call)
	, dest(dest)
	, callee(callee)
	, args(std::move(args))
	, type(std::move(type)) {}

BinaryOp::BinaryOp(const RegisterID dest, const RegisterID left, const RegisterID right,
				   const BinaryOpKind op, type::TypePtr type)
	: Instr(InstrKind::BinaryOp)
	, dest(dest)
	, left(left)
	, right(right)
	, op(op)
	, type(std::move(type)) {}

UnaryOp::UnaryOp(const RegisterID dest, const RegisterID operand, const UnaryOpKind op,
				 type::TypePtr type)
	: Instr(InstrKind::UnaryOp)
	, dest(dest)
	, operand(operand)
	, op(op)
	, type(std::move(type)) {}

SPCreate::SPCreate(const RegisterID reg, type::TypePtr type)
	: Instr(InstrKind::SPCreate)
	, reg(reg)
	, type(std::move(type)) {}

SPRetain::SPRetain(const RegisterID reg, type::TypePtr type)
	: Instr(InstrKind::SPRetain)
	, reg(reg)
	, type(std::move(type)) {}

SPRelease::SPRelease(const RegisterID reg)
	: Instr(InstrKind::SPRelease)
	, reg(reg) {}

Term::Term(const TermKind kind)
	: kind(kind) {}

Jump::Jump(const BlockID target)
	: Term(TermKind::Jump)
	, target(target) {}

Branch::Branch(const RegisterID cond, const BlockID then, const BlockID else_)
	: Term(TermKind::Branch)
	, cond(cond)
	, then(then)
	, else_(else_) {}

Return::Return(const RegisterID val, type::TypePtr type)
	: Term(TermKind::Return)
	, val(val)
	, type(std::move(type)) {}

BasicBlock::BasicBlock(const BlockID id)
	: id(id) {}

Function::Function(U8String name)
	: name(std::move(name)) {}

RegisterID Function::nextRegisterID() {
	return ++nextRegId;
}

BasicBlock &Function::createBlock() {
	blocks.push_back(std::make_unique<BasicBlock>(++nextBlockId));
	return *blocks.back();
}

Module::Module(U8String name)
	: name(std::move(name)) {}
}