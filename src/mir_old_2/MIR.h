#pragma once
#include "core/Operators.h"
#include "type/Type.h"

namespace mir {
using RegID = u32;
using BlockID = u32;

enum struct InstrKind {
	IntLit,
	BoolLit,
	CharLit,
	UnitLit,
	Alloc,
	Load,
	Assign,
	Call,
	LoadFunc,
	BinaryOp,
	UnaryOp,
	SPCreate,
	SPRetain,
	SPRelease
};

struct Instr {
	const InstrKind kind;
	virtual ~Instr() = default;

protected:
	explicit Instr(InstrKind kind);
};

struct IntLit : Instr {
	const RegID dest;
	const i32 value;
	const type::TypePtr type;
	IntLit(RegID dest, i32 value);
};

struct BoolLit : Instr {
	const RegID dest;
	const bool value;
	BoolLit(RegID dest, bool value);
};

struct CharLit : Instr {
	const RegID dest;
	const char32_t value;
	const type::TypePtr type;
	CharLit(RegID dest, char32_t value);
};

struct UnitLit : Instr {
	const RegID dest;
	const type::TypePtr type;
	UnitLit(RegID dest);
};

struct LoadFunc : Instr {
	const RegID dest;
	const U8String funcName;
	const type::TypePtr type;

	LoadFunc(RegID dest, U8String funcName, type::TypePtr type);
};

struct Alloc : Instr {
	const RegID dest;
	const type::TypePtr type;
	Alloc(RegID dest, type::TypePtr type);
};

struct Load : Instr {
	const RegID dest;
	const RegID addr;
	const type::TypePtr type;
	Load(RegID dest, RegID addr, type::TypePtr type);
};

struct Store : Instr {
	const RegID dest;
	const RegID src;
	Store(RegID dest, RegID src);
};

struct Call : Instr {
	const RegID dest;
	const RegID callee;
	const Vec<RegID> args;
	Call(RegID dest, RegID callee, Vec<RegID> args);
};

struct BinaryOp : Instr {
	const RegID dest, left, right;
	const BinaryOpKind op;
	BinaryOp(RegID dest, RegID left, RegID right, BinaryOpKind op);
};

struct UnaryOp : Instr {
	const RegID dest, operand;
	const UnaryOpKind op;
	UnaryOp(RegID dest, RegID operand, UnaryOpKind op);
};

struct Construct : Instr {
	const RegID reg;
	const type::TypePtr type;
	Construct(RegID reg, type::TypePtr type);
};

struct Copy : Instr {
	const RegID reg;
	const type::TypePtr type;
	Copy(RegID reg, type::TypePtr type);
};

struct Destruct : Instr {
	const RegID reg;
	type::TypePtr type;
	Destruct(RegID reg, type::TypePtr type);
};

enum struct TermKind { Jump, Branch, Return };

struct Term {
	const TermKind kind;
	virtual ~Term() = default;

protected:
	explicit Term(TermKind kind);
};

struct Jump : Term {
	const BlockID target;
	explicit Jump(BlockID target);
};

struct Branch : Term {
	const RegID cond;
	const BlockID then;
	const BlockID else_;
	Branch(RegID cond, BlockID then, BlockID else_);
};

struct Return : Term {
	const RegID val;
	Return(RegID val);
};

struct BasicBlock {
	const BlockID id;
	Vec<Box<Instr>> instrs;

	Box<Term> term;
	BasicBlock(BlockID id);

	bool isTerminated() {
		return term != nullptr;
	}
};

struct Function {
	const U8String name;
	Vec<RegID> params;
	Vec<Ptr<BasicBlock>> blocks;

	u32 nextRegId = 0;
	u32 nextBlockId = 0;
	Function(U8String name);

	RegID nextRegID();
	Ptr<BasicBlock> createBlock();
};

struct Module {
	const U8String name;
	Vec<Ptr<Function>> funcs;
	explicit Module(U8String name);
};
}