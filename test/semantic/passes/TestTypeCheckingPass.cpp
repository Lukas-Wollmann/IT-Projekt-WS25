#include "Doctest.h"
#include "ast/PrintVisitor.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"
#include "type/CompareVisitor.h"

using namespace ast;
using namespace type;
using namespace semantic;

TEST_CASE("TypeCheckingPass: IntLit type will be infered as Typename i32") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto intLit = std::make_unique<IntLit>(67);

	// Act
	tc.dispatch(*intLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(intLit->inferredType.has_value());
	CHECK(intLit->valueCategory.has_value());

	auto type = intLit->inferredType.value();
	CHECK(*type == Typename(u8"i32"));
	CHECK(intLit->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: FloatLit type will be infered as Typename f32") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto floatLit = std::make_unique<FloatLit>(67.0f);

	// Act
	tc.dispatch(*floatLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(floatLit->inferredType.has_value());
	CHECK(floatLit->valueCategory.has_value());

	auto type = floatLit->inferredType.value();
	CHECK(*type == Typename(u8"f32"));
	CHECK(floatLit->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: CharLit type will be infered as Typename char") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto charLit = std::make_unique<CharLit>(U'👻');

	// Act
	tc.dispatch(*charLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(charLit->inferredType.has_value());
	CHECK(charLit->valueCategory.has_value());

	auto type = charLit->inferredType.value();
	CHECK(*type == Typename(u8"char"));
}

TEST_CASE("TypeCheckingPass: BoolLit type will be infered as Typename bool") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto boolLit = std::make_unique<BoolLit>(false);

	// Act
	tc.dispatch(*boolLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(boolLit->inferredType.has_value());
	CHECK(boolLit->valueCategory.has_value());

	auto type = boolLit->inferredType.value();
	CHECK(*type == Typename(u8"bool"));
	CHECK(boolLit->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: StringLit type will be infered as Typename string") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto strLit = std::make_unique<StringLit>(u8"👾BeepBoop👾");

	// Act
	tc.dispatch(*strLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(strLit->inferredType.has_value());
	CHECK(strLit->valueCategory.has_value());

	auto type = strLit->inferredType.value();
	CHECK(*type == Typename(u8"string"));
	CHECK(strLit->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: UnitLit type will be infered as UnitLit") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto unitLit = std::make_unique<UnitLit>();

	// Act
	tc.dispatch(*unitLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(unitLit->inferredType.has_value());
	CHECK(unitLit->valueCategory.has_value());

	auto type = unitLit->inferredType.value();
	CHECK(*type == UnitType());
	CHECK(unitLit->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: ArrayLit type will be infered as ArrayType of element type and "
		  "correct size") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	Vec<Box<Expr>> elements;
	elements.push_back(std::make_unique<IntLit>(1));
	elements.push_back(std::make_unique<IntLit>(2));
	elements.push_back(std::make_unique<IntLit>(3));
	auto elemType = std::make_shared<Typename>(u8"i32");
	auto arrayLit = std::make_unique<ArrayExpr>(elemType, std::move(elements));

	// Act
	tc.dispatch(*arrayLit);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(arrayLit->inferredType.has_value());
	CHECK(arrayLit->valueCategory.has_value());

	auto type = arrayLit->inferredType.value();
	CHECK(*type == ArrayType(elemType, 3));
	CHECK(arrayLit->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: ArrayLit will error if element type missmatch") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	Vec<Box<Expr>> elements;
	elements.push_back(std::make_unique<IntLit>(1));
	elements.push_back(std::make_unique<FloatLit>(2.0f));
	elements.push_back(std::make_unique<IntLit>(3));
	auto elemType = std::make_shared<Typename>(u8"i32");
	auto arrayLit = std::make_unique<ArrayExpr>(elemType, std::move(elements));

	// Act
	tc.dispatch(*arrayLit);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(arrayLit->inferredType.has_value());
	CHECK(arrayLit->valueCategory.has_value());

	auto type = arrayLit->inferredType.value();
	CHECK(*type == ArrayType(elemType, 3));
	CHECK(arrayLit->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: UnaryExpr will infer correct type for valid operator") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr = std::make_unique<UnaryExpr>(
		UnaryOpKind::Negative,
		std::make_unique<IntLit>(42)
	);

	// Act
	tc.dispatch(*expr);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(*type == Typename(u8"i32"));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: UnaryExpr will error for invalid operand type") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr = std::make_unique<UnaryExpr>(
		UnaryOpKind::Not,
		std::make_unique<StringLit>(u8"67")
	);

	// Act
	tc.dispatch(*expr);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: UnaryExpr will propagate ErrorType if operand has ErrorType") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto operand = std::make_unique<UnaryExpr>(
		UnaryOpKind::Not,
		std::make_unique<StringLit>(u8"67")
	);

	auto expr = std::make_unique<UnaryExpr>(
		UnaryOpKind::Negative,
		std::move(operand)
	);

	// Act
	tc.dispatch(*expr);

	// Assert
	// Still only one error, second unary expr should only propagate the error
	CHECK(ctx.getErrors().size() == 1);
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);

}

TEST_CASE("TypeCheckingPass: BinaryExpr will infer correct type for valid operator") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr = std::make_unique<BinaryExpr>(
		BinaryOpKind::Equality,
		std::make_unique<IntLit>(42),
		std::make_unique<IntLit>(58)
	);

	// Act
	tc.dispatch(*expr);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(*type == Typename(u8"bool"));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: BinaryExpr will error for invalid operand types") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr = std::make_unique<BinaryExpr>(
		BinaryOpKind::Modulo,
		std::make_unique<StringLit>(u8"🦒"),
		std::make_unique<StringLit>(u8"67")
	);

	// Act
	tc.dispatch(*expr);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: BinaryExpr will propagate ErrorType if one of the operands has ErrorType") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto operand = std::make_unique<UnaryExpr>(
		UnaryOpKind::Not,
		std::make_unique<StringLit>(u8"🐄 Mooh 🐄")
	);
	auto expr = std::make_unique<BinaryExpr>(
		BinaryOpKind::Addition,
		std::make_unique<StringLit>(u8"67"),
		std::move(operand)
	);

	// Act
	tc.dispatch(*expr);

	// Assert
	// Still only one error, second binary expr should only propagate the error
	CHECK(ctx.getErrors().size() == 1);
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: HeapAlloc will return a pointer to allocated type") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto allocType = std::make_shared<Typename>(u8"i32");
	auto expr = std::make_unique<HeapAlloc>(allocType);

	// Act
	tc.dispatch(*expr);

	// Assert
	CHECK(ctx.getErrors().empty());
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(*type == PointerType(allocType));
	CHECK(expr->valueCategory == ValueCategory::RValue);
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