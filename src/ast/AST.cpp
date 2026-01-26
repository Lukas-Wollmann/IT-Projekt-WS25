#include "AST.h"

#include "Macros.h"
#include "core/U8String.h"
#include "type/Printer.h"

namespace ast {
	Node::Node(const NodeKind kind)
		: kind(kind) {}

	Stmt::Stmt(const NodeKind kind)
		: Node(kind) {}

	Expr::Expr(const NodeKind kind)
		: Stmt(kind) {}

	void Expr::infer(type::TypePtr type, ValueCategory category) {
		inferredType = std::move(type);
		valueCategory = category;
	}

	bool Expr::isInferred() const {
		return inferredType.has_value() && valueCategory.has_value();
	}

	IntLit::IntLit(const i32 value)
		: Expr(NodeKind::IntLit)
		, value(value) {}

	CharLit::CharLit(const char32_t value)
		: Expr(NodeKind::CharLit)
		, value(value) {}

	BoolLit::BoolLit(const bool value)
		: Expr(NodeKind::BoolLit)
		, value(value) {}

	UnitLit::UnitLit()
		: Expr(NodeKind::UnitLit) {}

	UnaryExpr::UnaryExpr(const UnaryOpKind op, Box<Expr> operand)
		: Expr(NodeKind::UnaryExpr)
		, op(op)
		, operand(std::move(operand)) {}

	BinaryExpr::BinaryExpr(const BinaryOpKind op, Box<Expr> left, Box<Expr> right)
		: Expr(NodeKind::BinaryExpr)
		, op(op)
		, left(std::move(left))
		, right(std::move(right)) {}

	Assignment::Assignment(const AssignmentKind assignmentKind, Box<Expr> left, Box<Expr> right)
		: Expr(NodeKind::Assignment)
		, assignmentKind(assignmentKind)
		, left(std::move(left))
		, right(std::move(right)) {}

	VarRef::VarRef(U8String ident)
		: Expr(NodeKind::VarRef)
		, ident(std::move(ident)) {}

	FuncCall::FuncCall(Box<Expr> expr, Vec<Box<Expr>> args)
		: Expr(NodeKind::FuncCall)
		, expr(std::move(expr))
		, args(std::move(args)) {}

	BlockStmt::BlockStmt(Vec<Box<Stmt>> stmts)
		: Stmt(NodeKind::BlockStmt)
		, stmts(std::move(stmts)) {}

	IfStmt::IfStmt(Box<Expr> cond, Box<BlockStmt> then, Box<BlockStmt> else_)
		: Stmt(NodeKind::IfStmt)
		, cond(std::move(cond))
		, then(std::move(then))
		, else_(std::move(else_)) {}

	WhileStmt::WhileStmt(Box<Expr> cond, Box<BlockStmt> body)
		: Stmt(NodeKind::WhileStmt)
		, cond(std::move(cond))
		, body(std::move(body)) {}

	ReturnStmt::ReturnStmt(Box<Expr> expr)
		: Stmt(NodeKind::ReturnStmt)
		, expr(std::move(expr)) {}

	VarDef::VarDef(U8String ident, type::TypePtr type, Box<Expr> value)
		: Stmt(NodeKind::VarDef)
		, ident(std::move(ident))
		, type(std::move(type))
		, value(std::move(value)) {}

	FuncDecl::FuncDecl(U8String ident, Vec<Param> params, type::TypePtr returnType,
					   Box<BlockStmt> body)
		: Node(NodeKind::FuncDecl)
		, ident(std::move(ident))
		, params(std::move(params))
		, returnType(std::move(returnType))
		, body(std::move(body)) {}

	Module::Module(U8String name, Vec<Box<FuncDecl>> decls)
		: Node(NodeKind::Module)
		, name(std::move(name))
		, decls(std::move(decls)) {}

	static void printNodeLine(std::ostream &os, const U8String &prefix, bool isLast,
							  const U8String &text) {
		os << prefix << (isLast ? "└─ " : "├─ ") << text << '\n';
	}

	static U8String childPrefix(const U8String &prefix, bool isLast) {
		return prefix + (isLast ? u8"   " : u8"│  ");
	}

	void IntLit::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		U8String text("IntLit(" + std::to_string(value) + ")");
		printNodeLine(os, prefix, isLast, text);
	}

	void CharLit::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		auto text = u8"CharLit('" + U8String(value) + u8"')";
		printNodeLine(os, prefix, isLast, text);
	}

	void BoolLit::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		auto text = U8String("BoolLit(") + (value ? u8"true" : u8"false") + u8")";
		printNodeLine(os, prefix, isLast, text);
	}

	void UnitLit::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, u8"UnitLit()");
	}

	void UnaryExpr::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		std::stringstream ss;
		ss << "UnaryExpr(" << op << ")";

		printNodeLine(os, prefix, isLast, U8String(ss.str()));
		operand->print(os, childPrefix(prefix, isLast), true);
	}

	void BinaryExpr::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		std::stringstream ss;
		ss << "BinaryExpr(" << op << ")";

		printNodeLine(os, prefix, isLast, U8String(ss.str()));
		left->print(os, childPrefix(prefix, isLast), false);
		right->print(os, childPrefix(prefix, isLast), true);
	}

	void Assignment::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		std::stringstream ss;
		ss << "Assignment(" << assignmentKind << ")";

		printNodeLine(os, prefix, isLast, U8String(ss.str()));
		left->print(os, childPrefix(prefix, isLast), false);
		right->print(os, childPrefix(prefix, isLast), true);
	}

	void VarRef::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, U8String("VarRef(") + ident + u8")");
	}

	void FuncCall::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, u8"FuncCall");
		const auto np = childPrefix(prefix, isLast);

		printNodeLine(os, np, args.empty(), u8"Callee");
		expr->print(os, childPrefix(np, args.empty()), true);

		if (!args.empty()) {
			printNodeLine(os, np, true, u8"Args");
			const auto ap = childPrefix(np, true);

			for (size_t i = 0; i < args.size(); ++i)
				args[i]->print(os, ap, i + 1 == args.size());
		}
	}

	void BlockStmt::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, u8"BlockStmt");
		const auto np = childPrefix(prefix, isLast);

		for (size_t i = 0; i < stmts.size(); ++i)
			stmts[i]->print(os, np, i + 1 == stmts.size());
	}

	void IfStmt::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, u8"IfStmt");
		const auto np = childPrefix(prefix, isLast);

		printNodeLine(os, np, false, u8"Cond");
		cond->print(os, childPrefix(np, false), true);

		printNodeLine(os, np, false, u8"Then");
		then->print(os, childPrefix(np, false), true);

		printNodeLine(os, np, true, u8"Else");
		else_->print(os, childPrefix(np, true), true);
	}

	void WhileStmt::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, u8"WhileStmt");
		const auto np = childPrefix(prefix, isLast);

		printNodeLine(os, np, false, u8"Cond");
		cond->print(os, childPrefix(np, false), true);

		printNodeLine(os, np, true, u8"Body");
		body->print(os, childPrefix(np, true), true);
	}

	void ReturnStmt::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, u8"ReturnStmt");
		expr->print(os, childPrefix(prefix, isLast), true);
	}

	void VarDef::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		std::stringstream ss;
		ss << "VarDef(" << ident << ", " << *type << ", " << *value << ")";

		printNodeLine(os, prefix, isLast, U8String(ss.str()));
		const auto np = childPrefix(prefix, isLast);
		printNodeLine(os, np, true, u8"Value");
		value->print(os, childPrefix(np, true), true);
	}

	void FuncDecl::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, U8String("FuncDecl(") + ident + u8")");
		const auto np = childPrefix(prefix, isLast);

		if (!params.empty()) {
			printNodeLine(os, np, false, u8"Params");
			const auto pp = childPrefix(np, false);

			for (size_t i = 0; i < params.size(); ++i) {
				const bool last = (i + 1 == params.size());
				std::stringstream ss;
				ss << params[i].first << ": " << *params[i].second;

				printNodeLine(os, pp, last, U8String(ss.str()));
			}
		}

		std::stringstream ss;
		ss << "ReturnType: " << *returnType;

		printNodeLine(os, np, false, U8String(ss.str()));
		printNodeLine(os, np, true, u8"Body");
		body->print(os, childPrefix(np, true), true);
	}

	void Module::print(std::ostream &os, const U8String &prefix, bool isLast) const {
		printNodeLine(os, prefix, isLast, U8String("Module(") + name + u8")");
		const auto np = childPrefix(prefix, isLast);

		for (size_t i = 0; i < decls.size(); ++i)
			decls[i]->print(os, np, i + 1 == decls.size());
	}

	std::ostream &operator<<(std::ostream &os, const NodeKind kind) {
		using enum NodeKind;

		switch (kind) {
			case IntLit:	 return os << "IntLit";
			case CharLit:	 return os << "CharLit";
			case BoolLit:	 return os << "BoolLit";
			case UnaryExpr:	 return os << "UnaryExpr";
			case BinaryExpr: return os << "BinaryExpr";
			case FuncCall:	 return os << "FuncCall";
			case VarRef:	 return os << "VarRef";
			case BlockStmt:	 return os << "BlockStmt";
			case IfStmt:	 return os << "IfStmt";
			case WhileStmt:	 return os << "WhileStmt";
			case ReturnStmt: return os << "ReturnStmt";
			case VarDef:	 return os << "VarDef";
			case FuncDecl:	 return os << "FuncDecl";
			case Module:	 return os << "Module";
			default:		 UNREACHABLE();
		}
	}

	std::ostream &operator<<(std::ostream &os, const AssignmentKind kind) {
		using enum AssignmentKind;

		switch (kind) {
			case Simple:		 return os << "=";
			case Addition:		 return os << "+=";
			case Subtraction:	 return os << "-=";
			case Multiplication: return os << "*=";
			case Division:		 return os << "/=";
			case Modulo:		 return os << "%=";
			case BitwiseAnd:	 return os << "&=";
			case BitwiseOr:		 return os << "|=";
			case BitwiseXor:	 return os << "^=";
			case LeftShift:		 return os << "<<=";
			case RightShift:	 return os << ">>=";
			default:			 UNREACHABLE();
		}
	}

	std::ostream &operator<<(std::ostream &os, const ValueCategory cat) {
		using enum ValueCategory;

		switch (cat) {
			case LValue: return os << "LValue";
			case RValue: return os << "RValue";
			default:	 UNREACHABLE();
		}
	}

	std::ostream &operator<<(std::ostream &os, const Node &n) {
		n.print(os);

		return os;
	}
}