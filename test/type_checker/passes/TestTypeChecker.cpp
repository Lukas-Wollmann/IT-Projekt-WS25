#include "Doctest.h"
#include "ast/PrintVisitor.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"

using namespace ast;
using namespace type;
using namespace semantic;

TEST_CASE("TypeCheckingPass: IntLit type will be infered as Typename i32") {
	// Arrange
	auto intLit = std::make_unique<IntLit>(67);
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	// Act
	tc.dispatch(*intLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(intLit->inferredType.has_value());
    CHECK(intLit->valueCategory.has_value());
	CHECK(intLit->value);

	auto type = intLit->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Typename));

	auto typename_ = std::static_pointer_cast<const Typename>(type);
	CHECK(typename_->typename_ == u8"i32");
}

#if 0
TEST_CASE("TypeChecker: FloatLit type will be infered as Typename f32") {
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
	CHECK(type.kind == TypeKind::Typename);

	auto &typename_ = static_cast<const Typename &>(type);
	CHECK(typename_.typename_ == u8"f32");
}

TEST_CASE("TypeChecker: CharLit type will be infered as Typename char") {
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
	CHECK(type.kind == TypeKind::Typename);

	auto &typename_ = static_cast<const Typename &>(type);
	CHECK(typename_.typename_ == u8"char");
}

TEST_CASE("TypeChecker: BoolLit type will be infered as Typename bool") {
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
	CHECK(type.kind == TypeKind::Typename);

	auto &typename_ = static_cast<const Typename &>(type);
	CHECK(typename_.typename_ == u8"bool");
}

TEST_CASE("TypeChecker: StringLit type will be infered as Typename string") {
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
	CHECK(type.kind == TypeKind::Typename);

	auto &typename_ = static_cast<const Typename &>(type);
	CHECK(typename_.typename_ == u8"string");
}

TEST_CASE("TypeChecker: UnitLit type will be infered as UnitType") {
	// Arrange
	auto unitLit = std::make_unique<UnitLit>();
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	// Act
	tc.dispatch(*unitLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(unitLit->inferredType);

	const Type &type = *unitLit->inferredType.value();
	CHECK(type.kind == TypeKind::Unit);
}

TEST_CASE("TypeChecker: ReturnStmt works if return expression has correct type") {
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<FloatLit>(10.0f)));

	auto funcDecl = std::make_unique<FuncDecl>(u8"testFunction", Vec<Param>{},
											   std::make_unique<Typename>(u8"f32"),
											   std::make_unique<BlockStmt>(std::move(stmts)));

	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	tc.dispatch(*funcDecl);

	for (const auto &err : ctx.getErrors())
		std::cout << err << std::endl;
}

TEST_CASE("TypeChecker: Playground") {
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<UnitLit>());
	
    // foo: i32 = 5
    stmts.push_back(std::make_unique<VarDef>(
        u8"foo",
        std::make_unique<Typename>(u8"i32"),
		std::make_unique<IntLit>(3)
    ));

    // foo += 2
    stmts.push_back(std::make_unique<Assignment>(
        AssignmentKind::Addition, 
        std::make_unique<IntLit>(3),
        std::make_unique<IntLit>(3)
    ));

	auto funcDecl =
			std::make_unique<FuncDecl>(u8"testFunction", Vec<Param>{}, std::make_unique<UnitType>(),
									   std::make_unique<BlockStmt>(std::move(stmts)));

	TypeCheckerContext ctx;
	ExplorationPass ep(ctx);
	ep.dispatch(*funcDecl);
	TypeCheckingPass tc(ctx);
	tc.dispatch(*funcDecl);

	for (const auto &err : ctx.getErrors())
		std::cout << err << std::endl;
}
#endif