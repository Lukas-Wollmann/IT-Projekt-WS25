#include "Doctest.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"

using namespace parser;
using namespace lexer;
using namespace lexer;

TEST_CASE("Parser: peek() returns token at the next position") {
	// Arrange
	const Vec<Token> tokens{{TokenType::IntLiteral}, {TokenType::StringLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act
	auto peek = parser.peek();

	// Assert
	CHECK(peek.matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: peek() throws if called at last position") {
	// Arrange
	const Vec<Token> tokens{{TokenType::StringLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act & Assert
	CHECK_THROWS_AS(parser.peek(), std::runtime_error);
}

TEST_CASE("Parser: advance() advances to the next token") {
	// Arrange
	const Vec<Token> tokens{{TokenType::IntLiteral}, {TokenType::StringLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act
	parser.advance();

	// Assert
	CHECK(parser.m_Current->matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: advance() throws if called at the last position") {
	// Arrange
	const Vec<Token> tokens{{TokenType::IntLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act & Assert
	CHECK_THROWS_AS(parser.advance(), std::runtime_error);
}

TEST_CASE("Parser: consume() advances if called with correct token type") {
	// Arrange
	const Vec<Token> tokens{{TokenType::IntLiteral}, {TokenType::StringLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act
	auto before = parser.consume(TokenType::IntLiteral);

	// Assert
	CHECK(before.matches(TokenType::IntLiteral));
	CHECK(parser.m_Current->matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: consume() throws if called with incorrect token type") {
	// Arrange
	const Vec<Token> tokens{{TokenType::IntLiteral}, {TokenType::StringLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act
	parser.consume(TokenType::IntLiteral);

	// Assert
	CHECK(parser.m_Current->matches(TokenType::StringLiteral));
}

TEST_CASE("Parser: consume() throws if called with incorrect lexeme") {
	// Arrange
	const Vec<Token> tokens{{TokenType::Keyword, u8"func"}, {TokenType::StringLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act & Assert
	CHECK_THROWS_AS(parser.consume(TokenType::IntLiteral, u8"if"), ParsingError);
}

TEST_CASE("Parser: consume() throws if called with incorrect token type and lexeme") {
	// Arrange
	const Vec<Token> tokens{{TokenType::Keyword, u8"func"}, {TokenType::StringLiteral}};
	Parser parser{tokens, u8"test-module"};

	// Act & Assert
	CHECK_THROWS_AS(parser.consume(TokenType::Separator, u8"if"), ParsingError);
}

TEST_CASE("Parser: advanceToNext() finds next occurance of token") {
	// Arrange
	const Vec<Token> tokens{{TokenType::StringLiteral},
							{TokenType::BoolLiteral},
							{TokenType::Keyword, u8"func"}};
	Parser parser{tokens, u8"test-module"};

	// Act
	parser.advanceToNext(TokenType::Keyword, u8"func");

	// Assert
	CHECK(parser.m_Current->matches(TokenType::Keyword, u8"func"));
}

TEST_CASE("Parser: Sandbox") {
	U8String src = u8"func a() { b<a;}";

	auto tokens = Lexer::tokenize(src, u8"");
	auto module = Parser::parse(tokens, u8"test-module");

	std::cout << *module.first << std::endl;

	for (auto err : module.second)
		std::cout << err << std::endl;
}