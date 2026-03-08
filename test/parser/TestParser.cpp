#include "Doctest.h"
#include "ast/Printer.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"

using namespace prs;
using namespace lex;

TEST_CASE("Parser: peek() returns token at the next position") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::IntLiteral), Token(TokenType::StringLiteral)};
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto peek = parser.peek();

	// Assert
	CHECK(peek.matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: peek() throws if called at last position") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::StringLiteral)};
	Parser parser(tokens, err, u8"test-module");

	// Act & Assert
	CHECK_THROWS_AS(const auto _ = parser.peek(), std::runtime_error);
}

TEST_CASE("Parser: advance() advances to the next token") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::IntLiteral), Token(TokenType::StringLiteral)};
	Parser parser(tokens, err, u8"test-module");

	// Act
	parser.advance();

	// Assert
	CHECK(parser.m_Current->matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: advance() throws if called at the last position") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::IntLiteral)};
	Parser parser(tokens, err, u8"test-module");

	// Act & Assert
	CHECK_THROWS_AS(parser.advance(), std::runtime_error);
}

TEST_CASE("Parser: consume() advances if called with correct token type") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::IntLiteral), Token(TokenType::StringLiteral)};
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto before = parser.consume(TokenType::IntLiteral);

	// Assert
	CHECK(before.matches(TokenType::IntLiteral));
	CHECK(parser.m_Current->matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: consume() throws if called with incorrect token type") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::IntLiteral), Token(TokenType::StringLiteral)};
	Parser parser(tokens, err, u8"test-module");

	// Act
	parser.consume(TokenType::IntLiteral);

	// Assert
	CHECK(parser.m_Current->matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: consume() throws if called with incorrect lexeme") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::Keyword, u8"func"),
							   Token(TokenType::StringLiteral)};
	Parser parser{tokens, err, u8"test-module"};

	// Act & Assert
	CHECK_THROWS_AS(parser.consume(TokenType::IntLiteral, u8"if"), ParsingError);
}

TEST_CASE("Parser: consume() throws if called with incorrect token type and lexeme") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::Keyword, u8"func"),
							   Token(TokenType::StringLiteral)};
	Parser parser(tokens, err, u8"test-module");

	// Act & Assert
	CHECK_THROWS_AS(parser.consume(TokenType::Separator, u8"if"), ParsingError);
}

TEST_CASE("Parser: advanceToNext() finds next occurance of token") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	const Vec<Token> tokens = {Token(TokenType::StringLiteral), Token(TokenType::BoolLiteral),
							   Token(TokenType::Keyword, u8"func")};
	Parser parser(tokens, err, u8"test-module");

	// Act
	parser.advanceToNext(TokenType::Keyword, u8"func");

	// Assert
	CHECK(parser.m_Current->matches(TokenType::Keyword, u8"func"));
}

TEST_CASE("Parser: parseType() - Simple typename") {
	// Arrange
	U8String source = u8"i32";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto type = parser.parseType();

	// Assert
	CHECK(type->kind == type::TypeKind::Typename);
}

TEST_CASE("Parser: parseType() - Unit type") {
	// Arrange
	U8String source = u8"()";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto type = parser.parseType();

	// Assert
	CHECK(type->kind == type::TypeKind::Unit);
}

TEST_CASE("Parser: parseType() - Pointer type") {
	// Arrange
	U8String source = u8"*i32";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto type = parser.parseType();

	// Assert
	CHECK(type->kind == type::TypeKind::Pointer);
}

TEST_CASE("Parser: parsePrimaryExpr() - Integer literal") {
	// Arrange
	U8String source = u8"42";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::IntLit);
	auto intLit = dynamic_cast<ast::IntLit *>(expr.get());
	CHECK(intLit->value == 42);
}

TEST_CASE("Parser: parsePrimaryExpr() - Boolean literal true") {
	// Arrange
	U8String source = u8"true";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BoolLit);
	auto boolLit = dynamic_cast<ast::BoolLit *>(expr.get());
	CHECK(boolLit->value == true);
}

TEST_CASE("Parser: parsePrimaryExpr() - Boolean literal false") {
	// Arrange
	U8String source = u8"false";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BoolLit);
	auto boolLit = dynamic_cast<ast::BoolLit *>(expr.get());
	CHECK(boolLit->value == false);
}

TEST_CASE("Parser: parsePrimaryExpr() - Character literal") {
	// Arrange
	U8String source = u8"'x'";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::CharLit);
	auto charLit = dynamic_cast<ast::CharLit *>(expr.get());
	CHECK(charLit->value == U'x');
}

TEST_CASE("Parser: parsePrimaryExpr() - Variable reference") {
	// Arrange
	U8String source = u8"myVar";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::VarRef);
	auto varRef = dynamic_cast<ast::VarRef *>(expr.get());
	CHECK(varRef->ident == u8"myVar");
}

TEST_CASE("Parser: parsePrimaryExpr() - Unit literal") {
	// Arrange
	U8String source = u8"()";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::UnitLit);
}

TEST_CASE("Parser: parsePrimaryExpr() - Parenthesized expression") {
	// Arrange
	U8String source = u8"(42)";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::IntLit);
}

TEST_CASE("Parser: parsePrimaryExpr() - Heap allocation") {
	// Arrange
	U8String source = u8"new i32(5)";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePrimaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::HeapAlloc);
}

TEST_CASE("Parser: parseUnaryExpr() - Negative number") {
	// Arrange
	U8String source = u8"-42";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseUnaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::UnaryExpr);
	auto unary = dynamic_cast<ast::UnaryExpr *>(expr.get());
	CHECK(unary->op == UnaryOpKind::Negative);
}

TEST_CASE("Parser: parseUnaryExpr() - Logical not") {
	// Arrange
	U8String source = u8"!true";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseUnaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::UnaryExpr);
	auto unary = dynamic_cast<ast::UnaryExpr *>(expr.get());
	CHECK(unary->op == UnaryOpKind::LogicalNot);
}

TEST_CASE("Parser: parseUnaryExpr() - Dereference") {
	// Arrange
	U8String source = u8"*ptr";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseUnaryExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::UnaryExpr);
	auto unary = dynamic_cast<ast::UnaryExpr *>(expr.get());
	CHECK(unary->op == UnaryOpKind::Dereference);
}

TEST_CASE("Parser: parseMultiplicativeExpr() - Multiplication") {
	// Arrange
	U8String source = u8"3 * 4";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseMultiplicativeExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::Multiplication);
}

TEST_CASE("Parser: parseMultiplicativeExpr() - Division") {
	// Arrange
	U8String source = u8"10 / 2";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseMultiplicativeExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::Division);
}

TEST_CASE("Parser: parseMultiplicativeExpr() - Modulo") {
	// Arrange
	U8String source = u8"10 % 3";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseMultiplicativeExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::Modulo);
}

TEST_CASE("Parser: parseAdditiveExpr() - Addition") {
	// Arrange
	U8String source = u8"1 + 2";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseAdditiveExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::Addition);
}

TEST_CASE("Parser: parseAdditiveExpr() - Subtraction") {
	// Arrange
	U8String source = u8"5 - 3";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseAdditiveExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::Subtraction);
}

TEST_CASE("Parser: parseRelationalExpr() - Less than") {
	// Arrange
	U8String source = u8"x < 10";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseRelationalExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::LessThan);
}

TEST_CASE("Parser: parseRelationalExpr() - Greater than") {
	// Arrange
	U8String source = u8"x > 5";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseRelationalExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::GreaterThan);
}

TEST_CASE("Parser: parseRelationalExpr() - Less than or equal") {
	// Arrange
	U8String source = u8"x <= 10";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseRelationalExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::LessThanOrEqual);
}

TEST_CASE("Parser: parseRelationalExpr() - Greater than or equal") {
	// Arrange
	U8String source = u8"x >= 5";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseRelationalExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::GreaterThanOrEqual);
}

TEST_CASE("Parser: parseEqualityExpr() - Equality") {
	// Arrange
	U8String source = u8"x == 5";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseEqualityExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::Equality);
}

TEST_CASE("Parser: parseEqualityExpr() - Inequality") {
	// Arrange
	U8String source = u8"x != 0";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseEqualityExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto binary = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(binary->op == BinaryOpKind::Inequality);
}

TEST_CASE("Parser: parseAssignmentExpr() - Simple assignment") {
	// Arrange
	U8String source = u8"x = 5";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseAssignmentExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::Assignment);
	auto assign = dynamic_cast<ast::Assignment *>(expr.get());
	CHECK(assign->assignmentKind == AssignmentKind::Simple);
}

TEST_CASE("Parser: parseAssignmentExpr() - Addition assignment") {
	// Arrange
	U8String source = u8"x += 5";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseAssignmentExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::Assignment);
	auto assign = dynamic_cast<ast::Assignment *>(expr.get());
	CHECK(assign->assignmentKind == AssignmentKind::Addition);
}

TEST_CASE("Parser: parsePostfixExpr() - Function call no args") {
	// Arrange
	U8String source = u8"foo()";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePostfixExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::FuncCall);
	auto call = dynamic_cast<ast::FuncCall *>(expr.get());
	CHECK(call->args.size() == 0);
}

TEST_CASE("Parser: parsePostfixExpr() - Function call with args") {
	// Arrange
	U8String source = u8"add(1, 2)";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parsePostfixExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::FuncCall);
	auto call = dynamic_cast<ast::FuncCall *>(expr.get());
	CHECK(call->args.size() == 2);
}

TEST_CASE("Parser: parseExprList() - Empty list") {
	// Arrange
	U8String source = u8"()";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto exprs = parser.parseExprList();

	// Assert
	CHECK(exprs.size() == 0);
}

TEST_CASE("Parser: parseExprList() - Multiple expressions") {
	// Arrange
	U8String source = u8"(1, 2, 3)";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto exprs = parser.parseExprList();

	// Assert
	CHECK(exprs.size() == 3);
}

TEST_CASE("Parser: parseVarDef() - Variable definition") {
	// Arrange
	U8String source = u8"x: i32 = 10";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto varDef = parser.parseVarDef();

	// Assert
	CHECK(varDef->kind == ast::NodeKind::VarDef);
	CHECK(varDef->ident == u8"x");
}

TEST_CASE("Parser: parseBlockStmt() - Empty block") {
	// Arrange
	U8String source = u8"{}";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto block = parser.parseBlockStmt();

	// Assert
	CHECK(block->kind == ast::NodeKind::BlockStmt);
	CHECK(block->stmts.size() == 0);
}

TEST_CASE("Parser: parseBlockStmt() - Block with statements") {
	// Arrange
	U8String source = u8"{ x = 5; y = 10; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto block = parser.parseBlockStmt();

	// Assert
	CHECK(block->kind == ast::NodeKind::BlockStmt);
	CHECK(block->stmts.size() == 2);
}

TEST_CASE("Parser: parseIfStmt() - If without else") {
	// Arrange
	U8String source = u8"if (x > 0) { y = 1; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto ifStmt = parser.parseIfStmt();

	// Assert
	CHECK(ifStmt->kind == ast::NodeKind::IfStmt);
	CHECK(ifStmt->else_->stmts.size() == 0);
}

TEST_CASE("Parser: parseIfStmt() - If with else") {
	// Arrange
	U8String source = u8"if (x > 0) { y = 1; } else { y = 0; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto ifStmt = parser.parseIfStmt();

	// Assert
	CHECK(ifStmt->kind == ast::NodeKind::IfStmt);
	CHECK(ifStmt->then->stmts.size() == 1);
	CHECK(ifStmt->else_->stmts.size() == 1);
}

TEST_CASE("Parser: parseIfStmt() - If else if chain") {
	// Arrange
	U8String source = u8"if (x > 0) { y = 1; } else if (x < 0) { y = -1; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto ifStmt = parser.parseIfStmt();

	// Assert
	CHECK(ifStmt->kind == ast::NodeKind::IfStmt);
	CHECK(ifStmt->else_->stmts.size() == 1);
	CHECK(ifStmt->else_->stmts[0]->kind == ast::NodeKind::IfStmt);
}

TEST_CASE("Parser: parseWhileStmt() - While loop") {
	// Arrange
	U8String source = u8"while (x < 10) { x = x + 1; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto whileStmt = parser.parseWhileStmt();

	// Assert
	CHECK(whileStmt->kind == ast::NodeKind::WhileStmt);
	CHECK(whileStmt->body->stmts.size() == 1);
}

TEST_CASE("Parser: parseStmt() - Return statement with value") {
	// Arrange
	U8String source = u8"return 42;";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto stmt = parser.parseStmt();

	// Assert
	CHECK(stmt->kind == ast::NodeKind::ReturnStmt);
}

TEST_CASE("Parser: parseStmt() - Return statement without value") {
	// Arrange
	U8String source = u8"return;";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto stmt = parser.parseStmt();

	// Assert
	CHECK(stmt->kind == ast::NodeKind::ReturnStmt);
	auto ret = dynamic_cast<ast::ReturnStmt *>(stmt.get());
	CHECK(ret->expr->kind == ast::NodeKind::UnitLit);
}

TEST_CASE("Parser: parseStmt() - Empty statement") {
	// Arrange
	U8String source = u8";";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto stmt = parser.parseStmt();

	// Assert
	CHECK(stmt->kind == ast::NodeKind::UnitLit);
}

TEST_CASE("Parser: parseParamList() - Empty params") {
	// Arrange
	U8String source = u8"()";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto params = parser.parseParamList();

	// Assert
	CHECK(params.size() == 0);
}

TEST_CASE("Parser: parseParamList() - Single param") {
	// Arrange
	U8String source = u8"(x: i32)";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto params = parser.parseParamList();

	// Assert
	CHECK(params.size() == 1);
	CHECK(params[0].first == u8"x");
}

TEST_CASE("Parser: parseParamList() - Multiple params") {
	// Arrange
	U8String source = u8"(a: i32, b: i32, c: bool)";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto params = parser.parseParamList();

	// Assert
	CHECK(params.size() == 3);
	CHECK(params[0].first == u8"a");
	CHECK(params[1].first == u8"b");
	CHECK(params[2].first == u8"c");
}

TEST_CASE("Parser: parseFuncDecl() - Simple function") {
	// Arrange
	U8String source = u8"func main() { return; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto func = parser.parseFuncDecl();

	// Assert
	CHECK(func->kind == ast::NodeKind::FuncDecl);
	CHECK(func->ident == u8"main");
	CHECK(func->params.size() == 0);
	CHECK(func->returnType->kind == type::TypeKind::Unit);
}

TEST_CASE("Parser: parseFuncDecl() - Function with params and return type") {
	// Arrange
	U8String source = u8"func add(a: i32, b: i32) -> i32 { return a + b; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto func = parser.parseFuncDecl();

	// Assert
	CHECK(func->kind == ast::NodeKind::FuncDecl);
	CHECK(func->ident == u8"add");
	CHECK(func->params.size() == 2);
	CHECK(func->returnType->kind == type::TypeKind::Typename);
}

TEST_CASE("Parser: parseModule() - Empty module") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto module = parser.parseModule();

	// Assert
	CHECK(module->kind == ast::NodeKind::Module);
	CHECK(module->funcs.size() == 0);
}

TEST_CASE("Parser: parseModule() - Module with single function") {
	// Arrange
	U8String source = u8"func main() { return; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto module = parser.parseModule();

	// Assert
	CHECK(module->kind == ast::NodeKind::Module);
	CHECK(module->funcs.size() == 1);
	CHECK(module->funcs[0]->ident == u8"main");
}

TEST_CASE("Parser: parseModule() - Module with multiple functions") {
	// Arrange
	U8String source = u8"func foo() { return; } func bar() { return; }";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto module = parser.parseModule();

	// Assert
	CHECK(module->kind == ast::NodeKind::Module);
	CHECK(module->funcs.size() == 2);
	CHECK(module->funcs[0]->ident == u8"foo");
	CHECK(module->funcs[1]->ident == u8"bar");
}

TEST_CASE("Parser: Complex expression - Operator precedence") {
	// Arrange
	U8String source = u8"2 + 3 * 4";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseExpr();

	// Assert
	// Should parse as: 2 + (3 * 4), not (2 + 3) * 4
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
	auto add = dynamic_cast<ast::BinaryExpr *>(expr.get());
	CHECK(add->op == BinaryOpKind::Addition);
	CHECK(add->right->kind == ast::NodeKind::BinaryExpr);
}

TEST_CASE("Parser: Complex expression - Chained comparisons") {
	// Arrange
	U8String source = u8"a < b == c > d";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::BinaryExpr);
}

TEST_CASE("Parser: Complex expression - Nested function calls") {
	// Arrange
	U8String source = u8"outer(inner(42))";
	ErrorHandler err(u8"", source);
	auto tokens = Lexer::tokenize(source, err);
	Parser parser(tokens, err, u8"test-module");

	// Act
	auto expr = parser.parseExpr();

	// Assert
	CHECK(expr->kind == ast::NodeKind::FuncCall);
	auto outer = dynamic_cast<ast::FuncCall *>(expr.get());
	CHECK(outer->args.size() == 1);
	CHECK(outer->args[0]->kind == ast::NodeKind::FuncCall);
}

TEST_CASE("Parser: getAssignmentKindFromString() - All assignment types") {
	// Assert
	CHECK(Parser::getAssignmentKindFromString(u8"=") == AssignmentKind::Simple);
	CHECK(Parser::getAssignmentKindFromString(u8"+=") == AssignmentKind::Addition);
	CHECK(Parser::getAssignmentKindFromString(u8"-=") == AssignmentKind::Subtraction);
	CHECK(Parser::getAssignmentKindFromString(u8"*=") == AssignmentKind::Multiplication);
	CHECK(Parser::getAssignmentKindFromString(u8"/=") == AssignmentKind::Division);
	CHECK(Parser::getAssignmentKindFromString(u8"%=") == AssignmentKind::Modulo);
}