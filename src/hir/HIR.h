#pragma once
#include "core/Operators.h"
#include "type/Type.h"

namespace hir {
	using LocalID = size_t;

	struct Slot {
		LocalID id;
		type::TypePtr type;
		Opt<U8String> name;
	};

	struct RValue {
		type::TypePtr type;

		virtual ~RValue() = default;
	};

	struct IntLiteral : RValue {
		i32 value;
	};

	struct UnaryOp : RValue {
		UnaryOpKind op;
		Slot slot;
	};

	struct BinaryOp : RValue {
		BinaryOpKind op;
		Slot left, right;
	};

	struct Stmt {
		virtual ~Stmt() = default;
	};

	struct Assignment : Stmt {
		Slot slot;
		Box<RValue> value;
	};

	struct Drop : Stmt {
		Slot slot;
	};

	struct Block {
		Vec<Box<Stmt>> stmts;
	};

	struct FuncDecl {
		U8String name;
		Vec<Slot> params;
		Vec<Block> blocks;
	};
}
