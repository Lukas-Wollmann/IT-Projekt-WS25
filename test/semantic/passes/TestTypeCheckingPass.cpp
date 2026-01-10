#include "Doctest.h"
#include "ast/Printer.h"
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

TEST_CASE("TypeCheckingPass: UnitLit type will be infered as UnitType") {
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
	CHECK(unitLit->valueCategory.value() == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: UnaryExpr will infer correct type if operator exists") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr = std::make_unique<UnaryExpr>(UnaryOpKind::Negative, std::make_unique<IntLit>(42));

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

TEST_CASE("TypeCheckingPass: UnaryExpr will error if operator doesn't exist") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr = std::make_unique<UnaryExpr>(UnaryOpKind::LogicalNot,
											std::make_unique<StringLit>(u8"2"));

	// Act
	tc.dispatch(*expr);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] == u8"Cannot use unary operator '!' on a value of type 'string'.");
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: UnaryExpr will fail silently if operand has ErrorType") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto errorOperand = std::make_unique<UnaryExpr>(UnaryOpKind::LogicalNot,
													std::make_unique<StringLit>(u8"67"));

	auto expr = std::make_unique<UnaryExpr>(UnaryOpKind::Negative, std::move(errorOperand));

	// Act
	tc.dispatch(*expr);

	// Assert
	// Still only one error, second unary expr should only propagate the error
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] == u8"Cannot use unary operator '!' on a value of type 'string'.");
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: BinaryExpr will infer correct type if operator exists") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr = std::make_unique<BinaryExpr>(BinaryOpKind::Equality, std::make_unique<IntLit>(42),
											 std::make_unique<IntLit>(58));

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

TEST_CASE("TypeCheckingPass: BinaryExpr will error if operator doesn't exist") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto expr =
			std::make_unique<BinaryExpr>(BinaryOpKind::Modulo, std::make_unique<StringLit>(u8"🦒"),
										 std::make_unique<StringLit>(u8"67"));

	// Act
	tc.dispatch(*expr);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] ==
		  u8"Cannot use binary operator '%' on values of type 'string' and 'string'.");
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: BinaryExpr will fail silently if one of the operands has ErrorType") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto errorOperand = std::make_unique<UnaryExpr>(UnaryOpKind::LogicalNot,
													std::make_unique<StringLit>(u8"🐄 Mooh 🐄"));
	auto expr = std::make_unique<BinaryExpr>(BinaryOpKind::Addition,
											 std::make_unique<StringLit>(u8"67"),
											 std::move(errorOperand));

	// Act
	tc.dispatch(*expr);

	// Assert
	// Still only one error, second binary expr should only propagate the error
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] == u8"Cannot use unary operator '!' on a value of type 'string'.");
	CHECK(expr->inferredType.has_value());
	CHECK(expr->valueCategory.has_value());

	auto type = expr->inferredType.value();
	CHECK(type->isTypeKind(TypeKind::Error));
	CHECK(expr->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: Simple Assignment will be infered as UnitType") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	auto var = std::make_unique<VarDef>(u8"i", std::make_shared<Typename>(u8"i32"),
										std::make_unique<IntLit>(67));
	auto assignment =
			std::make_unique<Assignment>(AssignmentKind::Simple, std::make_unique<VarRef>(u8"i"),
										 std::make_unique<IntLit>(187));

	// Add a variable to the symbol table
	tc.dispatch(*var);

	// Act
	tc.dispatch(*assignment);

	// Assert
	CHECK(ctx.getErrors().size() == 0);
	CHECK(assignment->inferredType.has_value());
	CHECK(assignment->valueCategory.has_value());

	auto type = assignment->inferredType.value();
	CHECK(*type == UnitType());
	CHECK(assignment->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: Compound Assignment will be infered as UnitType") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	auto var = std::make_unique<VarDef>(u8"i", std::make_shared<Typename>(u8"i32"),
										std::make_unique<IntLit>(67));
	auto assignment =
			std::make_unique<Assignment>(AssignmentKind::Addition, std::make_unique<VarRef>(u8"i"),
										 std::make_unique<IntLit>(187));

	// Add a variable to the symbol table
	tc.dispatch(*var);

	// Act
	tc.dispatch(*assignment);

	// Assert
	CHECK(ctx.getErrors().size() == 0);
	CHECK(assignment->inferredType.has_value());
	CHECK(assignment->valueCategory.has_value());

	auto type = assignment->inferredType.value();
	CHECK(*type == UnitType());
	CHECK(assignment->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: Assignment to rvalue will add an error") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	auto assignment =
			std::make_unique<Assignment>(AssignmentKind::Simple, std::make_unique<IntLit>(1),
										 std::make_unique<IntLit>(2));

	// Act
	tc.dispatch(*assignment);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] == u8"Left side of an assignment needs to be an l-value.");
	CHECK(assignment->inferredType.has_value());
	CHECK(assignment->valueCategory.has_value());

	auto type = assignment->inferredType.value();
	CHECK(*type == UnitType());
	CHECK(assignment->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: Assignment will fail silently if one of the sides has ErrorType") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	auto errorExpr = std::make_unique<UnaryExpr>(UnaryOpKind::LogicalNot,
												 std::make_unique<StringLit>(u8"test"));
	auto assignment = std::make_unique<Assignment>(AssignmentKind::Simple, std::move(errorExpr),
												   std::make_unique<IntLit>(2));

	// Act
	tc.dispatch(*assignment);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] == u8"Cannot use unary operator '!' on a value of type 'string'.");
	CHECK(assignment->inferredType.has_value());
	CHECK(assignment->valueCategory.has_value());

	auto type = assignment->inferredType.value();
	CHECK(*type == UnitType());
	CHECK(assignment->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: Illegal Compound Assignment will add an error") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	auto var = std::make_unique<VarDef>(u8"i", std::make_shared<Typename>(u8"string"),
										std::make_unique<StringLit>(u8"foo"));
	auto assignment = std::make_unique<Assignment>(AssignmentKind::Multiplication,
												   std::make_unique<VarRef>(u8"i"),
												   std::make_unique<StringLit>(u8"bar"));

	// Add a variable to the symbol table
	tc.dispatch(*var);

	// Act
	tc.dispatch(*assignment);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] ==
		  u8"Cannot use binary operator '*' on values of type 'string' and 'string'.");
	CHECK(assignment->inferredType.has_value());
	CHECK(assignment->valueCategory.has_value());

	auto type = assignment->inferredType.value();
	CHECK(*type == UnitType());
	CHECK(assignment->valueCategory == ValueCategory::RValue);
}

TEST_CASE("TypeCheckingPass: Compound Assignment with wrong return type will add an error") {
	// No case like this exists yet.
}

TEST_CASE("TypeCheckingPass: Simple Assignment with wrong type will add an error") {
	// Arrange
	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);

	auto var = std::make_unique<VarDef>(u8"i", std::make_shared<Typename>(u8"string"),
										std::make_unique<StringLit>(u8"foo"));
	auto assignment =
			std::make_unique<Assignment>(AssignmentKind::Simple, std::make_unique<VarRef>(u8"i"),
										 std::make_unique<IntLit>(67));

	// Add a variable to the symbol table
	tc.dispatch(*var);

	// Act
	tc.dispatch(*assignment);

	// Assert
	CHECK(ctx.getErrors().size() == 1);
	CHECK(ctx.getErrors()[0] == u8"Expected value of type 'string', got type 'i32' instead.");
	CHECK(assignment->inferredType.has_value());
	CHECK(assignment->valueCategory.has_value());

	auto type = assignment->inferredType.value();
	CHECK(*type == UnitType());
	CHECK(assignment->valueCategory == ValueCategory::RValue);
}