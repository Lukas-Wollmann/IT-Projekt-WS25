#include "Doctest.h"
#include "hir/LoweringContext.h"

TEST_CASE("") {
	hir::Function fn;
	hir::Block block;
	hir::LoweringContext ctx(fn, block);

	auto intType = std::make_shared<type::Typename>(u8"i32");
	auto left = std::make_unique<ast::IntLit>(42);
	left->infer(intType, ast::ValueCategory::RValue);

	auto value = std::make_unique<ast::IntLit>(10);
	value->infer(intType, ast::ValueCategory::RValue);

	auto right = std::make_unique<ast::HeapAlloc>(std::move(value));
	right->infer(std::make_shared<type::PointerType>(intType), ast::ValueCategory::RValue);

	ctx.lowerExpr(*right);

	for (auto &stmt : block.stmts) {
		stmt->toString(std::cout);
		std::cout << std::endl;
	}
}