#pragma once

#include <variant>

#include "core/Typedef.h"
#include "core/U8String.h"
#include "type/Type.h"

namespace mir {

// Represents a unique identifier for a variable, parameter, or temporary register
using Id = U8String;

// The basic operations of the MIR.
// These are more explicit than AST nodes to facilitate RAII and Codegen.
enum struct InstKind {
	// Memory & Ownership
	Retain,	 // Increment Reference Count (inc_ref)
	Release, // Decrement Reference Count & Drop if zero (dec_ref)

	// Assignment & Data Flow
	Assign,	 // t1 = t2
	LoadLit, // t1 = constant

	// Computation
	UnaryOp,  // t1 = op t2
	BinaryOp, // t1 = t2 op t3

	// Subroutines
	Call, // t1 = call func(args...)

	// Control Flow (Terminators)
	Jump,	// unconditional goto label
	Branch, // if t1 goto label_true else label_false
	Return	// exit function with value t1
};

// Represents a single flattened instruction
struct Instruction {
	InstKind kind;
	Opt<Id> dest;		// Destination register (if any)
	Vec<Id> args;		// Source registers/operands
	type::TypePtr type; // Type of the result/operation
};

// A Basic Block is a linear sequence of instructions.
// It ends with a terminator (Jump, Branch, or Return).
struct BasicBlock {
	Id label;
	Vec<Instruction> instructions;

	explicit BasicBlock(Id label)
		: label(std::move(label)) {}

	// Helper to check if the block already has a terminating instruction
	[[nodiscard]] bool isTerminated() const {
		if (instructions.empty())
			return false;
		auto k = instructions.back().kind;
		return k == InstKind::Jump || k == InstKind::Branch || k == InstKind::Return;
	}
};

// Represents a linearized function with its own control flow graph (CFG)
struct Function {
	Id name;
	type::TypePtr returnType;
	Vec<Id> params;				 // Parameter names
	Vec<Box<BasicBlock>> blocks; // The actual code graph

	u32 tempCounter = 0; // Generator for t0, t1, t2...

	Function(Id name, type::TypePtr retType)
		: name(std::move(name))
		, returnType(std::move(retType)) {}

	// Helper to generate unique temporary register names
	Id nextTemp() {
		return "t" + std::to_string(tempCounter++);
	}
};

// The top-level container for the linearized program
struct Module {
	Id name;
	Vec<Box<Function>> functions;

	explicit Module(Id name)
		: name(std::move(name)) {}
};

} // namespace mir