#include "TypeCheckingPass.h"

#include "core/Macros.h"
#include "semantic/common/ErrorMessages.h"
#include "type/TypeFactory.h"

namespace sem {
using namespace ast;
using enum ErrorMessageKind;

TypeCheckingPass::TypeCheckingPass(TypeCheckerContext &ctx)
	: m_Context(ctx) {}

bool TypeCheckingPass::visit(IntLit &n) {
	VERIFY(!n.isInferred());
	n.infer(TypeFactory::getI32(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(CharLit &n) {
	VERIFY(!n.isInferred());
	n.infer(TypeFactory::getChar(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(BoolLit &n) {
	VERIFY(!n.isInferred());
	n.infer(TypeFactory::getBool(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(NullLit &n) {
	VERIFY(!n.isInferred());
	n.infer(TypeFactory::getNull(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(UnitLit &n) {
	VERIFY(!n.isInferred());
	n.infer(TypeFactory::getUnit(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(HeapAlloc &n) {
	VERIFY(!n.isInferred());
	const auto actualType = checkExpression(*n.expr);
	const auto expectedType = n.type;

	if (!typesMatch(actualType, expectedType)) {
		const auto msg = ErrorMessage<TypeMissmatch>::str(expectedType, actualType);
		m_Context.submitError(msg, n.loc);
	}

	n.infer(TypeFactory::getPointer(expectedType), ValueCategory::RValue);
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
			m_Context.submitError(msg, n.loc);

			n.infer(TypeFactory::getError(), ValueCategory::RValue);
			return false;
		}

		const auto ptrType = dynamic_cast<const PointerType *>(type);
		n.infer(ptrType->pointeeType, ValueCategory::LValue);
		return false;
	}

	auto &operatorTable = m_Context.getOperatorTable();

	if (const auto opFunc = operatorTable.getUnaryOperator(n.op, type)) {
		const auto returnType = opFunc.value()->returnType;

		n.infer(returnType, ValueCategory::RValue);
		return false;
	}

	const auto msg = ErrorMessage<UnaryOperatorNotFound>::str(type, n.op);
	m_Context.submitError(msg, n.loc);

	n.infer(TypeFactory::getError(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(BinaryExpr &n) {
	const auto left = checkExpression(*n.left);
	const auto right = checkExpression(*n.right);

	if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error)) {
		n.infer(TypeFactory::getError(), ValueCategory::RValue);
		return false;
	}

	auto &operatorTable = m_Context.getOperatorTable();

	if (const auto opFunc = operatorTable.getBinaryOperator(n.op, left, right)) {
		const auto returnType = opFunc.value()->returnType;

		n.infer(returnType, ValueCategory::RValue);
		return false;
	}

	const auto msg = ErrorMessage<BinaryOperatorNotFound>::str(left, right, n.op);
	m_Context.submitError(msg, n.loc);

	n.infer(TypeFactory::getError(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(Assignment &n) {
	// For now assignments don't return anything, so i = j = 5 won't work.
	n.infer(TypeFactory::getUnit(), ValueCategory::RValue);

	const auto left = checkExpression(*n.left);
	const auto right = checkExpression(*n.right);

	if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error))
		return false;

	if (n.left->valueCategory != ValueCategory::LValue) {
		const auto msg = ErrorMessage<AssignToRValue>::str();
		m_Context.submitError(msg, n.loc);

		return false;
	}

	// Some assignments are compound assignments like '+='
	const auto compoundOp = getBinaryOpFromAssignment(n.assignmentKind);

	// Normal assignment '='
	if (!compoundOp.has_value()) {
		if (!typesMatch(left, right)) {
			const auto msg = ErrorMessage<TypeMissmatch>::str(left, right);
			m_Context.submitError(msg, n.loc);
		}

		return false;
	}

	auto &operatorTable = m_Context.getOperatorTable();
	const auto opFunc = operatorTable.getBinaryOperator(compoundOp.value(), left, right);

	if (!opFunc.has_value()) {
		const auto msg = ErrorMessage<BinaryOperatorNotFound>::str(left, right, compoundOp.value());
		m_Context.submitError(msg, n.loc);

		return false;
	}

	const auto resultType = opFunc.value()->returnType;

	// The expression is only legal if the result type is equal to the left type of the
	// assignment. If we have 'a += b' this will be expanded to 'a = a + b', then the type
	// of 'a' has to be equal to the type of 'a + b' to make the assignment legal.
	if (!typesMatch(left, resultType)) {
		const auto msg = ErrorMessage<TypeMissmatch>::str(left, resultType);
		m_Context.submitError(msg, n.loc);
	}

	return false;
}

bool TypeCheckingPass::visit(FuncCall &n) {
	if (auto *varRef = dynamic_cast<VarRef *>(n.expr.get())) {
		auto *structType = TypeFactory::getStruct(varRef->ident);
		const auto function = m_Context.getGlobalNamespace().getFunction(varRef->ident);

		if (n.isStructConstructor && structType->isDeclared) {
			// Treat `StructName{...}` as a struct constructor expression.
			varRef->infer(structType, ValueCategory::RValue);

			TypeList argTypes;
			argTypes.reserve(n.args.size());

			for (auto &expr : n.args)
				argTypes.push_back(checkExpression(*expr));

			if (argTypes.size() != structType->orderedFields.size()) {
				const auto msg =
						ErrorMessage<TooManyArguments>::str(structType->orderedFields.size(),
															argTypes.size());
				m_Context.submitError(msg, n.loc);

				n.infer(TypeFactory::getError(), ValueCategory::RValue);
				return false;
			}

			size_t index = 0;
			for (const auto &[fieldName, fieldType] : structType->orderedFields) {
				const auto argType = argTypes[index++];

				if (!argType->isTypeKind(TypeKind::Error) && !typesMatch(argType, fieldType)) {
					const auto msg = std::format(
							"Struct field '{}' expected type '{}', got '{}' at argument {}.",
							fieldName, *fieldType, *argType, index);
					m_Context.submitError(msg, n.args[index - 1]->loc);
				}
			}

			n.infer(structType, ValueCategory::RValue);
			return false;
		}

		if (structType->isDeclared && !function.has_value()) {
			const auto msg = std::format("Struct '{}' must be constructed using '{{}}', not '()'.",
										 structType->name);
			m_Context.submitError(msg, n.loc);
			n.infer(TypeFactory::getError(), ValueCategory::RValue);
			return false;
		}
	}

	const auto type = checkExpression(*n.expr);

	if (type->isTypeKind(TypeKind::Error)) {
		n.infer(TypeFactory::getError(), ValueCategory::RValue);
		return false;
	}

	if (!type->isTypeKind(TypeKind::Function)) {
		const auto msg = ErrorMessage<CallOnNonFunctionType>::str(type);
		m_Context.submitError(msg, n.loc);
		n.infer(TypeFactory::getError(), ValueCategory::RValue);

		return false;
	}

	const auto funcType = dynamic_cast<const FunctionType *>(type);

	TypeList argTypes;
	argTypes.reserve(n.args.size());

	for (auto &expr : n.args) {
		auto exprType = checkExpression(*expr);
		argTypes.push_back(exprType);
	}

	checkIfArgsCanCallFunction(argTypes, funcType, n.loc);

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
	m_Context.submitError(msg, n.loc);

	n.infer(TypeFactory::getError(), ValueCategory::RValue);
	return false;
}

bool TypeCheckingPass::visit(FieldAccess &n) {
	const auto baseType = checkExpression(*n.base);

	if (baseType->isTypeKind(TypeKind::Error)) {
		n.infer(TypeFactory::getError(), ValueCategory::RValue);
		return false;
	}

	if (!baseType->isTypeKind(TypeKind::Struct)) {
		const auto msg = std::format("Cannot access field '{}' on non-struct type '{}'.", n.field,
									 *baseType);
		m_Context.submitError(msg, n.loc);
		n.infer(TypeFactory::getError(), ValueCategory::RValue);
		return false;
	}

	auto *structType = static_cast<StructType *>(baseType);
	auto it = structType->fields.find(n.field);

	if (it == structType->fields.end()) {
		const auto msg =
				std::format("Struct '{}' has no field named '{}'.", structType->name, n.field);
		m_Context.submitError(msg, n.loc);
		n.infer(TypeFactory::getError(), ValueCategory::RValue);
		return false;
	}

	n.infer(it->second, ValueCategory::LValue);
	return false;
}

bool TypeCheckingPass::visit(BlockStmt &n) {
	m_SymbolTable.enterScope();

	bool foundReturn = false, foundUnreachable = false;

	for (const auto &stmt : n.stmts) {
		const bool didReturn = dispatch(*stmt);

		// Any statement that appears after a returning path is unreachable.
		if (foundReturn && !foundUnreachable) {
			foundUnreachable = true;
			const auto msg = ErrorMessage<UnreachableStatement>::str();
			m_Context.submitError(msg, stmt->loc, ErrorLevel::WARNING);
		}

		foundReturn |= didReturn;
	}

	m_SymbolTable.exitScope();
	return foundReturn;
}

bool TypeCheckingPass::visit(IfStmt &n) {
	const auto type = checkExpression(*n.cond);
	const auto boolType = TypeFactory::getBool();

	if (!(type->isTypeKind(TypeKind::Error) || typesMatch(type, boolType))) {
		const auto msg = ErrorMessage<TypeMissmatch>::str(boolType, type);
		m_Context.submitError(msg, n.cond->loc);
	}

	const bool thenReturns = dispatch(*n.then);
	const bool elseReturns = dispatch(*n.else_);

	return thenReturns && elseReturns;
}

bool TypeCheckingPass::visit(WhileStmt &n) {
	const auto type = checkExpression(*n.cond);
	const auto boolType = TypeFactory::getBool();

	if (!(type->isTypeKind(TypeKind::Error) || typesMatch(type, boolType))) {
		const auto msg = ErrorMessage<TypeMissmatch>::str(boolType, type);
		m_Context.submitError(msg, n.cond->loc);
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
	m_Context.submitError(msg, n.loc);

	return true;
}

bool TypeCheckingPass::visit(VarDef &n) {
	const auto type = checkExpression(*n.value);
	auto varType = n.type;

	// The expression is not of type <error-type> but does not match the
	// type of the variable declaration - this is an actual error
	if (!type->isTypeKind(TypeKind::Error) && !typesMatch(type, varType)) {
		const auto msg = ErrorMessage<TypeMissmatch>::str(varType, type);
		m_Context.submitError(msg, n.loc);
	}

	// Does this symbol already exist in the current scope (shadowing outer scope possible)
	if (m_SymbolTable.isSymbolDefinedInCurrentScope(n.ident)) {
		const auto msg = ErrorMessage<SymbolRedefinition>::str(n.ident);
		m_Context.submitError(msg, n.loc);

		return false;
	}

	m_SymbolTable.addSymbol(n.ident, varType);

	return false;
}

bool TypeCheckingPass::visit(FuncDecl &n) {
	m_SymbolTable.enterScope();

	for (auto &[name, type] : n.params) {
		m_SymbolTable.addSymbol(name, type);
	}

	// Set the current expected return type.
	m_CurrentFunctionReturnType = n.returnType;

	// Type check the function body (in a nested scope, allows param shadowing).
	const bool doesReturn = dispatch(*n.body);

	if (!doesReturn && !n.returnType->isTypeKind(TypeKind::Unit)) {
		const auto msg = ErrorMessage<NonReturningPaths>::str(n.ident);
		m_Context.submitError(msg, n.loc);
	}

	m_CurrentFunctionReturnType = std::nullopt;
	m_SymbolTable.exitScope();

	// We already explored this function during the exploration pass, don't
	// add it to the symbol table a second time, that would break it.
	return false;
}

bool TypeCheckingPass::visit(Module &n) {
	for (auto &d : n.funcs)
		dispatch(*d);

	const auto mainType = m_Context.getGlobalNamespace().getFunction(u8"main");

	if (!mainType.has_value()) {
		m_Context.submitError(u8"Missing entry point, expected 'func main() -> i32'.", n.loc);
		return false;
	}

	const auto *fn = mainType.value();
	const bool isValidMain = fn->paramTypes.empty() && fn->returnType == TypeFactory::getI32();

	if (isValidMain) {
		return false;
	}

	SourceLoc mainLoc = n.loc;
	for (const auto &decl : n.funcs) {
		if (decl->ident == u8"main") {
			mainLoc = decl->loc;
			break;
		}
	}

	m_Context.submitError(u8"Invalid entry point, expected 'func main() -> i32'.", mainLoc);

	return false;
}

Type TypeCheckingPass::checkExpression(Expr &n) {
	VERIFY(!n.inferredType.has_value());
	dispatch(n);
	VERIFY(n.inferredType.has_value());

	return n.inferredType.value();
}

bool TypeCheckingPass::typesMatch(Type left, Type right) {
	if (left->isTypeKind(TypeKind::Error) || right->isTypeKind(TypeKind::Error))
		return false;

	if (left->isTypeKind(TypeKind::Null) && right->isTypeKind(TypeKind::Pointer))
		return true;

	if (right->isTypeKind(TypeKind::Null) && left->isTypeKind(TypeKind::Pointer))
		return true;

	return left == right;
}

void TypeCheckingPass::checkIfArgsCanCallFunction(const TypeList &args, const FunctionType *func,
												  const SourceLoc &callLoc) const {
	const auto &params = func->paramTypes;

	if (args.size() != params.size()) {
		const auto msg = ErrorMessage<TooManyArguments>::str(params.size(), args.size());
		m_Context.submitError(msg, callLoc);

		return;
	}

	for (size_t i = 0; i < args.size(); ++i) {
		auto &argType = args[i];
		auto paramType = params[i];

		if (typesMatch(argType, paramType))
			continue;

		const auto msg = ErrorMessage<TypeMissmatch>::str(paramType, argType);
		m_Context.submitError(msg, callLoc);

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