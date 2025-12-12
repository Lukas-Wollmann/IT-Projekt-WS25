#include "Doctest.h"
#include "ast/PrintVisitor.h"
#include "type_checker/passes/ExplorationPass.h"
#include "type_checker/passes/TypeCheckingPass.h"

using namespace ast;
using namespace type;

TEST_CASE("TypeChecker: IntLit type will be infered as PrimitiveType::I32") {
	// Arrange
	auto intLit = std::make_unique<IntLit>(67);
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	// Act
	tc.dispatch(*intLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(intLit->inferredType);

	auto &type = *intLit->inferredType.value();
	CHECK(intLit->inferredType.value()->kind == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.primitiveKind == PrimitiveTypeKind::I32);
}

TEST_CASE("TypeChecker: FloatLit type will be infered as PrimitiveType::F32") {
	// Arrange
	auto floatLit = std::make_unique<FloatLit>(187.0f);
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	// Act
	tc.dispatch(*floatLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(floatLit->inferredType);

	const Type &type = *floatLit->inferredType.value();
	CHECK(type.kind == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.primitiveKind == PrimitiveTypeKind::F32);
}

TEST_CASE("TypeChecker: CharLit type will be infered as PrimitiveType::Char") {
	// Arrange
	auto charLit = std::make_unique<CharLit>('X');
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	// Act
	tc.dispatch(*charLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(charLit->inferredType);

	const Type &type = *charLit->inferredType.value();
	CHECK(type.kind == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.primitiveKind == PrimitiveTypeKind::Char);
}

TEST_CASE("TypeChecker: BoolLit type will be infered as PrimitiveType::Bool") {
	// Arrange
	auto boolLit = std::make_unique<BoolLit>(false);
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	// Act
	tc.dispatch(*boolLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(boolLit->inferredType);

	const Type &type = *boolLit->inferredType.value();
	CHECK(type.kind == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.primitiveKind == PrimitiveTypeKind::Bool);
}

TEST_CASE("TypeChecker: StringLit type will be infered as PrimitiveType::String") {
	// Arrange
	auto strLit = std::make_unique<StringLit>(u8"UwU");
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	// Act
	tc.dispatch(*strLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(strLit->inferredType);

	const Type &type = *strLit->inferredType.value();
	CHECK(type.kind == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.primitiveKind == PrimitiveTypeKind::String);
}

TEST_CASE("TypeChecker: ReturnStmt works if return expression has correct type") {
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<FloatLit>(10.0f)));

	auto funcDecl =
			std::make_unique<FuncDecl>(u8"testFunction", Vec<Param>{},
									   std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32),
									   std::make_unique<BlockStmt>(std::move(stmts)));

	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	tc.dispatch(*funcDecl);

	for (const auto &err : ctx.getErrors())
		std::cout << err << std::endl;
}

TEST_CASE("TypeChecker: Playground") {
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<BinaryExpr>(
		BinaryOpKind::Multiplication, 
		std::make_unique<StringLit>(u8"HI"), 
		std::make_unique<StringLit>(u8"GRR")
	));

	auto funcDecl =
			std::make_unique<FuncDecl>(u8"testFunction", Vec<Param>{},
									   std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32),
									   std::make_unique<BlockStmt>(std::move(stmts)));

	TypeCheckerContext ctx;
	ExplorationPass ep(ctx);
	ep.dispatch(*funcDecl);
	TypeCheckingPass tc(ctx);
	tc.dispatch(*funcDecl);

	for (const auto &err : ctx.getErrors())
		std::cout << err << std::endl;
}