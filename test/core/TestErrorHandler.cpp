#include "Doctest.h"
#include "core/ErrorHandler.h"
#include "lexer/Token.h"

TEST_CASE("ErrorHandler - Basic Error") {
	U8String sourceCode = U8String("x:i32 = 5;\ny:i32 = +;\nreturn x;");
	ErrorHandler handler(U8String("test.ocn"), sourceCode);

	// Add an error at line 2, column 8
	handler.addError(U8String("expected expression after '+'"), { 2, 9, 18, 1 }, ErrorLevel::ERROR);

	CHECK(handler.hasError() == true);
	CHECK(handler.errorCount() == 1);
	CHECK(handler.warningCount() == 0);

	std::cout << "\n=== Test: Basic Error ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Multiple Errors and Warnings") {
	U8String sourceCode = U8String("x:i32;\ny: i32 =x +;\nz:i32 = 5");
	ErrorHandler handler(U8String("multi.ocn"), sourceCode);

	handler.addError(U8String("expected expression"), { 2, 12, 18, 3 }, ErrorLevel::ERROR);

	handler.addError(U8String("missing semicolon"), { 3, 10, 28, 1 }, ErrorLevel::WARNING);

	CHECK(handler.errorCount() == 1);
	CHECK(handler.warningCount() == 1);

	std::cout << "\n=== Test: Multiple Errors ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Token Error") {
	U8String sourceCode = U8String("x:i32 = 'abc';");
	ErrorHandler handler(U8String("token.ocn"), sourceCode);

	// Create a token with an error
	lex::Token badToken(lex::TokenType::Illegal, U8String("\'abc\'"), {1, 10, 8, 5});

	handler.addError(U8String("character literal may only contain one codepoint"), badToken.loc,
					 ErrorLevel::ERROR);

	std::cout << "\n=== Test: Token Error ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - getLineFromSource") {
	U8String sourceCode = U8String("line 1\nline 2\nline 3");
	ErrorHandler handler(U8String("lines.ocn"), sourceCode);

	// Test private method via public interface
	handler.addError(U8String("test line 2"), { 2, 1, 7, 6 }, ErrorLevel::NOTE);

	std::cout << "\n=== Test: Line Extraction ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Last Line Without Newline") {
	U8String sourceCode = U8String("line 1\nline 2"); // No \n at end
	ErrorHandler handler(U8String("no-newline.ocn"), sourceCode);

	handler.addError(U8String("error on last line"), { 2, 4, 10, 4 }, ErrorLevel::ERROR);

	std::cout << "\n=== Test: Last Line Without Newline ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Clear") {
	U8String sourceCode = U8String("int x;");
	ErrorHandler handler(U8String("clear.ocn"), sourceCode);

	handler.addError(U8String("test"), { 1, 1, 0, 1 }, ErrorLevel::ERROR);
	CHECK(handler.hasError() == true);
	CHECK(handler.errorCount() == 1);

	handler.clear();
	CHECK(handler.hasError() == false);
	CHECK(handler.errorCount() == 0);
}

TEST_CASE("ErrorHandler - UTF-8 Support") {
	U8String sourceCode = U8String("äöü:i32 = 5;\n日本語:i32 = 10;");
	ErrorHandler handler(U8String("utf8.ocn"), sourceCode);

	handler.addError(U8String("non-ASCII identifier"), { 1, 1, 4, 3 }, ErrorLevel::WARNING);

	handler.addError(U8String("non-ASCII identifier"), { 2, 1, 17, 3 }, ErrorLevel::WARNING);

	std::cout << "\n=== Test: UTF-8 Support ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Large Line Numbers") {
	U8String sourceCode = U8String("line 1\nline 2\nline 3");
	ErrorHandler handler(U8String("large.ocn"), sourceCode);

	// Add errors at different line numbers to test width calculation
	handler.addError(U8String("error at line 5"), { 5, 1, 0, 1 }, ErrorLevel::ERROR);
	handler.addError(U8String("error at line 1234"), { 1234, 3, 0, 4 }, ErrorLevel::ERROR);

	std::cout << "\n=== Test: Large Line Numbers ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Empty Source Code") {
	U8String sourceCode = U8String("");
	ErrorHandler handler(U8String("empty.ocn"), sourceCode);

	handler.addError(U8String("error in empty file"), { 1, 1, 0, 1 }, ErrorLevel::ERROR);

	CHECK(handler.errorCount() == 1);

	std::cout << "\n=== Test: Empty Source Code ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Highlight Length Zero") {
	U8String sourceCode = U8String("x:i32 = 5;");
	ErrorHandler handler(U8String("zero-highlight.ocn"), sourceCode);

	handler.addError(U8String("zero length highlight"), { 1, 5, 4, 0 }, ErrorLevel::ERROR);

	std::cout << "\n=== Test: Highlight Length Zero ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Column Out of Bounds") {
	U8String sourceCode = U8String("i16  ");
	ErrorHandler handler(U8String("bounds.ocn"), sourceCode);

	handler.addError(U8String("column beyond line length"), { 1, 1000, 0, 5 }, ErrorLevel::ERROR);

	std::cout << "\n=== Test: Column Out of Bounds ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Multiple Errors Same Line") {
	U8String sourceCode = U8String("x:i32 = 5 + 10 - error;");
	ErrorHandler handler(U8String("same-line.ocn"), sourceCode);

	handler.addError(U8String("first error"), { 1, 9, 8, 1 }, ErrorLevel::ERROR);
	handler.addError(U8String("second warning"), { 1, 18, 17, 5 }, ErrorLevel::WARNING);
	handler.addError(U8String("note here"), { 1, 1, 0, 3 }, ErrorLevel::NOTE);

	std::cout << "\n=== Test: Multiple Errors Same Line ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Very Long Line") {
	// Create a line with 500 characters
	std::u8string longLine(500, u8'x');
	U8String sourceCode = U8String(longLine);
	ErrorHandler handler(U8String("long-line.ocn"), sourceCode);

	handler.addError(U8String("error in long line"), { 1, 251, 250, 10 }, ErrorLevel::ERROR);

	std::cout << "\n=== Test: Very Long Line ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - NOTE Level Isolated") {
	U8String sourceCode = U8String("x:i32 = 5;");
	ErrorHandler handler(U8String("note.ocn"), sourceCode);

	handler.addError(U8String("this is just a note"), { 1, 5, 4, 1 }, ErrorLevel::NOTE);

	CHECK(handler.errorCount() == 0);
	CHECK(handler.warningCount() == 0);
	CHECK(handler.hasError() == false);

	std::cout << "\n=== Test: NOTE Level Isolated ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Mix All Error Levels") {
	U8String sourceCode = U8String("x:i32 = 5;\ny:i32 = 10;\nz:i32 = 15;");
	ErrorHandler handler(U8String("mixed.ocn"), sourceCode);

	handler.addError(U8String("critical error"), { 1, 5, 4, 1 }, ErrorLevel::ERROR);
	handler.addError(U8String("potential issue"), { 2, 5, 15, 1 }, ErrorLevel::WARNING);
	handler.addError(U8String("informational note"), { 3, 5, 26, 1 }, ErrorLevel::NOTE);
	handler.addError(U8String("another error"), { 3, 9, 30, 2 }, ErrorLevel::ERROR);

	CHECK(handler.errorCount() == 2);
	CHECK(handler.warningCount() == 1);

	std::cout << "\n=== Test: Mix All Error Levels ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Zero Errors Print") {
	U8String sourceCode = U8String("x:i32 = 5;");
	ErrorHandler handler(U8String("no-errors.ocn"), sourceCode);

	CHECK(handler.errorCount() == 0);
	CHECK(handler.warningCount() == 0);
	CHECK(handler.hasError() == false);

	std::cout << "\n=== Test: Zero Errors Print ===\n";
	handler.printErrors(); // Should print nothing
	std::cout << "(Should see no error output above)\n";
}

TEST_CASE("ErrorHandler - Special Characters in Message") {
	U8String sourceCode = U8String("x:i32 = 5;");
	ErrorHandler handler(U8String("special.ocn"), sourceCode);

	handler.addError(U8String("Expected 'expression' after \"operator\" with <brackets>"),
					 {1, 8, 8, 1}, ErrorLevel::ERROR);

	std::cout << "\n=== Test: Special Characters in Message ===\n";
	handler.printErrors();
}

TEST_CASE("ErrorHandler - Lexical Error (Unterminated String)") {
U8String sourceCode = U8String("str: string = \"Hello World;");
ErrorHandler handler(U8String("lexer_err.ocn"), sourceCode);

handler.addError(U8String("unterminated string literal"), { 1, 15, 14, 12 }, ErrorLevel::ERROR);

CHECK(handler.hasError() == true);
std::cout << "\n=== Test: Lexical Error ===\n";
handler.printErrors();
}

TEST_CASE("ErrorHandler - Syntax Error (Missing Parenthesis)") {
U8String sourceCode = U8String("if x > 5 {\n    return 1;\n}");
ErrorHandler handler(U8String("parser_err.ocn"), sourceCode);

handler.addError(U8String("expected '(' after 'if'"), { 1, 4, 3, 1 }, ErrorLevel::ERROR);

CHECK(handler.hasError() == true);
std::cout << "\n=== Test: Syntax Error ===\n";
handler.printErrors();
}

TEST_CASE("ErrorHandler - Semantic Layer (Type Mismatch)") {
U8String sourceCode = U8String("x: i32 = \"I am a string\";\nreturn x;");
ErrorHandler handler(U8String("type_err.ocn"), sourceCode);

handler.addError(U8String("type mismatch: expected 'i32', found 'string'"), { 1, 10, 9, 15 }, ErrorLevel::ERROR);

CHECK(handler.hasError() == true);
std::cout << "\n=== Test: Semantic Layer (Type Mismatch) ===\n";
handler.printErrors();
}

TEST_CASE("ErrorHandler - Semantic Layer (Redeclaration)") {
U8String sourceCode = U8String("x: i32 = 1;\nx: i32 = 2;");
ErrorHandler handler(U8String("redecl_err.ocn"), sourceCode);

handler.addError(U8String("redefinition of variable 'x'"), { 2, 1, 12, 1 }, ErrorLevel::ERROR);
handler.addError(U8String("previous definition was here"), { 1, 1, 0, 1 }, ErrorLevel::NOTE);

CHECK(handler.errorCount() == 1); // Only 1 ERROR, 1 NOTE
CHECK(handler.hasError() == true);
std::cout << "\n=== Test: Semantic Layer (Redeclaration) ===\n";
handler.printErrors();
}
