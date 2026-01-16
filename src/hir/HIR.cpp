#include "HIR.h"

#include "type/Printer.h"

namespace hir {
	LocalID Function::newLocal(type::TypePtr type) {
		LocalID id = locals.size();
		locals.push_back(Local{id, std::move(type)});
		return id;
	}

	LocalRef::LocalRef(LocalID id)
		: id(id) {}

	void LocalRef::toString(std::ostream &os) const {
		os << "LocalRef(" << id << ")";
	}

	IntLit::IntLit(i32 value)
		: value(value) {}

	void IntLit::toString(std::ostream &os) const {
		os << "IntLit(" << value << ")";
	}

	StackAlloc::StackAlloc(LocalID dest, type::TypePtr type)
		: dest(dest)
		, type(std::move(type)) {}

	void StackAlloc::toString(std::ostream &os) const {
		os << "StackAlloc(" << dest << ", " << *type << ")";
	}

	HeapAlloc::HeapAlloc(LocalID dest, Box<Operand> value, type::TypePtr type)
		: dest(dest)
		, value(std::move(value))
		, type(std::move(type)) {}

	void HeapAlloc::toString(std::ostream &os) const {
		os << "HeapAlloc(" << dest << ", ";
		value->toString(os);
		os << ", " << *type << ")";
	}

	Copy::Copy(LocalID dest, Box<Operand> src)
		: dest(dest)
		, src(std::move(src)) {}

	void Copy::toString(std::ostream &os) const {
		os << "Copy(" << dest << ", ";
		src->toString(os);
		os << ")";
	}

	Store::Store(Box<Operand> dest, Box<Operand> value)
		: dest(std::move(dest))
		, value(std::move(value)) {}

	void Store::toString(std::ostream &os) const {
		os << "Store(";
		dest->toString(os);
		os << ", ";
		value->toString(os);
		os << ")";
	}

	UnaryOp::UnaryOp(LocalID dest, Box<Operand> src, UnaryOpKind op)
		: dest(dest)
		, src(std::move(src))
		, op(op) {}

	void UnaryOp::toString(std::ostream &os) const {
		os << "UnaryOp(" << dest << ", ";
		src->toString(os);
		os << ", " << op << ")";
	}

	BinaryOp::BinaryOp(LocalID dest, Box<Operand> left, Box<Operand> right, BinaryOpKind op)
		: dest(dest)
		, left(std::move(left))
		, right(std::move(right))
		, op(op) {}

	void BinaryOp::toString(std::ostream &os) const {
		os << "BinaryOp(" << dest << ", ";
		left->toString(os);
		os << ", ";
		right->toString(os);
		os << ", " << op << ")";
	}

	Drop::Drop(LocalID target)
		: target(target) {}

	void Drop::toString(std::ostream &os) const {
		os << "Drop(" << target << ")";
	}
}
