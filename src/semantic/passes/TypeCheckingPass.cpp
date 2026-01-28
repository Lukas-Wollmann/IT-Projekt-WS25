#include "TypeCheckingPass.h"

#include "core/Macros.h"
#include "semantic/common/ErrorMessages.h"
#include "type/Compare.h"

namespace semantic {
	using namespace type;
	using namespace ast;
	using enum ErrorMessageKind;

	TypeCheckingPass::TypeCheckingPass(TypeCheckerContext &ctx)
		: m_Context(ctx) {}

	bool TypeCheckingPass::visit(IntLit &n) {
		VERIFY(!n.isInferred());
		n.infer(std::make_shared<Typename>(u8"i32"), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(CharLit &n) {
		VERIFY(!n.isInferred());
		n.infer(std::make_shared<Typename>(u8"char"), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(BoolLit &n) {
		VERIFY(!n.isInferred());
		n.infer(std::make_shared<Typename>(u8"bool"), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(UnitLit &n) {
		VERIFY(!n.isInferred());
		n.infer(std::make_shared<UnitType>(), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(UnaryExpr &n) {
		VERIFY(!n.isInferred());
		const auto type = checkExpression(*n.operand);

		if (type->isTypeKind(TypeKind::Error)) {
			n.infer(type, ValueCategory::RValue);
			return false;
		}

		if (n.op == UnaryOpKind::Dereference) {
			if (!type->isTypeKind(TypeKind::Pointer)) {
				const auto msg = ErrorMessage<DereferenceNonPointerType>::str(type);
				m_Context.submitError(msg, {});

				n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
				return false;
			}

			const auto ptrType = std::static_pointer_cast<const PointerType>(type);

			n.infer(ptrType->pointeeType, ValueCategory::LValue);
			return false;
		}

		auto &operatorTable = m_Context.getOperatorTable();

		if (const auto opFunc = operatorTable.getUnaryOperator(n.op, type)) {
			const auto returnType = opFunc.value().returnType;

			n.infer(returnType, ValueCategory::RValue);
			return false;
		}

		const auto msg = ErrorMessage<UnaryOperatorNotFound>::str(type, n.op);
		m_Context.submitError(msg, {});

		n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(BinaryExpr &n) {
		const auto left = checkExpression(*n.left);
		const auto right = checkExpression(*n.right);

		if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error)) {
			n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
			return false;
		}

		auto &operatorTable = m_Context.getOperatorTable();

		if (const auto opFunc = operatorTable.getBinaryOperator(n.op, left, right)) {
			const auto returnType = opFunc.value().returnType;

			n.infer(returnType, ValueCategory::RValue);
			return false;
		}

		const auto msg = ErrorMessage<BinaryOperatorNotFound>::str(left, right, n.op);
		m_Context.submitError(msg, {});

		n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(Assignment &n) {
		// For now assignments don't return anything, so i = j = 5 won't work.
		n.infer(std::make_shared<UnitType>(), ValueCategory::RValue);

		const auto left = checkExpression(*n.left);
		const auto right = checkExpression(*n.right);

		if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error))
			return false;

		if (n.left->valueCategory != ValueCategory::LValue) {
			const auto msg = ErrorMessage<AssignToRValue>::str();
			m_Context.submitError(msg, {});

			return false;
		}

		// Some assignments are compound assignments like '+='
		const auto compoundOp = getBinaryOpFromAssignment(n.assignmentKind);

		// Normal assignment '='
		if (!compoundOp.has_value()) {
			if (typesMatch(left, right))
				return false;

			const auto msg = ErrorMessage<TypeMissmatch>::str(left, right);
			m_Context.submitError(msg, {});

			return false;
		}

		auto &operatorTable = m_Context.getOperatorTable();
		const auto opFunc = operatorTable.getBinaryOperator(compoundOp.value(), left, right);

		if (!opFunc.has_value()) {
			const auto msg =
					ErrorMessage<BinaryOperatorNotFound>::str(left, right, compoundOp.value());
			m_Context.submitError(msg, {});

			return false;
		}

		const auto resultType = opFunc.value().returnType;

		// The expression is only legal if the result type is equal to the left type of the
		// assignment. If we have 'a += b' this will be expanded to 'a = a + b', then the type
		// of 'a' has to be equal to the type of 'a + b' to make the assignment legal.
		if (*left == *resultType)
			return false;

		const auto msg = ErrorMessage<TypeMissmatch>::str(left, resultType);
		m_Context.submitError(msg, {});

		return false;
	}

	bool TypeCheckingPass::visit(FuncCall &n) {
		const auto type = checkExpression(*n.expr);

		if (type->isTypeKind(TypeKind::Error)) {
			n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
			return false;
		}

		if (!type->isTypeKind(TypeKind::Function)) {
			const auto msg = ErrorMessage<CallOnNonFunctionType>::str(type);
			m_Context.submitError(msg, {});

			return false;
		}

		const auto funcType = std::static_pointer_cast<const FunctionType>(type);

		TypeList argTypes;
		argTypes.reserve(n.args.size());

		for (auto &expr : n.args) {
			auto exprType = checkExpression(*expr);
			argTypes.push_back(exprType);
		}

		checkIfArgsCanCallFunction(argTypes, funcType);

		n.infer(funcType->returnType, ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(VarRef &n) {
		if (const auto symbol = m_SymbolTable.getSymbol(n.ident)) {
			n.infer(symbol.value(), ValueCategory::LValue);
			return false;
		}

		if (const auto func = m_Context.getGlobalNamespace().getFunction(n.ident)) {
			n.infer(func.value(), ValueCategory::RValue);
			return false;
		}

		const auto msg = ErrorMessage<UndefinedReference>::str(n.ident);
		m_Context.submitError(msg, {});

		n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(BlockStmt &n) {
		m_SymbolTable.enterScope();

		bool foundReturn = false, foundUnreachable = false;

		for (const auto &stmt : n.stmts) {
			const bool didReturn = dispatch(*stmt);

			if (foundReturn && didReturn && !foundUnreachable) {
				foundUnreachable = true;
				const auto msg = ErrorMessage<ErrorMessageKind::UnreachableStatement>::str();
				m_Context.submitError(msg, {}, ErrorLevel::WARNING);
			}

			foundReturn |= didReturn;
		}

		m_SymbolTable.exitScope();
		return foundReturn;
	}

	bool TypeCheckingPass::visit(IfStmt &n) {
		const auto type = checkExpression(*n.cond);
		const auto boolType = std::make_shared<Typename>(u8"bool");

		if (!typesMatch(type, boolType)) {
			const auto msg = ErrorMessage<TypeMissmatch>::str(boolType, type);
			m_Context.submitError(msg, {});
		}

		const bool thenReturns = dispatch(*n.then);
		const bool elseReturns = dispatch(*n.else_);

		return thenReturns && elseReturns;
	}

	bool TypeCheckingPass::visit(WhileStmt &n) {
		const auto type = checkExpression(*n.cond);
		const auto boolType = std::make_shared<Typename>(u8"bool");

		if (!typesMatch(type, boolType)) {
			const auto msg = ErrorMessage<TypeMissmatch>::str(boolType, type);
			m_Context.submitError(msg, {});
		}

		dispatch(*n.body);
		return false;
	}

	bool TypeCheckingPass::visit(ReturnStmt &n) {
		VERIFY(m_CurrentFunctionReturnType.has_value());
		const auto currentFuncRetType = m_CurrentFunctionReturnType.value();
		const auto type = checkExpression(*n.expr);

		// If the type is <error-type> or if the return type matches
		// the function declaration, it's okay and a valid return.
		if (type->isTypeKind(TypeKind::Error) || typesMatch(type, currentFuncRetType))
			return true;

		// The return type is not matching the function declaration
		const auto msg = ErrorMessage<TypeMissmatch>::str(currentFuncRetType, type);
		m_Context.submitError(msg, {});

		return true;
	}

	bool TypeCheckingPass::visit(VarDef &n) {
		const auto type = checkExpression(*n.value);
		auto varType = n.type;

		// The expression is not of type <error-type> but does not match the
		// type of the variable declaration - this is an actual error
		if (!type->isTypeKind(TypeKind::Error) && !typesMatch(type, varType)) {
			const auto msg = ErrorMessage<ErrorMessageKind::TypeMissmatch>::str(varType, type);
			m_Context.submitError(msg, {});
		}

		// Does this symbol already exist in the current scope (shadowing outer scope possible)
		if (m_SymbolTable.isSymbolDefinedInCurrentScope(n.ident)) {
			const auto msg = ErrorMessage<ErrorMessageKind::SymbolRedefinition>::str(n.ident);
			m_Context.submitError(msg, {});

			return false;
		}

		m_SymbolTable.addSymbol(n.ident, varType);

		return false;
	}

	bool TypeCheckingPass::visit(FuncDecl &n) {
		m_SymbolTable.enterScope();

		for (auto &[name, type] : n.params)
			m_SymbolTable.addSymbol(name, type);

		// Set the current expected return type
		m_CurrentFunctionReturnType = n.returnType;

		// Type check the function body (in a nested scope, allows param shadowing)
		const bool doesReturn = dispatch(*n.body);

		if (!doesReturn && !n.returnType->isTypeKind(TypeKind::Unit)) {
			const auto msg = ErrorMessage<ErrorMessageKind::NonReturningPaths>::str(n.ident);
			m_Context.submitError(msg, {});
		}

		m_SymbolTable.exitScope();

		// We already explored this function during the exploration pass, don't
		// add it to the symbol table a second time, that would break it.
		return false;
	}

	bool TypeCheckingPass::visit(Module &n) {
		for (auto &d : n.decls)
			dispatch(*d);

		return false;
	}

	TypePtr TypeCheckingPass::checkExpression(Expr &n) {
		VERIFY(!n.inferredType.has_value());
		dispatch(n);
		VERIFY(n.inferredType.has_value());

		return n.inferredType.value();
	}

	bool TypeCheckingPass::typesMatch(const TypePtr &left, const TypePtr &right) {
		if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error))
			return false;

		return *left == *right;
	}

	void TypeCheckingPass::checkIfArgsCanCallFunction(const TypeList &args,
													  const FunctionTypePtr &func) const {
		const auto &params = func->paramTypes;

		if (args.size() != params.size()) {
			const auto msg = ErrorMessage<TooManyArguments>::str(params.size(), args.size());
			m_Context.submitError(msg, {});

			return;
		}

		for (size_t i = 0; i < args.size(); ++i) {
			auto &argType = args[i];
			auto paramType = params[i];

			if (typesMatch(argType, paramType))
				continue;

			const auto msg = ErrorMessage<TypeMissmatch>::str(paramType, argType);
			m_Context.submitError(msg, {});

			return;
		}
	}

	Opt<BinaryOpKind> TypeCheckingPass::getBinaryOpFromAssignment(const AssignmentKind kind) {
		using enum AssignmentKind;
		switch (kind) {
			case Simple:		 return std::nullopt;
			case Addition:		 return BinaryOpKind::Addition;
			case Subtraction:	 return BinaryOpKind::Subtraction;
			case Multiplication: return BinaryOpKind::Multiplication;
			case Division:		 return BinaryOpKind::Division;
			case Modulo:		 return BinaryOpKind::Modulo;
			case BitwiseAnd:	 return BinaryOpKind::BitwiseAnd;
			case BitwiseOr:		 return BinaryOpKind::BitwiseOr;
			case BitwiseXor:	 return BinaryOpKind::BitwiseXor;
			case LeftShift:		 return BinaryOpKind::LeftShift;
			case RightShift:	 return BinaryOpKind::RightShift;
			default:			 UNREACHABLE();
		}
	}
}