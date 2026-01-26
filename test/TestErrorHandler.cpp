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
	lexer::Token badToken(lexer::TokenType::Illegal, U8String("'abc'"), {1, 8, 8},
						  lexer::TokenError::MultipleCharsInCharLiteral);

	handler.addTokenError(badToken, U8String("character literal may only contain one codepoint"));

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

TEST_CASE("ErrorHandler - Empty Source Code") {
	U8String sourceCode = U8String("");
	ErrorHandler handler(U8String("empty.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("error in empty file"), {1, 0, 0}, 1);

	CHECK(handler.errorCount() == 1);

	std::cout << "\n=== Test: Empty Source Code ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Highlight Length Zero") {
	U8String sourceCode = U8String("int x = 5;");
	ErrorHandler handler(U8String("zero-highlight.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("zero length highlight"), {1, 4, 4}, 0);

	std::cout << "\n=== Test: Highlight Length Zero ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Column Out of Bounds") {
	U8String sourceCode = U8String("short");
	ErrorHandler handler(U8String("bounds.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("column beyond line length"), {1, 999, 0}, 5);

	std::cout << "\n=== Test: Column Out of Bounds ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Multiple Errors Same Line") {
	U8String sourceCode = U8String("int x = 5 + 10 - error;");
	ErrorHandler handler(U8String("same-line.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("first error"), {1, 8, 8}, 1);
	handler.addError(ErrorLevel::WARNING, U8String("second warning"), {1, 17, 17}, 5);
	handler.addError(ErrorLevel::NOTE, U8String("note here"), {1, 0, 0}, 3);

	std::cout << "\n=== Test: Multiple Errors Same Line ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Very Long Line") {
	// Create a line with 500 characters
	std::u8string longLine(500, u8'x');
	U8String sourceCode = U8String(longLine);
	ErrorHandler handler(U8String("long-line.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("error in long line"), {1, 250, 250}, 10);

	std::cout << "\n=== Test: Very Long Line ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - NOTE Level Isolated") {
	U8String sourceCode = U8String("int x = 5;");
	ErrorHandler handler(U8String("note.cpp"), sourceCode);

	handler.addError(ErrorLevel::NOTE, U8String("this is just a note"), {1, 4, 4}, 1);

	CHECK(handler.errorCount() == 0);
	CHECK(handler.warningCount() == 0);
	CHECK(handler.hasError() == false);

	std::cout << "\n=== Test: NOTE Level Isolated ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Mix All Error Levels") {
	U8String sourceCode = U8String("int x = 5;\nint y = 10;\nint z = 15;");
	ErrorHandler handler(U8String("mixed.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR, U8String("critical error"), {1, 4, 4}, 1);
	handler.addError(ErrorLevel::WARNING, U8String("potential issue"), {2, 4, 15}, 1);
	handler.addError(ErrorLevel::NOTE, U8String("informational note"), {3, 4, 26}, 1);
	handler.addError(ErrorLevel::ERROR, U8String("another error"), {3, 8, 30}, 2);

	CHECK(handler.errorCount() == 2);
	CHECK(handler.warningCount() == 1);

	std::cout << "\n=== Test: Mix All Error Levels ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Zero Errors Print") {
	U8String sourceCode = U8String("int x = 5;");
	ErrorHandler handler(U8String("no-errors.cpp"), sourceCode);

	CHECK(handler.errorCount() == 0);
	CHECK(handler.warningCount() == 0);
	CHECK(handler.hasError() == false);

	std::cout << "\n=== Test: Zero Errors Print ===\n";
	handler.printErrors(); // Should print nothing
	std::cout << "(Should see no error output above)\n";
}

TEST_CASE("ErrorHandler - Special Characters in Message") {
	U8String sourceCode = U8String("int x = 5;");
	ErrorHandler handler(U8String("special.cpp"), sourceCode);

	handler.addError(ErrorLevel::ERROR,
					 U8String("Expected 'expression' after \"operator\" with <brackets>"),
					 {1, 8, 8}, 1);

	std::cout << "\n=== Test: Special Characters in Message ===\n";
	handler.printErrors();
}
