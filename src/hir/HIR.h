#pragma once
#include "core/Operators.h"
#include "type/Type.h"

namespace hir {
	using LocalID = size_t;

	struct Local {
		LocalID id;
		type::TypePtr type;
	};

	struct Operand {
		virtual ~Operand() = default;
		virtual void toString(std::ostream &os) const = 0;
	};

	struct LocalRef : Operand {
		LocalID id;

		explicit LocalRef(LocalID id);

		virtual void toString(std::ostream &os) const override;
	};

	struct IntLit : Operand {
		i32 value;

		explicit IntLit(i32 value);

		virtual void toString(std::ostream &os) const override;
	};

	struct Stmt {
		virtual ~Stmt() = default;
		virtual void toString(std::ostream &os) const = 0;
	};

	struct StackAlloc : Stmt {
		LocalID dest;
		type::TypePtr type;

		StackAlloc(LocalID dest, type::TypePtr type);

		void toString(std::ostream &os) const override;
	};

	struct HeapAlloc : Stmt {
		LocalID dest;
		Box<Operand> value;
		type::TypePtr type;

		HeapAlloc(LocalID dest, Box<Operand> value, type::TypePtr type);

		void toString(std::ostream &os) const override;
	};

	struct Copy : Stmt {
		LocalID dest;
		Box<Operand> src;

		Copy(LocalID dest, Box<Operand> src);

		void toString(std::ostream &os) const override;
	};

	struct Store : Stmt {
		Box<Operand> dest;
		Box<Operand> value;

		Store(Box<Operand> dest, Box<Operand> value);

		void toString(std::ostream &os) const override;
	};

	struct UnaryOp : Stmt {
		LocalID dest;
		Box<Operand> src;
		UnaryOpKind op;

		UnaryOp(LocalID dest, Box<Operand> src, UnaryOpKind op);

		void toString(std::ostream &os) const override;
	};

	struct BinaryOp : Stmt {
		LocalID dest;
		Box<Operand> left, right;
		BinaryOpKind op;

		BinaryOp(LocalID dest, Box<Operand> left, Box<Operand> right, BinaryOpKind op);

		void toString(std::ostream &os) const override;
	};

	struct Drop : Stmt {
		LocalID target;

		explicit Drop(LocalID target);

		void toString(std::ostream &os) const override;
	};

	struct Terminator {
		virtual ~Terminator() = default;
	};

	struct Return : Terminator {
		Box<Operand> value;
	};

	struct Block;

	struct Jump : Terminator {
		Ptr<Block> target;
	};

	struct CondJump : Terminator {
		LocalID cond;
		Ptr<Block> then, else_;
	};

	struct Block {
		Vec<Box<Stmt>> stmts;
		Box<Terminator> term;

		void addStmt(Box<Stmt> stmt) {
			stmts.push_back(std::move(stmt));
		}
	};

	struct Function {
		U8String name;
		type::TypePtr returnType;
		Vec<Local> locals;
		Vec<Ptr<Block>> blocks;

		LocalID newLocal(type::TypePtr type);
	};
}