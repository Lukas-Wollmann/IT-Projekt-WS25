#include "Doctest.h"
#include "ast/AST.h"
#include "type/Type.h"
#include "type/TypeFactory.h"
#include "core/U8String.h"
#include "core/SourceLoc.h"

using namespace ast;

// ==================== IntLit Tests ====================

TEST_CASE("AST: IntLit construction with positive value") {
	// Arrange
	i32 value = 42;

	// Act
	IntLit intLit(value);

	// Assert
	CHECK(intLit.value == 42);
	CHECK(intLit.kind == NodeKind::IntLit);
}

TEST_CASE("AST: IntLit construction with negative value") {
	// Arrange
	i32 value = -100;

	// Act
	IntLit intLit(value);

	// Assert
	CHECK(intLit.value == -100);
	CHECK(intLit.kind == NodeKind::IntLit);
}

TEST_CASE("AST: IntLit construction with zero") {
	// Arrange
	i32 value = 0;

	// Act
	IntLit intLit(value);

	// Assert
	CHECK(intLit.value == 0);
	CHECK(intLit.kind == NodeKind::IntLit);
}

TEST_CASE("AST: IntLit is not inferred by default") {
	// Arrange
	IntLit intLit(42);

	// Act
	bool isInferred = intLit.isInferred();

	// Assert
	CHECK(!isInferred);
}

// ==================== CharLit Tests ====================

TEST_CASE("AST: CharLit construction with ASCII character") {
	// Arrange
	char32_t value = U'a';

	// Act
	CharLit charLit(value);

	// Assert
	CHECK(charLit.value == U'a');
	CHECK(charLit.kind == NodeKind::CharLit);
}

TEST_CASE("AST: CharLit construction with special character") {
	// Arrange
	char32_t value = U'\n';

	// Act
	CharLit charLit(value);

	// Assert
	CHECK(charLit.value == U'\n');
	CHECK(charLit.kind == NodeKind::CharLit);
}

TEST_CASE("AST: CharLit construction with unicode character") {
	// Arrange
	char32_t value = U'🎀';

	// Act
	CharLit charLit(value);

	// Assert
	CHECK(charLit.value == U'🎀');
	CHECK(charLit.kind == NodeKind::CharLit);
}

TEST_CASE("AST: CharLit is not inferred by default") {
	// Arrange
	CharLit charLit(U'x');

	// Act
	bool isInferred = charLit.isInferred();

	// Assert
	CHECK(!isInferred);
}

// ==================== BoolLit Tests ====================

TEST_CASE("AST: BoolLit construction with true value") {
	// Arrange
	bool value = true;

	// Act
	BoolLit boolLit(value);

	// Assert
	CHECK(boolLit.value == true);
	CHECK(boolLit.kind == NodeKind::BoolLit);
}

TEST_CASE("AST: BoolLit construction with false value") {
	// Arrange
	bool value = false;

	// Act
	BoolLit boolLit(value);

	// Assert
	CHECK(boolLit.value == false);
	CHECK(boolLit.kind == NodeKind::BoolLit);
}

TEST_CASE("AST: BoolLit is not inferred by default") {
	// Arrange
	BoolLit boolLit(true);

	// Act
	bool isInferred = boolLit.isInferred();

	// Assert
	CHECK(!isInferred);
}

// ==================== NullLit Tests ====================

TEST_CASE("AST: NullLit construction") {
	// Arrange & Act
	NullLit nullLit;

	// Assert
	CHECK(nullLit.kind == NodeKind::NullLit);
}

TEST_CASE("AST: NullLit is not inferred by default") {
	// Arrange
	NullLit nullLit;

	// Act
	bool isInferred = nullLit.isInferred();

	// Assert
	CHECK(!isInferred);
}

// ==================== UnitLit Tests ====================

TEST_CASE("AST: UnitLit construction") {
	// Arrange & Act
	UnitLit unitLit;

	// Assert
	CHECK(unitLit.kind == NodeKind::UnitLit);
}

TEST_CASE("AST: UnitLit is not inferred by default") {
	// Arrange
	UnitLit unitLit;

	// Act
	bool isInferred = unitLit.isInferred();

	// Assert
	CHECK(!isInferred);
}

// ==================== VarRef Tests ====================

TEST_CASE("AST: VarRef construction with simple identifier") {
	// Arrange
	U8String ident = u8"x";

	// Act
	VarRef varRef(ident);

	// Assert
	CHECK(varRef.ident == u8"x");
	CHECK(varRef.kind == NodeKind::VarRef);
}

TEST_CASE("AST: VarRef construction with complex identifier") {
	// Arrange
	U8String ident = u8"my_variable_name_123";

	// Act
	VarRef varRef(ident);

	// Assert
	CHECK(varRef.ident == u8"my_variable_name_123");
	CHECK(varRef.kind == NodeKind::VarRef);
}

TEST_CASE("AST: VarRef construction with unicode identifier") {
	// Arrange
	U8String ident = u8"变量";

	// Act
	VarRef varRef(ident);

	// Assert
	CHECK(varRef.ident == u8"变量");
	CHECK(varRef.kind == NodeKind::VarRef);
}

TEST_CASE("AST: VarRef is not inferred by default") {
	// Arrange
	VarRef varRef(u8"x");

	// Act
	bool isInferred = varRef.isInferred();

	// Assert
	CHECK(!isInferred);
}

// ==================== BinaryExpr Tests ====================

TEST_CASE("AST: BinaryExpr construction with addition") {
	// Arrange
	auto left = std::make_unique<IntLit>(5);
	auto right = std::make_unique<IntLit>(3);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::Addition, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::Addition);
	CHECK(binaryExpr.kind == NodeKind::BinaryExpr);
	CHECK(binaryExpr.left->kind == NodeKind::IntLit);
	CHECK(binaryExpr.right->kind == NodeKind::IntLit);
}

TEST_CASE("AST: BinaryExpr construction with subtraction") {
	// Arrange
	auto left = std::make_unique<IntLit>(10);
	auto right = std::make_unique<IntLit>(4);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::Subtraction, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::Subtraction);
	CHECK(binaryExpr.kind == NodeKind::BinaryExpr);
}

TEST_CASE("AST: BinaryExpr construction with multiplication") {
	// Arrange
	auto left = std::make_unique<IntLit>(6);
	auto right = std::make_unique<IntLit>(7);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::Multiplication, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::Multiplication);
}

TEST_CASE("AST: BinaryExpr construction with division") {
	// Arrange
	auto left = std::make_unique<IntLit>(20);
	auto right = std::make_unique<IntLit>(5);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::Division, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::Division);
}

TEST_CASE("AST: BinaryExpr construction with modulo") {
	// Arrange
	auto left = std::make_unique<IntLit>(17);
	auto right = std::make_unique<IntLit>(5);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::Modulo, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::Modulo);
}

TEST_CASE("AST: BinaryExpr construction with equality comparison") {
	// Arrange
	auto left = std::make_unique<IntLit>(42);
	auto right = std::make_unique<IntLit>(42);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::Equality, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::Equality);
}

TEST_CASE("AST: BinaryExpr construction with less than comparison") {
	// Arrange
	auto left = std::make_unique<IntLit>(5);
	auto right = std::make_unique<IntLit>(10);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::LessThan, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::LessThan);
}

TEST_CASE("AST: BinaryExpr construction with logical AND") {
	// Arrange
	auto left = std::make_unique<BoolLit>(true);
	auto right = std::make_unique<BoolLit>(false);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::LogicalAnd, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::LogicalAnd);
}

TEST_CASE("AST: BinaryExpr construction with logical OR") {
	// Arrange
	auto left = std::make_unique<BoolLit>(true);
	auto right = std::make_unique<BoolLit>(true);

	// Act
	BinaryExpr binaryExpr(BinaryOpKind::LogicalOr, std::move(left), std::move(right));

	// Assert
	CHECK(binaryExpr.op == BinaryOpKind::LogicalOr);
}

TEST_CASE("AST: BinaryExpr is not inferred by default") {
	// Arrange
	auto left = std::make_unique<IntLit>(1);
	auto right = std::make_unique<IntLit>(2);
	BinaryExpr binaryExpr(BinaryOpKind::Addition, std::move(left), std::move(right));

	// Act
	bool isInferred = binaryExpr.isInferred();

	// Assert
	CHECK(!isInferred);
}

// ==================== UnaryExpr Tests ====================

TEST_CASE("AST: UnaryExpr construction with logical NOT") {
	// Arrange
	auto operand = std::make_unique<BoolLit>(true);

	// Act
	UnaryExpr unaryExpr(UnaryOpKind::LogicalNot, std::move(operand));

	// Assert
	CHECK(unaryExpr.op == UnaryOpKind::LogicalNot);
	CHECK(unaryExpr.kind == NodeKind::UnaryExpr);
}

TEST_CASE("AST: UnaryExpr construction with bitwise NOT") {
	// Arrange
	auto operand = std::make_unique<IntLit>(42);

	// Act
	UnaryExpr unaryExpr(UnaryOpKind::BitwiseNot, std::move(operand));

	// Assert
	CHECK(unaryExpr.op == UnaryOpKind::BitwiseNot);
}

TEST_CASE("AST: UnaryExpr construction with unary positive") {
	// Arrange
	auto operand = std::make_unique<IntLit>(5);

	// Act
	UnaryExpr unaryExpr(UnaryOpKind::Positive, std::move(operand));

	// Assert
	CHECK(unaryExpr.op == UnaryOpKind::Positive);
}

TEST_CASE("AST: UnaryExpr construction with unary negative") {
	// Arrange
	auto operand = std::make_unique<IntLit>(10);

	// Act
	UnaryExpr unaryExpr(UnaryOpKind::Negative, std::move(operand));

	// Assert
	CHECK(unaryExpr.op == UnaryOpKind::Negative);
}

TEST_CASE("AST: UnaryExpr construction with dereference") {
	// Arrange
	auto operand = std::make_unique<VarRef>(u8"ptr");

	// Act
	UnaryExpr unaryExpr(UnaryOpKind::Dereference, std::move(operand));

	// Assert
	CHECK(unaryExpr.op == UnaryOpKind::Dereference);
}

// ==================== Assignment Tests ====================

TEST_CASE("AST: Assignment construction with simple assignment") {
	// Arrange
	auto left = std::make_unique<VarRef>(u8"x");
	auto right = std::make_unique<IntLit>(42);

	// Act
	Assignment assignment(AssignmentKind::Simple, std::move(left), std::move(right));

	// Assert
	CHECK(assignment.assignmentKind == AssignmentKind::Simple);
	CHECK(assignment.kind == NodeKind::Assignment);
}

TEST_CASE("AST: Assignment construction with addition assignment") {
	// Arrange
	auto left = std::make_unique<VarRef>(u8"x");
	auto right = std::make_unique<IntLit>(5);

	// Act
	Assignment assignment(AssignmentKind::Addition, std::move(left), std::move(right));

	// Assert
	CHECK(assignment.assignmentKind == AssignmentKind::Addition);
}

TEST_CASE("AST: Assignment construction with subtraction assignment") {
	// Arrange
	auto left = std::make_unique<VarRef>(u8"count");
	auto right = std::make_unique<IntLit>(1);

	// Act
	Assignment assignment(AssignmentKind::Subtraction, std::move(left), std::move(right));

	// Assert
	CHECK(assignment.assignmentKind == AssignmentKind::Subtraction);
}

TEST_CASE("AST: Assignment construction with multiplication assignment") {
	// Arrange
	auto left = std::make_unique<VarRef>(u8"value");
	auto right = std::make_unique<IntLit>(2);

	// Act
	Assignment assignment(AssignmentKind::Multiplication, std::move(left), std::move(right));

	// Assert
	CHECK(assignment.assignmentKind == AssignmentKind::Multiplication);
}

// ==================== FieldAccess Tests ====================

TEST_CASE("AST: FieldAccess construction with simple field") {
	// Arrange
	auto base = std::make_unique<VarRef>(u8"obj");
	U8String field = u8"x";

	// Act
	FieldAccess fieldAccess(std::move(base), field);

	// Assert
	CHECK(fieldAccess.field == u8"x");
	CHECK(fieldAccess.kind == NodeKind::FieldAccess);
}

TEST_CASE("AST: FieldAccess construction with nested field") {
	// Arrange
	auto base = std::make_unique<VarRef>(u8"person");
	U8String field = u8"address";

	// Act
	FieldAccess fieldAccess(std::move(base), field);

	// Assert
	CHECK(fieldAccess.field == u8"address");
	CHECK(fieldAccess.kind == NodeKind::FieldAccess);
}

// ==================== FuncCall Tests ====================

TEST_CASE("AST: FuncCall construction with no arguments") {
	// Arrange
	auto expr = std::make_unique<VarRef>(u8"print");
	Vec<Box<Expr>> args;

	// Act
	FuncCall funcCall(std::move(expr), std::move(args));

	// Assert
	CHECK(funcCall.args.empty());
	CHECK(funcCall.isStructConstructor == false);
	CHECK(funcCall.kind == NodeKind::FuncCall);
}

TEST_CASE("AST: FuncCall construction with single argument") {
	// Arrange
	auto expr = std::make_unique<VarRef>(u8"print");
	Vec<Box<Expr>> args;
	args.push_back(std::make_unique<IntLit>(42));

	// Act
	FuncCall funcCall(std::move(expr), std::move(args));

	// Assert
	CHECK(funcCall.args.size() == 1);
	CHECK(funcCall.args[0]->kind == NodeKind::IntLit);
}

TEST_CASE("AST: FuncCall construction with multiple arguments") {
	// Arrange
	auto expr = std::make_unique<VarRef>(u8"add");
	Vec<Box<Expr>> args;
	args.push_back(std::make_unique<IntLit>(5));
	args.push_back(std::make_unique<IntLit>(3));

	// Act
	FuncCall funcCall(std::move(expr), std::move(args));

	// Assert
	CHECK(funcCall.args.size() == 2);
	CHECK(funcCall.args[0]->kind == NodeKind::IntLit);
	CHECK(funcCall.args[1]->kind == NodeKind::IntLit);
}

TEST_CASE("AST: FuncCall construction as struct constructor") {
	// Arrange
	auto expr = std::make_unique<VarRef>(u8"Point");
	Vec<Box<Expr>> args;
	args.push_back(std::make_unique<IntLit>(10));
	args.push_back(std::make_unique<IntLit>(20));

	// Act
	FuncCall funcCall(std::move(expr), std::move(args), true);

	// Assert
	CHECK(funcCall.isStructConstructor == true);
	CHECK(funcCall.args.size() == 2);
}

// ==================== BlockStmt Tests ====================

TEST_CASE("AST: BlockStmt construction with empty block") {
	// Arrange
	Vec<Box<Stmt>> stmts;

	// Act
	BlockStmt blockStmt(std::move(stmts));

	// Assert
	CHECK(blockStmt.stmts.empty());
	CHECK(blockStmt.kind == NodeKind::BlockStmt);
}

TEST_CASE("AST: BlockStmt construction with single statement") {
	// Arrange
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(42)));

	// Act
	BlockStmt blockStmt(std::move(stmts));

	// Assert
	CHECK(blockStmt.stmts.size() == 1);
	CHECK(blockStmt.stmts[0]->kind == NodeKind::ReturnStmt);
}

TEST_CASE("AST: BlockStmt construction with multiple statements") {
	// Arrange
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(1)));
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(2)));

	// Act
	BlockStmt blockStmt(std::move(stmts));

	// Assert
	CHECK(blockStmt.stmts.size() == 2);
	CHECK(blockStmt.stmts[0]->kind == NodeKind::ReturnStmt);
	CHECK(blockStmt.stmts[1]->kind == NodeKind::ReturnStmt);
}

// ==================== ReturnStmt Tests ====================

TEST_CASE("AST: ReturnStmt construction with int literal") {
	// Arrange
	auto expr = std::make_unique<IntLit>(42);

	// Act
	ReturnStmt returnStmt(std::move(expr));

	// Assert
	CHECK(returnStmt.expr->kind == NodeKind::IntLit);
	CHECK(returnStmt.kind == NodeKind::ReturnStmt);
}

TEST_CASE("AST: ReturnStmt construction with variable reference") {
	// Arrange
	auto expr = std::make_unique<VarRef>(u8"result");

	// Act
	ReturnStmt returnStmt(std::move(expr));

	// Assert
	CHECK(returnStmt.expr->kind == NodeKind::VarRef);
}

TEST_CASE("AST: ReturnStmt construction with unit literal") {
	// Arrange
	auto expr = std::make_unique<UnitLit>();

	// Act
	ReturnStmt returnStmt(std::move(expr));

	// Assert
	CHECK(returnStmt.expr->kind == NodeKind::UnitLit);
}

// ==================== VarDef Tests ====================

TEST_CASE("AST: VarDef construction with simple type") {
	// Arrange
	U8String ident = u8"x";
	Type type = TypeFactory::getI32();
	auto value = std::make_unique<IntLit>(42);

	// Act
	VarDef varDef(ident, type, std::move(value));

	// Assert
	CHECK(varDef.ident == u8"x");
	CHECK(varDef.value->kind == NodeKind::IntLit);
	CHECK(varDef.kind == NodeKind::VarDef);
}

TEST_CASE("AST: VarDef construction with char type") {
	// Arrange
	U8String ident = u8"ch";
	Type type = TypeFactory::getChar();
	auto value = std::make_unique<CharLit>(U'a');

	// Act
	VarDef varDef(ident, type, std::move(value));

	// Assert
	CHECK(varDef.ident == u8"ch");
	CHECK(varDef.value->kind == NodeKind::CharLit);
}

TEST_CASE("AST: VarDef construction with bool type") {
	// Arrange
	U8String ident = u8"flag";
	Type type = TypeFactory::getBool();
	auto value = std::make_unique<BoolLit>(true);

	// Act
	VarDef varDef(ident, type, std::move(value));

	// Assert
	CHECK(varDef.ident == u8"flag");
	CHECK(varDef.value->kind == NodeKind::BoolLit);
}

// ==================== IfStmt Tests ====================

TEST_CASE("AST: IfStmt construction with simple condition") {
	// Arrange
	auto cond = std::make_unique<BoolLit>(true);
	Vec<Box<Stmt>> thenStmts;
	thenStmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(1)));
	auto thenBlock = std::make_unique<BlockStmt>(std::move(thenStmts));
	Vec<Box<Stmt>> elseStmts;
	elseStmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(0)));
	auto elseBlock = std::make_unique<BlockStmt>(std::move(elseStmts));

	// Act
	IfStmt ifStmt(std::move(cond), std::move(thenBlock), std::move(elseBlock));

	// Assert
	CHECK(ifStmt.cond->kind == NodeKind::BoolLit);
	CHECK(ifStmt.then->kind == NodeKind::BlockStmt);
	CHECK(ifStmt.else_->kind == NodeKind::BlockStmt);
	CHECK(ifStmt.kind == NodeKind::IfStmt);
}

TEST_CASE("AST: IfStmt construction with comparison condition") {
	// Arrange
	auto leftCond = std::make_unique<IntLit>(5);
	auto rightCond = std::make_unique<IntLit>(10);
	auto cond = std::make_unique<BinaryExpr>(BinaryOpKind::LessThan, std::move(leftCond), std::move(rightCond));
	Vec<Box<Stmt>> thenStmts;
	auto thenBlock = std::make_unique<BlockStmt>(std::move(thenStmts));
	Vec<Box<Stmt>> elseStmts;
	auto elseBlock = std::make_unique<BlockStmt>(std::move(elseStmts));

	// Act
	IfStmt ifStmt(std::move(cond), std::move(thenBlock), std::move(elseBlock));

	// Assert
	CHECK(ifStmt.cond->kind == NodeKind::BinaryExpr);
}

// ==================== WhileStmt Tests ====================

TEST_CASE("AST: WhileStmt construction with bool literal condition") {
	// Arrange
	auto cond = std::make_unique<BoolLit>(true);
	Vec<Box<Stmt>> bodyStmts;
	bodyStmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<UnitLit>()));
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));

	// Act
	WhileStmt whileStmt(std::move(cond), std::move(body));

	// Assert
	CHECK(whileStmt.cond->kind == NodeKind::BoolLit);
	CHECK(whileStmt.body->kind == NodeKind::BlockStmt);
	CHECK(whileStmt.kind == NodeKind::WhileStmt);
}

TEST_CASE("AST: WhileStmt construction with comparison condition") {
	// Arrange
	auto leftCond = std::make_unique<VarRef>(u8"i");
	auto rightCond = std::make_unique<IntLit>(10);
	auto cond = std::make_unique<BinaryExpr>(BinaryOpKind::LessThan, std::move(leftCond), std::move(rightCond));
	Vec<Box<Stmt>> bodyStmts;
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));

	// Act
	WhileStmt whileStmt(std::move(cond), std::move(body));

	// Assert
	CHECK(whileStmt.cond->kind == NodeKind::BinaryExpr);
	CHECK(whileStmt.body->kind == NodeKind::BlockStmt);
}

// ==================== HeapAlloc Tests ====================

TEST_CASE("AST: HeapAlloc construction with primitive type") {
	// Arrange
	Type type = TypeFactory::getI32();
	auto expr = std::make_unique<IntLit>(42);

	// Act
	HeapAlloc heapAlloc(type, std::move(expr));

	// Assert
	CHECK(heapAlloc.expr->kind == NodeKind::IntLit);
	CHECK(heapAlloc.kind == NodeKind::HeapAlloc);
}

TEST_CASE("AST: HeapAlloc construction with pointer type") {
	// Arrange
	Type baseType = TypeFactory::getI32();
	Type type = TypeFactory::getPointer(baseType);
	auto expr = std::make_unique<VarRef>(u8"x");

	// Act
	HeapAlloc heapAlloc(type, std::move(expr));

	// Assert
	CHECK(heapAlloc.expr->kind == NodeKind::VarRef);
	CHECK(heapAlloc.kind == NodeKind::HeapAlloc);
}

// ==================== FuncDecl Tests ====================

TEST_CASE("AST: FuncDecl construction with no parameters") {
	// Arrange
	U8String ident = u8"getValue";
	Vec<Param> params;
	Type returnType = TypeFactory::getI32();
	Vec<Box<Stmt>> bodyStmts;
	bodyStmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(42)));
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));

	// Act
	FuncDecl funcDecl(ident, params, returnType, std::move(body));

	// Assert
	CHECK(funcDecl.ident == u8"getValue");
	CHECK(funcDecl.params.empty());
	CHECK(funcDecl.body->kind == NodeKind::BlockStmt);
	CHECK(funcDecl.kind == NodeKind::FuncDecl);
}

TEST_CASE("AST: FuncDecl construction with single parameter") {
	// Arrange
	U8String ident = u8"square";
	Vec<Param> params;
	params.push_back({u8"x", TypeFactory::getI32()});
	Type returnType = TypeFactory::getI32();
	Vec<Box<Stmt>> bodyStmts;
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));

	// Act
	FuncDecl funcDecl(ident, params, returnType, std::move(body));

	// Assert
	CHECK(funcDecl.ident == u8"square");
	CHECK(funcDecl.params.size() == 1);
	CHECK(funcDecl.params[0].first == u8"x");
}

TEST_CASE("AST: FuncDecl construction with multiple parameters") {
	// Arrange
	U8String ident = u8"add";
	Vec<Param> params;
	params.push_back({u8"a", TypeFactory::getI32()});
	params.push_back({u8"b", TypeFactory::getI32()});
	Type returnType = TypeFactory::getI32();
	Vec<Box<Stmt>> bodyStmts;
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));

	// Act
	FuncDecl funcDecl(ident, params, returnType, std::move(body));

	// Assert
	CHECK(funcDecl.ident == u8"add");
	CHECK(funcDecl.params.size() == 2);
	CHECK(funcDecl.params[0].first == u8"a");
	CHECK(funcDecl.params[1].first == u8"b");
}

TEST_CASE("AST: FuncDecl construction with void return type") {
	// Arrange
	U8String ident = u8"printValue";
	Vec<Param> params;
	params.push_back({u8"val", TypeFactory::getI32()});
	Type returnType = TypeFactory::getUnit();
	Vec<Box<Stmt>> bodyStmts;
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));

	// Act
	FuncDecl funcDecl(ident, params, returnType, std::move(body));

	// Assert
	CHECK(funcDecl.ident == u8"printValue");
	CHECK(funcDecl.returnType->isTypeKind(TypeKind::Unit));
}

// ==================== StructDecl Tests ====================

TEST_CASE("AST: StructDecl construction with no fields") {
	// Arrange
	U8String ident = u8"Empty";
	Vec<StructField> fields;

	// Act
	StructDecl structDecl(ident, fields);

	// Assert
	CHECK(structDecl.ident == u8"Empty");
	CHECK(structDecl.fields.empty());
	CHECK(structDecl.kind == NodeKind::StructDecl);
}

TEST_CASE("AST: StructDecl construction with single field") {
	// Arrange
	U8String ident = u8"Point";
	Vec<StructField> fields;
	fields.push_back({u8"x", TypeFactory::getI32()});

	// Act
	StructDecl structDecl(ident, fields);

	// Assert
	CHECK(structDecl.ident == u8"Point");
	CHECK(structDecl.fields.size() == 1);
	CHECK(structDecl.fields[0].first == u8"x");
}

TEST_CASE("AST: StructDecl construction with multiple fields") {
	// Arrange
	U8String ident = u8"Rectangle";
	Vec<StructField> fields;
	fields.push_back({u8"x", TypeFactory::getI32()});
	fields.push_back({u8"y", TypeFactory::getI32()});
	fields.push_back({u8"width", TypeFactory::getI32()});
	fields.push_back({u8"height", TypeFactory::getI32()});

	// Act
	StructDecl structDecl(ident, fields);

	// Assert
	CHECK(structDecl.ident == u8"Rectangle");
	CHECK(structDecl.fields.size() == 4);
	CHECK(structDecl.fields[0].first == u8"x");
	CHECK(structDecl.fields[1].first == u8"y");
	CHECK(structDecl.fields[2].first == u8"width");
	CHECK(structDecl.fields[3].first == u8"height");
}

TEST_CASE("AST: StructDecl construction with mixed field types") {
	// Arrange
	U8String ident = u8"Person";
	Vec<StructField> fields;
	fields.push_back({u8"age", TypeFactory::getI32()});
	fields.push_back({u8"isActive", TypeFactory::getBool()});
	fields.push_back({u8"initial", TypeFactory::getChar()});

	// Act
	StructDecl structDecl(ident, fields);

	// Assert
	CHECK(structDecl.ident == u8"Person");
	CHECK(structDecl.fields.size() == 3);
}

// ==================== Module Tests ====================

TEST_CASE("AST: Module construction with no functions or structs") {
	// Arrange
	U8String name = u8"empty_module";
	Vec<Box<FuncDecl>> funcs;
	Vec<Box<StructDecl>> structs;

	// Act
	Module module(name, std::move(funcs), std::move(structs));

	// Assert
	CHECK(module.name == u8"empty_module");
	CHECK(module.funcs.empty());
	CHECK(module.structs.empty());
	CHECK(module.kind == NodeKind::Module);
}

TEST_CASE("AST: Module construction with single function") {
	// Arrange
	U8String name = u8"my_module";
	Vec<Box<FuncDecl>> funcs;
	Vec<Param> params;
	Type returnType = TypeFactory::getUnit();
	Vec<Box<Stmt>> bodyStmts;
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
	funcs.push_back(std::make_unique<FuncDecl>(u8"main", params, returnType, std::move(body)));
	Vec<Box<StructDecl>> structs;

	// Act
	Module module(name, std::move(funcs), std::move(structs));

	// Assert
	CHECK(module.name == u8"my_module");
	CHECK(module.funcs.size() == 1);
	CHECK(module.structs.empty());
}

TEST_CASE("AST: Module construction with multiple functions") {
	// Arrange
	U8String name = u8"math_module";
	Vec<Box<FuncDecl>> funcs;
	Vec<Box<StructDecl>> structs;

	for (int i = 0; i < 3; ++i) {
		Vec<Param> params;
		Type returnType = TypeFactory::getI32();
		Vec<Box<Stmt>> bodyStmts;
		auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
		U8String funcName(u8"func");
		funcName += U8String(std::to_string(i));
		funcs.push_back(std::make_unique<FuncDecl>(funcName, params, returnType, std::move(body)));
	}

	// Act
	Module module(name, std::move(funcs), std::move(structs));

	// Assert
	CHECK(module.name == u8"math_module");
	CHECK(module.funcs.size() == 3);
	CHECK(module.structs.empty());
}

TEST_CASE("AST: Module construction with single struct") {
	// Arrange
	U8String name = u8"data_module";
	Vec<Box<FuncDecl>> funcs;
	Vec<Box<StructDecl>> structs;
	Vec<StructField> fields;
	fields.push_back({u8"value", TypeFactory::getI32()});
	structs.push_back(std::make_unique<StructDecl>(u8"MyStruct", fields));

	// Act
	Module module(name, std::move(funcs), std::move(structs));

	// Assert
	CHECK(module.name == u8"data_module");
	CHECK(module.funcs.empty());
	CHECK(module.structs.size() == 1);
}

TEST_CASE("AST: Module construction with functions and structs") {
	// Arrange
	U8String name = u8"complex_module";
	Vec<Box<FuncDecl>> funcs;
	Vec<Box<StructDecl>> structs;

	// Add a function
	Vec<Param> params;
	Type returnType = TypeFactory::getUnit();
	Vec<Box<Stmt>> bodyStmts;
	auto body = std::make_unique<BlockStmt>(std::move(bodyStmts));
	funcs.push_back(std::make_unique<FuncDecl>(u8"process", params, returnType, std::move(body)));

	// Add a struct
	Vec<StructField> fields;
	fields.push_back({u8"id", TypeFactory::getI32()});
	structs.push_back(std::make_unique<StructDecl>(u8"Record", fields));

	// Act
	Module module(name, std::move(funcs), std::move(structs));

	// Assert
	CHECK(module.name == u8"complex_module");
	CHECK(module.funcs.size() == 1);
	CHECK(module.structs.size() == 1);
}

// ==================== Type Inference Tests ====================

TEST_CASE("AST: Expr::infer() sets type and value category") {
	// Arrange
	IntLit intLit(42);
	Type type = TypeFactory::getI32();

	// Act
	intLit.infer(type, ValueCategory::RValue);

	// Assert
	CHECK(intLit.isInferred());
	CHECK(intLit.inferredType.has_value());
	CHECK(intLit.valueCategory == ValueCategory::RValue);
}

TEST_CASE("AST: Expr::infer() works with different value categories") {
	// Arrange
	VarRef varRef(u8"x");
	Type type = TypeFactory::getI32();

	// Act
	varRef.infer(type, ValueCategory::LValue);

	// Assert
	CHECK(varRef.isInferred());
	CHECK(varRef.valueCategory == ValueCategory::LValue);
}

TEST_CASE("AST: Expr::infer() can be called multiple times") {
	// Arrange
	IntLit intLit(10);
	Type i32Type = TypeFactory::getI32();
	Type boolType = TypeFactory::getBool();

	// Act
	intLit.infer(i32Type, ValueCategory::RValue);
	intLit.infer(boolType, ValueCategory::LValue);

	// Assert
	CHECK(intLit.isInferred());
	CHECK(intLit.inferredType.has_value());
	CHECK(intLit.valueCategory == ValueCategory::LValue);
}














