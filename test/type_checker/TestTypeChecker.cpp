#include "Doctest.h"
#include "ast/PrintVisitor.h"
#include "type_checker/passes/ExplorationPass.h"
#include "type_checker/passes/TypeCheckingPass.h"

using namespace ast;

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
	CHECK(intLit->inferredType.value()->getKind() == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.getPrimitive() == PrimitiveTypeKind::I32);
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
	CHECK(type.getKind() == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.getPrimitive() == PrimitiveTypeKind::F32);
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
	CHECK(type.getKind() == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.getPrimitive() == PrimitiveTypeKind::Char);
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
	CHECK(type.getKind() == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.getPrimitive() == PrimitiveTypeKind::Bool);
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
	CHECK(type.getKind() == TypeKind::Primitive);

	auto &primitive = static_cast<const PrimitiveType &>(type);
	CHECK(primitive.getPrimitive() == PrimitiveTypeKind::String);
}

TEST_CASE("TypeChecker: ReturnStmt works if return expression has correct type") {
	Vec<Box<Stmt>> stmts;
	stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<FloatLit>(10.0f)));

	auto funcDecl =
			std::make_unique<FuncDecl>("testFunction", Vec<Param>{},
									   std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32),
									   std::make_unique<BlockStmt>(std::move(stmts)));

	TypeCheckerContext ctx;
	TypeCheckingPass tc(ctx);
	tc.dispatch(*funcDecl);

	for (const auto &err : ctx.getErrors())
		std::cout << err << std::endl;
}

TEST_CASE("TypeChecker: Sandbox") {
	Vec<Param> params;
	params.push_back({"a", std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32)});
	params.push_back({"b", std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32)});

	Vec<Box<Expr>> args;
	args.push_back(std::make_unique<VarRef>("a"));
	args.push_back(std::make_unique<BinaryExpr>(BinaryOpKind::Subtraction,
												std::make_unique<VarRef>("b"),
												std::make_unique<IntLit>(1)));
	Vec<Box<Stmt>> body;
	body.push_back(
			std::make_unique<VarDef>("x", std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32),
									 std::make_unique<FuncCall>(std::make_unique<VarRef>("add"),
																std::move(args))));

	Vec<Box<Expr>> arrElems;
	arrElems.push_back(std::make_unique<FloatLit>(1.0f));
	arrElems.push_back(std::make_unique<FloatLit>(2.0f));
	arrElems.push_back(std::make_unique<FloatLit>(3.0f));

	body.push_back(std::make_unique<VarDef>(
			"y",
			std::make_unique<ArrayType>(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32), 3),
			std::make_unique<ArrayExpr>(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32),
										std::move(arrElems))));
	body.push_back(std::make_unique<WhileStmt>(std::make_unique<BoolLit>(false),
											   std::make_unique<BlockStmt>(Vec<Box<Stmt>>{})));
	body.push_back(std::make_unique<ReturnStmt>(std::make_unique<VarRef>("x")));

	Vec<Box<FuncDecl>> decls;
	decls.push_back(
			std::make_unique<FuncDecl>("add", std::move(params),
									   std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32),
									   std::make_unique<BlockStmt>(std::move(body))));

	auto module = std::make_unique<Module>("test_module", std::move(decls));

	/*
		The AST above is basically:

		A file called test_module:
		```
		func add(a: i32, b: i32) -> i32 {
			x: i32 = add(a, b - 1);
			y: [3]f32 = { 1.0f, 2.0f, 3.0f };

			while (false) {}

			return x;
		}
		```

	*/

	std::cout << *module << std::endl;

	TypeCheckerContext ctx;

	ExplorationPass ep(ctx);
	ep.dispatch(*module);

	std::cout << ctx.getGlobalNamespace();

	TypeCheckingPass tc(ctx);
	tc.dispatch(*module);

	for (auto e : ctx.getErrors())
		std::cout << e << std::endl;
}