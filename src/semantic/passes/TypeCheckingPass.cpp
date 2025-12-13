#include "TypeCheckingPass.h"

#include <sstream>

#include "Macros.h"
#include "type/CloneVisitor.h"
#include "type/CompareVisitor.h"
#include "type/PrintVisitor.h"

namespace semantic {
	using namespace type;
	using namespace ast;

	TypeCheckingPass::TypeCheckingPass(TypeCheckerContext &context)
		: m_Context(context) {
		m_SymbolTable.enterScope();
	}

	TypeCheckingPass::~TypeCheckingPass() {
		m_SymbolTable.exitScope();
	}

	bool TypeCheckingPass::visit(IntLit &n) {
		VERIFY(!n.inferredType);

		n.inferredType = std::make_unique<Typename>(u8"i32");

		return false;
	}

	bool TypeCheckingPass::visit(FloatLit &n) {
		VERIFY(!n.inferredType);

		n.inferredType = std::make_unique<Typename>(u8"f32");

		return false;
	}

	bool TypeCheckingPass::visit(CharLit &n) {
		VERIFY(!n.inferredType);

		n.inferredType = std::make_unique<Typename>(u8"char");

		return false;
	}

	bool TypeCheckingPass::visit(BoolLit &n) {
		VERIFY(!n.inferredType);

		n.inferredType = std::make_unique<Typename>(u8"bool");
		return false;
	}

	bool TypeCheckingPass::visit(StringLit &n) {
		VERIFY(!n.inferredType);

		n.inferredType = std::make_unique<Typename>(u8"string");

		return false;
	}

	bool TypeCheckingPass::visit(ArrayExpr &n) {
		size_t arraySize = n.values.size();
		n.inferredType = std::make_unique<ArrayType>(clone(*n.elementType), arraySize);

		for (auto &expr : n.values) {
			dispatch(*expr);
			VERIFY(expr->inferredType);
			auto &type = *expr->inferredType;

			if (type->kind != TypeKind::Error && *type != *n.elementType) {
				std::stringstream ss;
				ss << "Expected an object of type '" << *n.elementType << "'";
				ss << " for an array - found an object of type '" << *type << "' instead.";
				m_Context.addError(ss.str());
			}
		}

		return false;
	}

	bool TypeCheckingPass::visit(UnaryExpr &n) {
		dispatch(*n.operand);
		VERIFY(n.operand->inferredType);

		auto &type = **n.operand->inferredType;

		if (type.kind == TypeKind::Error) {
			n.inferredType = clone(type);

			return false;
		}

		// Dereference needs special handling for now
		if (n.op == UnaryOpKind::Dereference) {
			if (type.kind != TypeKind::Pointer) {
				std::stringstream ss;
				ss << "Canno't dereference a value of type " << type;
				ss << " expected a some kind of pointer type.";

				m_Context.addError(ss.str());
				n.inferredType = std::make_unique<ErrorType>();

				return false;
			}

			auto &ptrType = static_cast<const PointerType &>(type);
			n.inferredType = clone(*ptrType.pointeeType);

			return false;
		}

		std::stringstream opIdent;
		opIdent << "operator" << n.op << "<" << type << ">";

		auto func = m_Context.getGlobalNamespace().getFunction(U8String(opIdent.str()));

		if (!func) {
			std::stringstream ss;
			ss << "Found no binary operator with for ";
			ss << n.op << " with types " << *type;

			m_Context.addError(ss.str());
			n.inferredType = std::make_unique<ErrorType>();

			return false;
		}

		canArgsCallFunc({*type}, *func);
		n.inferredType = clone(*func->get().returnType);

		return false;
	}

	///
	/// While type checking, unary expressions try to call their respective operator as if it was a
	/// normal function. For example "1 + 2" would look for an overloaded function in the current
	/// namespace: "operator<int,int>+(int, int)". This makes it very easy to add operator
	/// overloading later. This is type checking only, so no extra function calls will be inserted
	/// for trivial addition overloads in the code generaition phase later on.
	///
	bool TypeCheckingPass::visit(BinaryExpr &n) {
		// Infer the type of the left and right operand expressions
		dispatch(*n.left);
		dispatch(*n.right);

		VERIFY(n.left->inferredType);
		VERIFY(n.right->inferredType);

		auto &leftType = *n.left->inferredType;
		auto &rightType = *n.right->inferredType;

		// If one of the two expressions is of type <error-type>
		// then propagate the error upwards in the type tree.
		// The error did not really happen here, so dont add to m_Errors.
		if (leftType->kind == TypeKind::Error || rightType->kind == TypeKind::Error) {
			n.inferredType = std::make_unique<ErrorType>();

			return false;
		}

		std::stringstream opIdent;
		opIdent << "operator" << n.op << "<" << *leftType << "," << *rightType << ">";

		auto func = m_Context.getGlobalNamespace().getFunction(U8String(opIdent.str()));

		if (!func) {
			std::stringstream ss;
			ss << "Found no binary operator with for ";
			ss << n.op << " with types ";
			ss << *leftType << " and " << *rightType;

			m_Context.addError(ss.str());
			n.inferredType = std::make_unique<ErrorType>();

			return false;
		}

		canArgsCallFunc({*leftType, *rightType}, *func);
		n.inferredType = clone(*func->get().returnType);

		return false;
	}

	bool TypeCheckingPass::visit(ast::HeapAlloc &n) {
		dispatch(*n.value);
		VERIFY(n.value->inferredType);

        auto &type = **n.value->inferredType;

        if (type.kind == TypeKind::Error) {
            n.inferredType = std::make_unique<ErrorType>();

            return false;
        }

		n.inferredType = std::make_unique<PointerType>(clone(**n.value->inferredType));

		return false;
	}

	bool TypeCheckingPass::visit(Assignment &n) {
		// Assignments dont return anything, we dont have references so we
		// would have to add a lot of special handling, its not worth for now
		n.inferredType = std::make_unique<UnitType>();

		dispatch(*n.left);
		dispatch(*n.right);

		VERIFY(n.left->inferredType);
		VERIFY(n.right->inferredType);

		if (!isAssignable(*n.left)) {
			std::stringstream ss;
			ss << "Cannot assign to r-value.";

			m_Context.addError(ss.str());
			n.inferredType = std::make_unique<ErrorType>();

			return false;
		}

		auto &leftType = *n.left->inferredType;
		auto &rightType = *n.right->inferredType;

		// If one of the two expressions is of type <error-type>,
		// the error did not really happen here, so dont add to m_Errors.
		if (leftType->kind == TypeKind::Error || rightType->kind == TypeKind::Error)
			return false;

		// If both types are equal, its okay
		if (*leftType == *rightType)
			return false;

		// Here an actual error happenes. Add the error and make the type of
		// the binary expression an <error-type> to mark the subtree as invalid.
		std::stringstream ss;
		ss << "Found illegal assignment expression: ";
		ss << n.assignmentKind << " cannot be used with ";
		ss << *leftType << " and " << *rightType;

		m_Context.addError(ss.str());

		return false;
	}

	bool TypeCheckingPass::visit(FuncCall &n) {
		dispatch(*n.expr);
		VERIFY(n.expr->inferredType);

		auto &type = **n.expr->inferredType;

		if (type.kind != TypeKind::Function) {
			if (type.kind != TypeKind::Error) {
				std::stringstream ss;
				ss << "Cannot call a non-function-type expression";
				m_Context.addError(ss.str());
			}

			n.inferredType = std::make_unique<ErrorType>();

			return false;
		}

		auto &funcType = static_cast<const FunctionType &>(type);
		Vec<Ref<const Type>> args;

		for (auto &expr : n.args) {
			dispatch(*expr);
			VERIFY(expr->inferredType);

			args.push_back(**expr->inferredType);
		}

		canArgsCallFunc(args, funcType);
		n.inferredType = clone(*funcType.returnType);

		return false;
	}

	bool TypeCheckingPass::visit(VarRef &n) {
		auto symbol = m_SymbolTable.getSymbol(n.ident);

		// The symbol is known, so take its type
		if (symbol) {
			n.inferredType = clone(symbol->get().getType());

			return false;
		}

		// If the symbol is not known, it maybe be a function
		auto func = m_Context.getGlobalNamespace().getFunction(n.ident);

		if (func) {
			n.inferredType = clone(func->get());

			return false;
			;
		}

		// The symbol is unknown, so throw an error
		std::stringstream ss;
		ss << "Undefined identifier: " << n.ident;
		m_Context.addError(ss.str());
		n.inferredType = std::make_unique<ErrorType>();

		return false;
	}

	bool TypeCheckingPass::visit(BlockStmt &n) {
		m_SymbolTable.enterScope();

		bool foundReturn = false;

		for (size_t i = 0; i < n.stmts.size(); ++i) {
			bool didReturn = dispatch(*n.stmts[i]);

			if (foundReturn && didReturn) {
				std::stringstream ss;
				ss << "Unreachable statements detected after a return statement.";
				m_Context.addError(ss.str());
			}

			foundReturn |= didReturn;
		}

		m_SymbolTable.exitScope();

		return foundReturn;
	}

	bool TypeCheckingPass::visit(IfStmt &n) {
		dispatch(*n.cond);
		VERIFY(*n.cond->inferredType);

		auto &type = **n.cond->inferredType;

		// If the condition has type <error-type> fail silently
		if (type.kind != TypeKind::Error && type != Typename(u8"bool")) {
			std::stringstream ss;
			ss << "Cannot accept type " << type << " inside an if condition.";

			m_Context.addError(ss.str());
		}

		bool thenReturns = dispatch(*n.then);
		bool elseReturns = dispatch(*n.else_);

		return thenReturns && elseReturns;
	}

	bool TypeCheckingPass::visit(WhileStmt &n) {
		dispatch(*n.cond);
		VERIFY(*n.cond->inferredType);

		auto &type = **n.cond->inferredType;

		// If the condition has type <error-type> fail silently
		if (type.kind != TypeKind::Error && type != Typename(u8"bool")) {
			std::stringstream ss;
			ss << "Cannot accept type " << type << " inside a while condition.";

			m_Context.addError(ss.str());
		}

		dispatch(*n.body);

		return false;
	}

	bool TypeCheckingPass::visit(ReturnStmt &n) {
		VERIFY(m_CurrentFunctionReturnType);

		// TODO: Add bool type and return types with no values
		VERIFY(n.expr);

		dispatch(**n.expr);
		VERIFY(n.expr->get()->inferredType);

		auto &type = **n.expr->get()->inferredType;

		// If the type is <error-type> or if the return type matches
		// the function declaration, its okay and return
		if (type.kind == TypeKind::Error || type == *m_CurrentFunctionReturnType)
			return true;

		// The return type is not matching the function declaration
		std::stringstream ss;
		ss << "The type " << type << " does not match function declaration. ";
		ss << "Expected type: " << *m_CurrentFunctionReturnType;

		m_Context.addError(ss.str());

		return true;
	}

	bool TypeCheckingPass::visit(VarDef &n) {
		// Infer the type of the assigned expression
		dispatch(*n.value);
		VERIFY(n.value->inferredType);

		auto &type = **n.value->inferredType;

		// The expression is not of type <error-type> but does not match type
		// type of the variable declaration - this is an actual error
		if (type.kind != TypeKind::Error && type != *n.type) {
			std::stringstream ss;
			ss << "Missmatchting types at variable declaration: ";
			ss << "Expected " << *n.type << " but got " << type;

			m_Context.addError(ss.str());
		}

		// Does this symbol already exist in the current scope (shadowing possible)
		if (m_SymbolTable.isSymbolDefinedInCurrentScope(n.ident)) {
			std::stringstream ss;
			ss << "Illegal redefinition of symbol: " << n.ident;

			m_Context.addError(ss.str());
			return false;
		}

		m_SymbolTable.addSymbol(n.ident, SymbolInfo(clone(*n.type)));

		return false;
	}

	bool TypeCheckingPass::visit(FuncDecl &n) {
		m_SymbolTable.enterScope();

		// Add all params as symbols to the function scope
		for (auto &param : n.params)
			m_SymbolTable.addSymbol(param.first, SymbolInfo(clone(*param.second)));

		// Set the current expected return type

		m_CurrentFunctionReturnType = clone(*n.returnType);

		// Type check the function body (in a nested scope, allows param shadowing)
		bool doesReturn = dispatch(*n.body);

		if (!doesReturn) {
			std::stringstream ss;
			ss << "Not all control flow paths in function '" << n.ident << "' return a value.";
			m_Context.addError(ss.str());
		}

		m_SymbolTable.exitScope();

		// We already explored this function during the exploration pass, dont
		// add it to the symbol table a second time, that would break it.
		return false;
	}

	bool TypeCheckingPass::visit(Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);

		return false;
	}

	bool TypeCheckingPass::isAssignable(Expr &e) const {
		if (e.kind == NodeKind::VarRef)
			return true;

		if (e.kind != NodeKind::UnaryExpr)
			return false;

		auto &unary = static_cast<const UnaryExpr &>(e);

		return unary.op == UnaryOpKind::Dereference;
	}

	bool TypeCheckingPass::canArgsCallFunc(const Vec<Ref<const Type>> &args,
										   const FunctionType &func) const {
		auto &params = func.paramTypes;

		if (args.size() != params.size()) {
			std::stringstream ss;
			ss << "Provided wrong amount of args. Expected ";
			ss << params.size() << " arguments but got " << args.size();

			m_Context.addError(ss.str());

			return false;
		}

		for (size_t i = 0; i < args.size(); ++i) {
			auto &argType = args[i].get();
			auto &paramType = *params[i];

			if (argType.kind == TypeKind::Error || argType == paramType)
				continue;

			std::stringstream ss;
			ss << "Cannot use type " << argType << " as parameter ";
			ss << " for an argument of type " << paramType;

			m_Context.addError(ss.str());

			return false;
		}

		return true;
	}
}