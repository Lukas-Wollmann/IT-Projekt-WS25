#include "Doctest.h"
#include "ErrorHandler.h"
#include "lexer/Token.h"

TEST_CASE("ErrorHandler - Basic Error") {
	U8String sourceCode = U8String("int x = 5;\nint y = +;\nreturn x;");
	ErrorHandler handler(U8String("test.cpp"), sourceCode);

	// Add an error at line 2, column 8
	handler.addError(ErrorLevel::ERROR, U8String("expected expression after '+'"),
					 {2, 8, 18}, // line 2, column 8, index 18
					 1);

	CHECK(handler.hasError() == true);
	CHECK(handler.errorCount() == 1);
	CHECK(handler.warningCount() == 0);

	std::cout << "\n=== Test: Basic Error ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Multiple Errors and Warnings") {
	U8String sourceCode = U8String("int x;\nint y = x +;\nint z = 5");
	ErrorHandler handler(U8String("multi.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("expected expression"), {2, 11, 18}, 3);

	handler.addError(ErrorLevel::WARNING, U8String("missing semicolon"), {3, 9, 28}, 1);

	CHECK(handler.errorCount() == 1);
	CHECK(handler.warningCount() == 1);

	std::cout << "\n=== Test: Multiple Errors ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Token Error") {
	U8String sourceCode = U8String("int x = 'abc';");
	ErrorHandler handler(U8String("token.cpp"), sourceCode);

	// Create a token with an error
	Token badToken(TokenType::ILLEGAL, U8String("'abc'"), {1, 8, 8},
				   ErrorTypeToken::MULTIPLE_CHAR_IN_CHAR_LITERAL);

	handler.addTokenError(badToken, U8String("character literal may only contain one codepoint"));

	CHECK(handler.hasError() == true);
	CHECK(handler.errorCount() == 1);

	std::cout << "\n=== Test: Token Error ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - getLineFromSource") {
	U8String sourceCode = U8String("line 1\nline 2\nline 3");
	ErrorHandler handler(U8String("lines.cpp"), sourceCode);

	// Test private method via public interface
	handler.addError(ErrorLevel::NOTE, U8String("test line 2"), {2, 0, 7}, 6);

	std::cout << "\n=== Test: Line Extraction ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Last Line Without Newline") {
	U8String sourceCode = U8String("line 1\nline 2"); // No \n at end
	ErrorHandler handler(U8String("no-newline.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("error on last line"), {2, 3, 10}, 4);

	std::cout << "\n=== Test: Last Line Without Newline ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Clear") {
	U8String sourceCode = U8String("int x;");
	ErrorHandler handler(U8String("clear.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("test"), {1, 0, 0}, 1);
	CHECK(handler.hasError() == true);
	CHECK(handler.errorCount() == 1);

	handler.clear();
	CHECK(handler.hasError() == false);
	CHECK(handler.errorCount() == 0);
}

TEST_CASE("ErrorHandler - UTF-8 Support") {
	U8String sourceCode = U8String("int äöü = 5;\nint 日本語 = 10;");
	ErrorHandler handler(U8String("utf8.cpp"), sourceCode);

	handler.addError(ErrorLevel::WARNING, U8String("non-ASCII identifier"), {1, 4, 4}, 3);

	handler.addError(ErrorLevel::WARNING, U8String("non-ASCII identifier"), {2, 4, 17}, 3);

	std::cout << "\n=== Test: UTF-8 Support ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Large Line Numbers") {
	U8String sourceCode = U8String("line 1\nline 2\nline 3");
	ErrorHandler handler(U8String("large.cpp"), sourceCode);

	// Add errors at different line numbers to test width calculation
	handler.addError(ErrorLevel::ERROR, U8String("error at line 5"), {5, 0, 0}, 1);
	handler.addError(ErrorLevel::ERROR, U8String("error at line 1234"), {1234, 2, 0}, 4);

	std::cout << "\n=== Test: Large Line Numbers ===\n";
	handler.printErrors();
}
