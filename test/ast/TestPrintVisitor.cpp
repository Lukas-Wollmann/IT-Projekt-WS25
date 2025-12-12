#include "Doctest.h"
#include "ast/PrintVisitor.h"

using namespace ast;
using namespace type;

TEST_CASE("IntLit: toString works") {
	// Arrange
	auto intLit = std::make_unique<IntLit>(42);
	std::stringstream ss;

	// Act
	ss << *intLit;
	std::string result = ss.str();

	// Assert
	CHECK(result == "IntLit(42)");
}

TEST_CASE("FloatLit: toString works") {
	// Arrange
	auto floatLit = std::make_unique<FloatLit>(67.4f);
	std::stringstream ss;

	// Act
	ss << *floatLit;
	std::string result = ss.str();

	// Assert
	CHECK(result == "FloatLit(67.4)");
}

TEST_CASE("CharLit: toString works") {
	// Arrange
	char32_t flowerEmoji = U'\U0001f33a';
	auto charLit = std::make_unique<CharLit>(flowerEmoji);
	std::stringstream ss;

	// Act
	ss << *charLit;
	std::string result = ss.str();

	// Assert
	CHECK(result == "CharLit('\U0001f33a')");
}

TEST_CASE("BoolLit: toString works") {
	// Arrange
	auto boolLit = std::make_unique<BoolLit>(true);
	std::stringstream ss;

	// Act
	ss << *boolLit;
	std::string result = ss.str();

	// Assert
	CHECK(result == "BoolLit(true)");
}

TEST_CASE("StringLit: toString works") {
	// Arrange
	auto strLit = std::make_unique<StringLit>(u8"burger king");
	std::stringstream ss;

	// Act
	ss << *strLit;
	std::string result = ss.str();

	CHECK(result == "StringLit(\"burger king\")");
}

TEST_CASE("ArrayExpr: toString works") {
	// Arrange
	auto arrayType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
	auto value1 = std::make_unique<IntLit>(187);
	auto value2 = std::make_unique<IntLit>(67);

	Vec<Box<Expr>> exprs;
	exprs.push_back(std::move(value1));
	exprs.push_back(std::move(value2));

	auto arrayExpr = std::make_unique<ArrayExpr>(std::move(arrayType), std::move(exprs));

	std::stringstream ss;

	// Act
	ss << *arrayExpr;
	std::string result = ss.str();

	// Assert
	CHECK(result == "ArrayExpr(i32, {IntLit(187), IntLit(67)})");
}

TEST_CASE("UnaryExpr: toString works") {
	// Arrange
	auto operand = std::make_unique<IntLit>(7);
	auto unary = std::make_unique<UnaryExpr>(UnaryOpKind::Negative, std::move(operand));
	std::stringstream ss;

	// Act
	ss << *unary;
	std::string result = ss.str();

	// Assert
	CHECK(result == "UnaryExpr(-, IntLit(7))");
}

TEST_CASE("BinaryExpr: toString works") {
	// Arrange
	auto left = std::make_unique<IntLit>(2);
	auto right = std::make_unique<IntLit>(3);
	auto bin =
			std::make_unique<BinaryExpr>(BinaryOpKind::Addition, std::move(left), std::move(right));
	std::stringstream ss;

	// Act
	ss << *bin;
	std::string result = ss.str();

	// Assert
	CHECK(result == "BinaryExpr(IntLit(2), +, IntLit(3))");
}

TEST_CASE("Assignment: toString works") {
	// Arrange
	auto left = std::make_unique<VarRef>(u8"x");
	auto right = std::make_unique<IntLit>(3);
	auto assign = std::make_unique<Assignment>(AssignmentKind::Multiplication, std::move(left),
											   std::move(right));
	std::stringstream ss;

	// Act
	ss << *assign;
	std::string result = ss.str();

	// Assert
	CHECK(result == "Assignment(VarRef(x), *=, IntLit(3))");
}

TEST_CASE("VarRef: toString works") {
	// Arrange
	auto var = std::make_unique<VarRef>(u8"x");
	std::stringstream ss;

	// Act
	ss << *var;
	std::string result = ss.str();

	// Assert
	CHECK(result == "VarRef(x)");
}

TEST_CASE("FuncCall: toString works") {
	// Arrange
	Vec<Box<Expr>> args;
	args.push_back(std::make_unique<IntLit>(1));
	args.push_back(std::make_unique<IntLit>(2));

	auto expr = std::make_unique<VarRef>(u8"sum");
	auto call = std::make_unique<FuncCall>(std::move(expr), std::move(args));
	std::stringstream ss;

	// Act
	ss << *call;
	std::string result = ss.str();

	// Assert
	CHECK(result == "FuncCall(VarRef(sum), {IntLit(1), IntLit(2)})");
}

TEST_CASE("BlockStmt: toString work") {
	// Arrange
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(1)));
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(2)));

	auto block = std::make_unique<BlockStmt>(std::move(stmts));
	std::stringstream ss;

	// Act
	ss << *block;
	std::string result = ss.str();

	// Assert
	CHECK(result == "BlockStmt({ReturnStmt(IntLit(1)), ReturnStmt(IntLit(2))})");
}

TEST_CASE("IfStmt: toString works") {
	// Arrange
	auto cond = std::make_unique<BoolLit>(true);
	auto thenBlock = std::make_unique<BlockStmt>(Vec<Box<Stmt>>{});
	auto elseBlock = std::make_unique<BlockStmt>(Vec<Box<Stmt>>{});

	auto ifs =
			std::make_unique<IfStmt>(std::move(cond), std::move(thenBlock), std::move(elseBlock));
	std::stringstream ss;

	// Act
	ss << *ifs;
	std::string result = ss.str();

	// Assert
	CHECK(result == "IfStmt(BoolLit(true), BlockStmt({}), BlockStmt({}))");
}

TEST_CASE("WhileStmt: toString works") {
	// Arrange
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(0)));

	auto body = std::make_unique<BlockStmt>(std::move(stmts));
	auto whileCond = std::make_unique<BoolLit>(false);
	auto wh = std::make_unique<WhileStmt>(std::move(whileCond), std::move(body));
	std::stringstream ss;

	// Act
	ss << *wh;
	std::string result = ss.str();

	// Assert
	CHECK(result == "WhileStmt(BoolLit(false), BlockStmt({ReturnStmt(IntLit(0))}))");
}

TEST_CASE("ReturnStmt: toString works") {
	// Arrange
	auto retExpr = std::make_unique<IntLit>(7);
	auto ret = std::make_unique<ReturnStmt>(std::move(retExpr));
	std::stringstream ss;

	// Act
	ss << *ret;
	std::string result = ss.str();

	// Assert
	CHECK(result == "ReturnStmt(IntLit(7))");
}

TEST_CASE("VarDef: toString works") {
	// Arrange
	auto type = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
	auto val = std::make_unique<IntLit>(99);
	auto decl = std::make_unique<VarDef>(u8"myVar", std::move(type), std::move(val));
	std::stringstream ss;

	// Act
	ss << *decl;
	std::string result = ss.str();

	// Assert
	CHECK(result == "VarDef(myVar, i32, IntLit(99))");
}

TEST_CASE("FuncDecl: toString works") {
	// Arrange
	Vec<Param> params;
	params.push_back({u8"a", std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32)});
	params.push_back({u8"b", std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32)});

	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(0)));

	auto body = std::make_unique<BlockStmt>(std::move(stmts));
	auto retType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
	auto func = std::make_unique<FuncDecl>(u8"foo", std::move(params), std::move(retType),
										   std::move(body));
	std::stringstream ss;

	// Act
	ss << *func;
	std::string result = ss.str();

	// Assert
	CHECK(result == "FuncDecl(foo, {a: i32, b: f32}, bool, BlockStmt({ReturnStmt(IntLit(0))}))");
}