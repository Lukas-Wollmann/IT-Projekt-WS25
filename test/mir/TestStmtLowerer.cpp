#include "Doctest.h"
#include "core/PrintUtil.h"
#include "lexer/Lexer.h"
#include "mir/Lowerer.h"
#include "mir_old/MIRGraphviz.h"
#include "parser/Parser.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"

TEST_CASE("StmtLowerer: Sandbox") {
	// Arrange
	U8String source = u8"func a() { b: i32 = 10; }";
	ErrorHandler errorHandler(u8"test.ocn", source);

	const auto tokens = lex::Lexer::tokenize(source, errorHandler);
	const auto module = prs::Parser::parse(tokens, errorHandler, u8"test-module");
	const auto mirModule = mir::Lowerer().lowerModule(*module);

	util::print("{}", mir::MIRGraphviz::generateDOT(mirModule));
}