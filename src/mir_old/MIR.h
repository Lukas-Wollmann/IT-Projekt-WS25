#pragma once
#include "core/Operators.h"
#include "type/Type.h"

namespace mir_old {
using RegisterID = u32;
using BlockID = u32;

enum struct InstrKind {
	IntLit,
	CharLit,
	UnitLit,
	Alloc,
	Load,
	Assign,
	Call, // Added missing kind
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
	const RegisterID dest;
	const i32 value;
	const type::TypePtr type;
	IntLit(RegisterID dest, i32 value, type::TypePtr type);
};

struct LoadFunc : Instr {
	const RegisterID dest;
	const U8String funcName;
	const type::TypePtr type;

	LoadFunc(RegisterID dest, U8String funcName, type::TypePtr type);
};

struct CharLit : Instr {
	const RegisterID dest;
	const char32_t value;
	const type::TypePtr type;
	CharLit(RegisterID dest, char32_t value, type::TypePtr type);
};

struct UnitLit : Instr {
	const RegisterID dest;
	const type::TypePtr type;
	UnitLit(RegisterID dest, type::TypePtr type);
};

struct Alloc : Instr {
	const RegisterID dest;
	const type::TypePtr type;
	Alloc(RegisterID dest, type::TypePtr type);
};

struct Load : Instr {
	const RegisterID dest;
	const RegisterID addr;
	const type::TypePtr type;
	Load(RegisterID dest, RegisterID addr, type::TypePtr type);
};

struct Assign : Instr {
	const RegisterID dest;
	const RegisterID src;
	Assign(RegisterID dest, RegisterID src);
};

// Added Call Struct
struct Call : Instr {
	const RegisterID dest;
	const RegisterID callee;
	const Vec<RegisterID> args;
	const type::TypePtr type;
	Call(RegisterID dest, RegisterID callee, Vec<RegisterID> args, type::TypePtr type);
};

struct BinaryOp : Instr {
	const RegisterID dest, left, right;
	const BinaryOpKind op;
	const type::TypePtr type;
	BinaryOp(RegisterID dest, RegisterID left, RegisterID right, BinaryOpKind op,
			 type::TypePtr type);
};

struct UnaryOp : Instr {
	const RegisterID dest, operand;
	const UnaryOpKind op;
	const type::TypePtr type;
	UnaryOp(RegisterID dest, RegisterID operand, UnaryOpKind op, type::TypePtr type);
};

struct SPCreate : Instr {
	const RegisterID reg;
	const type::TypePtr type;
	SPCreate(RegisterID reg, type::TypePtr type);
};

struct SPRetain : Instr {
	const RegisterID reg;
	const type::TypePtr type;
	SPRetain(RegisterID reg, type::TypePtr type);
};

struct SPRelease : Instr {
	const RegisterID reg;
	SPRelease(RegisterID reg);
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
	const RegisterID cond;
	const BlockID then;
	const BlockID else_;
	Branch(RegisterID cond, BlockID then, BlockID else_);
};

struct Return : Term {
	const RegisterID val;
	const type::TypePtr type;
	Return(RegisterID val, type::TypePtr type);
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
	Vec<RegisterID> params;
	Vec<Box<BasicBlock>> blocks;

	u32 nextRegId = 0;
	u32 nextBlockId = 0;
	Function(U8String name);

	RegisterID nextRegisterID();
	BasicBlock &createBlock();
};

struct Module {
	const U8String name;
	Vec<Box<Function>> funcs;
	explicit Module(U8String name);
};
}