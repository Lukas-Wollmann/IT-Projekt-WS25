#include "TypeCheckingPass.h"

#include <sstream>

#include "Macros.h"
#include "semantic/common/ErrorMessages.h"
#include "type/CompareVisitor.h"
#include "type/PrintVisitor.h"

namespace semantic {
	using namespace type;
	using namespace ast;

	TypeCheckingPass::TypeCheckingPass(TypeCheckerContext &context)
		: m_Context(context) {}

	bool TypeCheckingPass::visit(IntLit &n) {
        VERIFY(!n.isInferred());
		n.infer(std::make_shared<Typename>(u8"i32"), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(FloatLit &n) {
        VERIFY(!n.isInferred());
		n.infer(std::make_shared<Typename>(u8"f32"), ValueCategory::RValue);
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

	bool TypeCheckingPass::visit(StringLit &n) {
        VERIFY(!n.isInferred());
        n.infer(std::make_shared<Typename>(u8"string"), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(UnitLit &n) {
        VERIFY(!n.isInferred());
        n.infer(std::make_shared<UnitType>(), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(ArrayExpr &n) {
		VERIFY(!n.isInferred());
		VERIFY(!n.elementType->isTypeKind(TypeKind::Error));

		for (auto &expr : n.values) {
			auto type = checkExpression(*expr);

			if (typesMatch(type, n.elementType))
				continue;

			ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
			m_Context.addError(err.str(n.elementType, type));
		}

        auto arrType = std::make_shared<ArrayType>(n.elementType, n.values.size());
        n.infer(arrType, ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(UnaryExpr &n) {
		VERIFY(!n.isInferred());
		auto type = checkExpression(*n.operand);

		if (type->isTypeKind(TypeKind::Error)) {
            n.infer(type, ValueCategory::RValue);
			return false;
        }

		if (n.op == UnaryOpKind::Dereference) {
			if (!type->isTypeKind(TypeKind::Pointer)) {
				ErrorMessage<ErrorMessageKind::DEREFERENCE_NON_POINTER_TYPE> err;
				m_Context.addError(err.str(type));

                n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
				return false;
			}

			auto ptrType = std::static_pointer_cast<const PointerType>(type);
			
            n.infer(ptrType->pointeeType, ValueCategory::LValue);
            return false;
		}

        auto &operatorTable = m_Context.getOperatorTable();
		auto opFunc = operatorTable.getUnaryOperator(n.op, type);

		if (opFunc.has_value()) {
            auto returnType = opFunc.value().returnType;

            n.infer(returnType, ValueCategory::RValue);
			return false;
		}

		ErrorMessage<ErrorMessageKind::UNARY_OPERATOR_NOT_FOUND> err;
		m_Context.addError(err.str(type, n.op));
		
        n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
        return false;
	}

	bool TypeCheckingPass::visit(BinaryExpr &n) {
		auto left = checkExpression(*n.left);
		auto right = checkExpression(*n.right);

		if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error)) {
            n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
			return false;
        }

            
        auto &operatorTable = m_Context.getOperatorTable();
		auto opFunc = operatorTable.getBinaryOperator(n.op, left, right);

		if (opFunc.has_value()) {
            auto returnType = opFunc.value().returnType;
            
            n.infer(returnType, ValueCategory::RValue);
			return false;
		}

		ErrorMessage<ErrorMessageKind::BINARY_OPERATOR_NOT_FOUND> err;
		m_Context.addError(err.str(left, right, n.op));

        n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(HeapAlloc &n) {
		VERIFY(!n.type->isTypeKind(TypeKind::Error));
        n.infer(std::make_shared<PointerType>(n.type), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(Assignment &n) {
		// For now assignments dont return anything, so i = j = 5 won't work.
		n.infer(std::make_shared<UnitType>(), ValueCategory::RValue);
		
        auto left = checkExpression(*n.left);
		auto right = checkExpression(*n.right);

		if (n.left->valueCategory != ValueCategory::LValue) {
			ErrorMessage<ErrorMessageKind::ASSIGN_TO_RVALUE> err;
			m_Context.addError(err.str());
            return false;
		}

		if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error))
			return false;

		// Some assignments are compound assignments like '+='
		auto compoundOp = getBinaryOpFromAssignment(n.assignmentKind);

		// Normal assignment '='
		if (!compoundOp.has_value()) {
			if (typesMatch(left, right))
				return false;

			ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
			m_Context.addError(err.str(left, right));
			return false;
		}

        auto &operatorTable = m_Context.getOperatorTable();
		auto opFunc = operatorTable.getBinaryOperator(compoundOp.value(), left, right);

		if (!opFunc.has_value()) {
			ErrorMessage<ErrorMessageKind::BINARY_OPERATOR_NOT_FOUND> err;
			m_Context.addError(err.str(left, right, compoundOp.value()));

			n.inferredType = std::make_shared<ErrorType>();	
            return false;
		}

		auto resultType = opFunc.value().returnType;

        // The expression is only legal if the result type is equal to the left type of the 
        // assignment. If we have 'a += b' this will be expanded to 'a = a + b', then the type
        // of 'a' has to be equal to the type of 'a + b' to make the assignment legal.
		if (*left == *resultType)
			return false;

		ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
		m_Context.addError(err.str(left, resultType));
		return false;
	}

	bool TypeCheckingPass::visit(FuncCall &n) {
        auto type = checkExpression(*n.expr);

        if (type->isTypeKind(TypeKind::Error)) {
            n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
            return false;
        }

        if (!type->isTypeKind(TypeKind::Function)) {
            ErrorMessage<ErrorMessageKind::CALL_ON_NON_FUNCTION> err;
			m_Context.addError(err.str(type));
            return false;
        }

		auto funcType = std::static_pointer_cast<const FunctionType>(type);
        
        TypeList argTypes;
        argTypes.reserve(n.args.size());

		for (auto &expr : n.args) {
            auto exprType = checkExpression(*expr);
            argTypes.push_back(exprType);
		}

		canArgsCallFunc(argTypes, funcType);
        n.infer(funcType->returnType, ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(VarRef &n) {
		auto symbol = m_SymbolTable.getSymbol(n.ident);

		if (symbol.has_value()) {
            n.infer(symbol.value().get().getType(), ValueCategory::LValue);
			return false;
		}

		auto func = m_Context.getGlobalNamespace().getFunction(n.ident);

		if (func.has_value()) {
            n.infer(func.value(), ValueCategory::RValue);
			return false;
		}

		ErrorMessage<ErrorMessageKind::UNDEFINED_REFERENCE> err;
		m_Context.addError(err.str(n.ident));

        n.infer(std::make_shared<ErrorType>(), ValueCategory::RValue);
		return false;
	}

	bool TypeCheckingPass::visit(BlockStmt &n) {
		m_SymbolTable.enterScope();

		bool foundReturn = false;

		for (size_t i = 0; i < n.stmts.size(); ++i) {
			bool didReturn = dispatch(*n.stmts[i]);

			if (foundReturn && didReturn) {
				ErrorMessage<ErrorMessageKind::UNREACHABLE_STATEMENT> err;
				m_Context.addError(err.str());
			}

			foundReturn |= didReturn;
		}

		m_SymbolTable.exitScope();
		return foundReturn;
	}

	bool TypeCheckingPass::visit(IfStmt &n) {
        auto type = checkExpression(*n.cond);
        auto boolType = std::make_shared<Typename>(u8"bool");

        if (!typesMatch(type, boolType)) {
            ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
            m_Context.addError(err.str(boolType, type));
        }

		bool thenReturns = dispatch(*n.then);
		bool elseReturns = dispatch(*n.else_);

		return thenReturns && elseReturns;
	}

	bool TypeCheckingPass::visit(WhileStmt &n) {
        auto type = checkExpression(*n.cond);
        auto boolType = std::make_shared<Typename>(u8"bool");

        if (!typesMatch(type, boolType)) {
            ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
            m_Context.addError(err.str(boolType, type));
        }

		dispatch(*n.body);
		return false;
	}

	bool TypeCheckingPass::visit(ReturnStmt &n) {
		VERIFY(m_CurrentFunctionReturnType.has_value());
        auto currentFuncRetType = m_CurrentFunctionReturnType.value();

		// The function has a Unit return type and we returned with no value.
		if (currentFuncRetType->isTypeKind(TypeKind::Unit) && !n.expr.has_value())
			return true;

		auto &expr = *n.expr.value();
        auto type = checkExpression(expr);

		// If the type is <error-type> or if the return type matches
		// the function declaration, its okay and a valid return.
		if (type->isTypeKind(TypeKind::Error) || typesMatch(type, currentFuncRetType))
			return true;

		// The return type is not matching the function declaration
		ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
		m_Context.addError(err.str(currentFuncRetType, type));
		return true;
	}

	bool TypeCheckingPass::visit(VarDef &n) {
        auto type = checkExpression(*n.value);
        auto varType = n.type;

		// The expression is not of type <error-type> but does not match type
		// type of the variable declaration - this is an actual error
		if (!typesMatch(type, varType)) {
			ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
			m_Context.addError(err.str(varType, type));
		}

		// Does this symbol already exist in the current scope (shadowing outer scope possible)
		if (m_SymbolTable.isSymbolDefinedInCurrentScope(n.ident)) {
			ErrorMessage<ErrorMessageKind::VARIABLE_REDEFINITION> err;
			m_Context.addError(err.str(n.ident));
			return false;
		}

		m_SymbolTable.addSymbol(n.ident, SymbolInfo(varType));
		return false;
	}

	bool TypeCheckingPass::visit(FuncDecl &n) {
		m_SymbolTable.enterScope();

		for (auto &param : n.params)
			m_SymbolTable.addSymbol(param.first, SymbolInfo(param.second));

		// Set the current expected return type
		m_CurrentFunctionReturnType = n.returnType;

		// Type check the function body (in a nested scope, allows param shadowing)
		bool doesReturn = dispatch(*n.body);

		if (!doesReturn && !n.returnType->isTypeKind(TypeKind::Unit)) {
			ErrorMessage<ErrorMessageKind::NON_RETURNING_PATHS> err;
			m_Context.addError(err.str(n.ident));
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

    TypePtr TypeCheckingPass::checkExpression(Expr &n) {
		VERIFY(!n.inferredType.has_value());
		dispatch(n);
		VERIFY(n.inferredType.has_value());

		return n.inferredType.value();
	}

	bool TypeCheckingPass::typesMatch(TypePtr left, TypePtr right) const {
		if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error))
			return false;

		return *left == *right;
	}

	bool TypeCheckingPass::canArgsCallFunc(const TypeList &args, FunctionTypePtr func) const {
		auto params = func->paramTypes;

		if (args.size() != params.size()) {
			ErrorMessage<ErrorMessageKind::TOO_MANY_ARGUMENTS> err;
			m_Context.addError(err.str(params.size(), args.size()));
			return false;
		}

		for (size_t i = 0; i < args.size(); ++i) {
			auto argType = args[i];
			auto paramType = params[i];

            if (typesMatch(argType, paramType))
                continue; 

			ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> err;
			m_Context.addError(err.str(paramType, argType));
			return false;
		}

		return true;
	}

	Opt<BinaryOpKind> TypeCheckingPass::getBinaryOpFromAssignment(AssignmentKind kind) const {
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