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