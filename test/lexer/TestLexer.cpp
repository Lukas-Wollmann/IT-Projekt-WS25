#include "Doctest.h"
#include "core/U8String.h"
#include "lexer/Lexer.h"

using namespace lex;
using enum TokenType;

TEST_CASE("Lexer: Empty source") {
	// Arrange
	U8String source = u8"";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single identifier") {
	// Arrange
	U8String source = u8"hallo";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Identifier, u8"hallo"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single keyword") {
	// Arrange
	U8String source = u8"if";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Keyword, u8"if"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single bool literal") {
	// Arrange
	U8String source = u8"true";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(BoolLiteral, u8"true"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single integer literal") {
	// Arrange
	U8String source = u8"67";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(IntLiteral, u8"67"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single string literal") {
	// Arrange
	U8String source = u8"\"Hall\\o\\n\\\\\"";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(StringLiteral, u8"Hallo\n\\"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single separator") {
	// Arrange
	U8String source = u8"i32,bool";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Identifier, u8"i32"), Token(Separator, u8","),
						   Token(Identifier, u8"bool"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single line comment") {
	// Arrange
	U8String source = u8"// Test 1\nabc\n//Test 2 ";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Identifier, u8"abc"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Multi line comment") {
	// Arrange
	U8String source = u8"/* Test 1\n Test 2 */ abc";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Identifier, u8"abc"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Multi line comment unterminated") {
	// Arrange
	U8String source = u8"/* Test 1\n Test 2";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Illegal, u8" Test 1\n Test 2"), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Illegal identifier") {
	// Arrange
	U8String source = u8"hallo ß";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Identifier, u8"hallo"), Token(Illegal, u8"ß"),
						   Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

TEST_CASE("Lexer: Single operator") {
	// Arrange
	U8String source = u8"<<=";
	ErrorHandler err(u8"", source);
	Vec<Token> expected = {Token(Operator, u8"<<="), Token(EndOfFile, u8"")};

	// Act
	auto tokens = Lexer::tokenize(source, err);

	// Assert
	CHECK(tokens.size() == expected.size());
	CHECK(tokens == expected);
}

// LexNumber tests
TEST_CASE("LexNumber: simple integer") {
    // Arrange
    U8String source = u8"12345";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(IntLiteral, u8"12345"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexNumber: integer with trailing characters") {
    // Arrange
    U8String source = u8"6789abc";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(IntLiteral, u8"6789"),
                           Token(Identifier, u8"abc"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexNumber: multiple integers separated by spaces") {
    // Arrange
    U8String source = u8"42  1001\t256\n512";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(IntLiteral, u8"42"),
                           Token(IntLiteral, u8"1001"),
                           Token(IntLiteral, u8"256"),
                           Token(IntLiteral, u8"512"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// LexString tests
TEST_CASE("LexString: simple string literal") {
    // Arrange
    U8String source = u8"\"Hello, World!\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"Hello, World!"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: string with escaped characters") {
    // Arrange
    U8String source = u8"\"Line1\\nLine2\\tTabbed\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"Line1\\nLine2\\tTabbed"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: unterminated string literal") {
    // Arrange
    U8String source = u8"\"Unterminated string";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"Unterminated string"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: empty string literal") {
    // Arrange
    U8String source = u8"\"\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8""), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: string with unicode characters") {
    // Arrange
    U8String source = u8"\"Unicode: \U0001F600 \U0001F603\""; // Grinning face emojis
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"Unicode: \U0001F600 \U0001F603"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: string with internal quotes") {
    // Arrange
    U8String source = u8"\"She said, \\\"Hello!\\\"\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"She said, \\\"Hello!\\\""), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: string with escaped backslash") {
    // Arrange
    U8String source = u8"\"This is a backslash: \\\\\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"This is a backslash: \\\\"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: string with various escaped characters") {
    // Arrange
    U8String source = u8"\"Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: string with only escaped characters") {
    // Arrange
    U8String source = u8"\"\\n\\t\\r\\\\\\'\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"\\n\\t\\r\\\\\\'"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: string with spaces and tabs") {
    // Arrange
    U8String source = u8"\"   Leading and trailing spaces   \\t\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"   Leading and trailing spaces   \\t"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexString: multiple string literals separated by spaces") {
    // Arrange
    U8String source = u8"\"First String\"   \"Second String\"\t\"Third String\"\n\"Fourth String\"";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(StringLiteral, u8"First String"),
                           Token(StringLiteral, u8"Second String"),
                           Token(StringLiteral, u8"Third String"),
                           Token(StringLiteral, u8"Fourth String"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// LexChar tests
TEST_CASE("LexChar: simple char literal") {
    // Arrange
    U8String source = u8"'a'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"a"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: escaped char literal") {
    // Arrange
    U8String source = u8"'\\n'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"\n"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: unterminated char literal") {
    // Arrange
    U8String source = u8"'b";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"b"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: empty char literal") {
    // Arrange
    U8String source = u8"''";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8""), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with multiple characters") {
    // Arrange
    U8String source = u8"'ab'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"ab"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with unicode character") {
    // Arrange
    U8String source = u8"'\U0001F600'"; // Grinning face emoji
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"\U0001F600"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with escaped single quote") {
    // Arrange
    U8String source = u8"'\\''";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"'"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: multiple char literals separated by spaces") {
    // Arrange
    U8String source = u8"'x' 'y' 'z'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"x"),
                           Token(CharLiteral, u8"y"),
                           Token(CharLiteral, u8"z"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: multiple chars in multiple lines") {
    // Arrange
    U8String source = u8"'a'\n'b'\n'c'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"a"),
                           Token(CharLiteral, u8"b"),
                           Token(CharLiteral, u8"c"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with escaped backslash") {
    // Arrange
    U8String source = u8"'\\\\'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"\\"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with space character") {
    // Arrange
    U8String source = u8"' '";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8" "), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: whole sentence in char literals") {
    // Arrange
    U8String source = u8"'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' '!'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"H"), Token(CharLiteral, u8"e"), Token(CharLiteral, u8"l"),
                           Token(CharLiteral, u8"l"), Token(CharLiteral, u8"o"), Token(CharLiteral, u8","),
                           Token(CharLiteral, u8" "), Token(CharLiteral, u8"W"), Token(CharLiteral, u8"o"),
                           Token(CharLiteral, u8"r"), Token(CharLiteral, u8"l"), Token(CharLiteral, u8"d"),
                           Token(CharLiteral, u8"!"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: whole sentence in one char literal") {
    // Arrange
    U8String source = u8"'Hello, World!'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"Hello, World!"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: illegal escape sequence in char literal") {
    // Arrange
    U8String source = u8"'\\x'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"\\x"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: missing closing quote in char literal and other tokens after") {
    // Arrange
    U8String source = u8"'a + b";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"a + b"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: missing opening quote in char literal") {
    // Arrange
    U8String source = u8"a'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"a"),
                           Token(Illegal, u8""),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: missing both quotes in char literal") {
    // Arrange
    U8String source = u8"a";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"a"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: missing closing quote and char literal in next line") {
    // Arrange
    U8String source = u8"'a\n'b'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"a"),
                           Token(CharLiteral, u8"b"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: missing closing quote and char literal in same line") {
    // Arrange
    U8String source = u8"'a 'b'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"a "),
                           Token(Identifier, u8"b"),
                           Token(Illegal, u8""),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with japanese character") {
    // Arrange
    U8String source = u8"'あ'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"あ"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with solo backslash") {
    // Arrange
    U8String source = u8"'\\'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"\\"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with solo backslash and unterminated") {
    // Arrange
    U8String source = u8"'\\";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"\\"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with qotation mark") {
    // Arrange
    U8String source = u8"'\"'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"\""), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexChar: char literal with quotation mark (escaped)") {
    // Arrange
    U8String source = u8"'\\\"'";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(CharLiteral, u8"\""), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// LexSeparator tests
TEST_CASE("LexSeparator: single separator") {
    // Arrange
    U8String source = u8";";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Separator, u8";"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexSeparator: single separator tokens") {
    // Arrange
    U8String source = u8";,(){}[]:";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Separator, u8";"), Token(Separator, u8","), Token(Separator, u8"("),
                           Token(Separator, u8")"), Token(Separator, u8"{"), Token(Separator, u8"}"),
                           Token(Separator, u8"["), Token(Separator, u8"]"), Token(Separator, u8":"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexSeparator: separators with whitespace") {
    // Arrange
    U8String source = u8" ; , ( ) { } [ ] : ";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Separator, u8";"), Token(Separator, u8","), Token(Separator, u8"("),
                           Token(Separator, u8")"), Token(Separator, u8"{"), Token(Separator, u8"}"),
                           Token(Separator, u8"["), Token(Separator, u8"]"), Token(Separator, u8":"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexSeparator: separators across multiple lines") {
    // Arrange
    U8String source = u8";\n,\n(\n)\n{\n}\n[\n]\n:";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Separator, u8";"), Token(Separator, u8","), Token(Separator, u8"("),
                           Token(Separator, u8")"), Token(Separator, u8"{"), Token(Separator, u8"}"),
                           Token(Separator, u8"["), Token(Separator, u8"]"), Token(Separator, u8":"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// LexOperator tests
TEST_CASE("LexOperator: single operator") {
    // Arrange
    U8String source = u8"+";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Operator, u8"+"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexOperator: multiple operators") {
    // Arrange
    U8String source = u8"+ - * / % == != < > <= >= && || ! =";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Operator, u8"+"), Token(Operator, u8"-"), Token(Operator, u8"*"),
                           Token(Operator, u8"/"), Token(Operator, u8"%"), Token(Operator, u8"=="),
                           Token(Operator, u8"!="), Token(Operator, u8"<"), Token(Operator, u8">"),
                           Token(Operator, u8"<="), Token(Operator, u8">="), Token(Operator, u8"&&"),
                           Token(Operator, u8"||"), Token(Operator, u8"!"), Token(Operator, u8"="),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexOperator: operators with whitespace") {
    // Arrange
    U8String source = u8"  +   - \t * \n / % ";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Operator, u8"+"), Token(Operator, u8"-"), Token(Operator, u8"*"),
                           Token(Operator, u8"/"), Token(Operator, u8"%"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexOperator: multi operators without spaces") {
    // Arrange
    U8String source = u8"==!=<><=>=&&||";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Operator, u8"=="), Token(Operator, u8"!="), Token(Operator, u8"<"),
                           Token(Operator, u8">"), Token(Operator, u8"<="), Token(Operator, u8">="),
                           Token(Operator, u8"&&"), Token(Operator, u8"||"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexOperator: lex triple character operators") {
    // Arrange
    U8String source = u8">>= <<=";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Operator, u8">>="), Token(Operator, u8"<<="), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexOperator: lex triple operators without spaces") {
    // Arrange
    U8String source = u8"<<=>>=";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Operator, u8"<<="), Token(Operator, u8">>="), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexOperator: mixed with other tokens") {
    // Arrange
    U8String source = u8"result = a + b * c - d / e;";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"result"), Token(Operator, u8"="), Token(Identifier, u8"a"),
                           Token(Operator, u8"+"), Token(Identifier, u8"b"), Token(Operator, u8"*"),
                           Token(Identifier, u8"c"), Token(Operator, u8"-"), Token(Identifier, u8"d"),
                           Token(Operator, u8"/"), Token(Identifier, u8"e"), Token(Separator, u8";"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexOperator: operators across multiple lines") {
    // Arrange
    U8String source = u8"+\n-\n*\n/\n%";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Operator, u8"+"), Token(Operator, u8"-"), Token(Operator, u8"*"),
                           Token(Operator, u8"/"), Token(Operator, u8"%"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// LexIdentifierOrKeyword tests
TEST_CASE("LexIdentifierOrKeyword: single Identifier") {
    // Arrange
    U8String source = u8"variableName";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"variableName"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexIdentifierOrKeyword: single Keyword") {
    // Arrange
    U8String source = u8"if";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Keyword, u8"if"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexSeparator: mixed with other tokens") {
    // Arrange
    U8String source = u8"x: i32 = 10;\nif (x > 5) {\n  x = x + 1;\n}";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"x"), Token(Separator, u8":"), Token(Identifier, u8"i32"),
                           Token(Operator, u8"="), Token(IntLiteral, u8"10"), Token(Separator, u8";"),
                           Token(Keyword, u8"if"), Token(Separator, u8"("), Token(Identifier, u8"x"),
                           Token(Operator, u8">"), Token(IntLiteral, u8"5"), Token(Separator, u8")"),
                           Token(Separator, u8"{"), Token(Identifier, u8"x"), Token(Operator, u8"="),
                           Token(Identifier, u8"x"), Token(Operator, u8"+"), Token(IntLiteral, u8"1"),
                           Token(Separator, u8";"), Token(Separator, u8"}"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexIdentifierOrKeyword: identifiers with underscores and digits") {
    // Arrange
    U8String source = u8"_var1 var_2 var3_name";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"_var1"), Token(Identifier, u8"var_2"),
                           Token(Identifier, u8"var3_name"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords with whitespace") {
    // Arrange
    U8String source = u8"  var   if \t else \n while ";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"var"), Token(Keyword, u8"if"),
                           Token(Keyword, u8"else"), Token(Keyword, u8"while"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords across multiple lines") {
    // Arrange
    U8String source = u8"var\nif\nelse\nwhile";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"var"), Token(Keyword, u8"if"),
                           Token(Keyword, u8"else"), Token(Keyword, u8"while"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexIdentifierOrKeyword: identifiers starting with digits (illegal)") {
    // Arrange
    U8String source = u8"1variable 2var_name 3_var";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(IntLiteral, u8"1"), Token(Identifier, u8"variable"),
                           Token(IntLiteral, u8"2"), Token(Identifier, u8"var_name"),
                           Token(IntLiteral, u8"3"), Token(Identifier, u8"_var"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// LexComments tests (Note: New Lexer appears to skip comments)
TEST_CASE("LexComments: single-line comment") {
    // Arrange
    U8String source = u8"// This is a single-line comment";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexComments: multi-line comment") {
    // Arrange
    U8String source = u8"/* This is a \n multi-line comment */";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexComments: unclosed multi-line comment") {
    // Arrange
    U8String source = u8"/* This is an unclosed comment";
    ErrorHandler err(u8"", source);
    // Assuming unclosed comments return an Illegal token with the content scanned so far
    Vec<Token> expected = {Token(Illegal, u8" This is an unclosed comment"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// LexIllegal tests
TEST_CASE("LexIllegal: single legal utf8-character") {
    // Arrange
    U8String source = u8"ß";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Illegal, u8"ß"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

// General tests
TEST_CASE("LexGeneral: correct simple token sequence") {
    // Arrange
    U8String source = u8"x: i32 = 42; // variable declaration";
    ErrorHandler err(u8"", source);
    // Comments are filtered out
    Vec<Token> expected = {Token(Identifier, u8"x"), Token(Separator, u8":"), Token(Identifier, u8"i32"),
                           Token(Operator, u8"="), Token(IntLiteral, u8"42"), Token(Separator, u8";"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexGeneral: correct complex token sequence") {
    // Arrange
    U8String source = u8"if (x >= 10) {\n  x = x + 1;\n} else {\n  x = x - 1;\n}";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Keyword, u8"if"), Token(Separator, u8"("), Token(Identifier, u8"x"),
                           Token(Operator, u8">="), Token(IntLiteral, u8"10"), Token(Separator, u8")"),
                           Token(Separator, u8"{"), Token(Identifier, u8"x"), Token(Operator, u8"="),
                           Token(Identifier, u8"x"), Token(Operator, u8"+"), Token(IntLiteral, u8"1"),
                           Token(Separator, u8";"), Token(Separator, u8"}"), Token(Keyword, u8"else"),
                           Token(Separator, u8"{"), Token(Identifier, u8"x"), Token(Operator, u8"="),
                           Token(Identifier, u8"x"), Token(Operator, u8"-"), Token(IntLiteral, u8"1"),
                           Token(Separator, u8";"), Token(Separator, u8"}"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexGeneral: correct complex token sequence featuring all token types") {
    // Arrange
    U8String source = u8"ch: char = 'a'; // char literal\nif (ch == '\\n') {\n  /* multi-line \n "
                      u8"comment */\n  ch = 'b';\n}";
    ErrorHandler err(u8"", source);
    // Comments skipped
    Vec<Token> expected = {Token(Identifier, u8"ch"), Token(Separator, u8":"), Token(Identifier, u8"char"),
                           Token(Operator, u8"="), Token(CharLiteral, u8"a"), Token(Separator, u8";"),
                           Token(Keyword, u8"if"), Token(Separator, u8"("), Token(Identifier, u8"ch"),
                           Token(Operator, u8"=="), Token(CharLiteral, u8"\n"), Token(Separator, u8")"),
                           Token(Separator, u8"{"), Token(Identifier, u8"ch"), Token(Operator, u8"="),
                           Token(CharLiteral, u8"b"), Token(Separator, u8";"), Token(Separator, u8"}"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexGeneral: very long correct token sequence over multiple lines") {
    // Arrange
    U8String source =
            u8"total: i32 = 0;\nwhile (i == 0) {\n  total = total + i;\n}\n// End of loop";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"total"), Token(Separator, u8":"), Token(Identifier, u8"i32"),
                           Token(Operator, u8"="), Token(IntLiteral, u8"0"), Token(Separator, u8";"),
                           Token(Keyword, u8"while"), Token(Separator, u8"("), Token(Identifier, u8"i"),
                           Token(Operator, u8"=="), Token(IntLiteral, u8"0"), Token(Separator, u8")"),
                           Token(Separator, u8"{"), Token(Identifier, u8"total"), Token(Operator, u8"="),
                           Token(Identifier, u8"total"), Token(Operator, u8"+"), Token(Identifier, u8"i"),
                           Token(Separator, u8";"), Token(Separator, u8"}"), Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

TEST_CASE("LexGeneral: incorrect simple token sequence") {
    // Arrange
    U8String source = u8"a: i32 = 10 'a";
    ErrorHandler err(u8"", source);
    Vec<Token> expected = {Token(Identifier, u8"a"), Token(Separator, u8":"), Token(Identifier, u8"i32"),
                           Token(Operator, u8"="), Token(IntLiteral, u8"10"), Token(Illegal, u8"a"),
                           Token(EndOfFile, u8"")};

    // Act
    auto tokens = Lexer::tokenize(source, err);

    // Assert
    CHECK(tokens.size() == expected.size());
    CHECK(tokens == expected);
}

#if 0

// LexNumber tests
TEST_CASE("LexNumber: simple integer") {
	// Arrange
	U8String source = u8"12345";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::IntLiteral, U8String("12345"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexNumber: integer with trailing characters") {
	// Arrange
	U8String source = u8"6789abc";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::IntLiteral, U8String("6789"), {1, 1, 0}),
										 Token(TokenType::Identifier, U8String("abc"), {1, 5, 4})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 3);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexNumber: multiple integers separated by spaces") {
	// Arrange
	U8String source = u8"42  1001\t256\n512";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::IntLiteral, U8String("42"), {1, 1, 0}),
										 Token(TokenType::IntLiteral, U8String("1001"), {1, 5, 4}),
										 Token(TokenType::IntLiteral, U8String("256"), {1, 10, 9}),
										 Token(TokenType::IntLiteral, U8String("512"), {2, 1, 13})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 5);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
	CHECK(tokens[2] == expectedTokens[2]);
	CHECK(tokens[3] == expectedTokens[3]);
}

// LexString tests
TEST_CASE("LexString: simple string literal") {
	// Arrange
	U8String source = u8"\"Hello, World!\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String("Hello, World!"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with escaped characters") {
	// Arrange
	U8String source = u8"\"Line1\\nLine2\\tTabbed\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String("Line1\\nLine2\\tTabbed"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: unterminated string literal") {
	// Arrange
	U8String source = u8"\"Unterminated string";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("Unterminated string"), startLoc,
						TokenError::UnterminatedString);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: empty string literal") {
	// Arrange
	U8String source = u8"\"\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String(""), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with unicode characters") {
	// Arrange
	U8String source = u8"\"Unicode: \U0001F600 \U0001F603\""; // Grinning face emojis
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String("Unicode: \U0001F600 \U0001F603"),
						startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with internal quotes") {
	// Arrange
	U8String source = u8"\"She said, \\\"Hello!\\\"\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String("She said, \\\"Hello!\\\""), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with escaped backslash") {
	// Arrange
	U8String source = u8"\"This is a backslash: \\\\\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String("This is a backslash: \\\\"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with various escaped characters") {
	// Arrange
	U8String source = u8"\"Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral,
						U8String("Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\"),
						startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with only escaped characters") {
	// Arrange
	U8String source = u8"\"\\n\\t\\r\\\\\\'\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String("\\n\\t\\r\\\\\\'"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with spaces and tabs") {
	// Arrange
	U8String source = u8"\"   Leading and trailing spaces   \\t\"";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::StringLiteral, U8String("   Leading and trailing spaces   \\t"),
						startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: multiple string literals separated by spaces") {
	// Arrange
	U8String source = u8"\"First String\"   \"Second String\"\t\"Third String\"\n\"Fourth String\"";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens =
			{Token(TokenType::StringLiteral, U8String("First String"), {1, 1, 0}),
			 Token(TokenType::StringLiteral, U8String("Second String"), {1, 18, 17}),
			 Token(TokenType::StringLiteral, U8String("Third String"), {1, 34, 33}),
			 Token(TokenType::StringLiteral, U8String("Fourth String"), {2, 1, 48})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 5);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
	CHECK(tokens[2] == expectedTokens[2]);
	CHECK(tokens[3] == expectedTokens[3]);
}

// LexChar tests
TEST_CASE("LexChar: simple char literal") {
	// Arrange
	U8String source = u8"'a'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("a"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: escaped char literal") {
	// Arrange
	U8String source = u8"'\\n'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("\n"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: unterminated char literal") {
	// Arrange
	U8String source = u8"'b";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("b"), startLoc,
						TokenError::UnterminatedCharLiteral);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: empty char literal") {
	// Arrange
	U8String source = u8"''";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String(""), startLoc, TokenError::EmptyCharLiteral);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with multiple characters") {
	// Arrange
	U8String source = u8"'ab'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("ab"), startLoc,
						TokenError::MultipleCharsInCharLiteral);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with unicode character") {
	// Arrange
	U8String source = u8"'\U0001F600'"; // Grinning face emoji
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("\U0001F600"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with escaped single quote") {
	// Arrange
	U8String source = u8"'\\''";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("'"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: multiple char literals separated by spaces") {
	// Arrange
	U8String source = u8"'x' 'y' 'z'";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::CharLiteral, U8String("x"), {1, 1, 0}),
										 Token(TokenType::CharLiteral, U8String("y"), {1, 5, 4}),
										 Token(TokenType::CharLiteral, U8String("z"), {1, 9, 8})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 4);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
	CHECK(tokens[2] == expectedTokens[2]);
}

TEST_CASE("LexChar: multiple chars in multiple lines") {
	// Arrange
	U8String source = u8"'a'\n'b'\n'c'";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::CharLiteral, U8String("a"), {1, 1, 0}),
										 Token(TokenType::CharLiteral, U8String("b"), {2, 1, 4}),
										 Token(TokenType::CharLiteral, U8String("c"), {3, 1, 8})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 4);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
	CHECK(tokens[2] == expectedTokens[2]);
}

TEST_CASE("LexChar: char literal with escaped backslash") {
	// Arrange
	U8String source = u8"'\\\\'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("\\"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with space character") {
	// Arrange
	U8String source = u8"' '";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String(" "), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: whole sentence in char literals") {
	// Arrange
	U8String source = u8"'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' '!'";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::CharLiteral, U8String("H"), {1, 1, 0}),
										 Token(TokenType::CharLiteral, U8String("e"), {1, 5, 4}),
										 Token(TokenType::CharLiteral, U8String("l"), {1, 9, 8}),
										 Token(TokenType::CharLiteral, U8String("l"), {1, 13, 12}),
										 Token(TokenType::CharLiteral, U8String("o"), {1, 17, 16}),
										 Token(TokenType::CharLiteral, U8String(","), {1, 21, 20}),
										 Token(TokenType::CharLiteral, U8String(" "), {1, 25, 24}),
										 Token(TokenType::CharLiteral, U8String("W"), {1, 29, 28}),
										 Token(TokenType::CharLiteral, U8String("o"), {1, 33, 32}),
										 Token(TokenType::CharLiteral, U8String("r"), {1, 37, 36}),
										 Token(TokenType::CharLiteral, U8String("l"), {1, 41, 40}),
										 Token(TokenType::CharLiteral, U8String("d"), {1, 45, 44}),
										 Token(TokenType::CharLiteral, U8String("!"), {1, 49, 48})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexChar: whole sentence in one char literal") {
	// Arrange
	U8String source = u8"'Hello, World!'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("Hello, World!"), startLoc,
						TokenError::MultipleCharsInCharLiteral);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: illegal escape sequence in char literal") {
	// Arrange
	U8String source = u8"'\\x'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("\\x"), startLoc,
						TokenError::InvalidEscapeSequence);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: missing closing quote in char literal and other tokens after") {
	// Arrange
	U8String source = u8"'a + b";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("a + b"), startLoc,
						TokenError::UnterminatedCharLiteral);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: missing opening quote in char literal") {
	// Arrange
	U8String source = u8"a'";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("a"), {1, 1, 0}),
										 Token(TokenType::Illegal, U8String(""), {1, 2, 1},
											   TokenError::UnterminatedCharLiteral)};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 3);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexChar: missing both quotes in char literal") {
	// Arrange
	U8String source = u8"a";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Identifier, U8String("a"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: missing closing quote and char literal in next line") {
	// Arrange
	U8String source = u8"'a\n'b'";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Illegal, U8String("a"), {1, 1, 0},
											   TokenError::UnterminatedCharLiteral),
										 Token(TokenType::CharLiteral, U8String("b"), {2, 1, 3})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 3);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexChar: missing closing quote and char literal in same line") {
	// Arrange
	U8String source = u8"'a 'b'";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Illegal, U8String("a "), {1, 1, 0},
											   TokenError::UnterminatedCharLiteral),
										 Token(TokenType::Identifier, U8String("b"), {1, 5, 4}),
										 Token(TokenType::Illegal, U8String(""), {1, 6, 5},
											   TokenError::UnterminatedCharLiteral)};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 4);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
	CHECK(tokens[2] == expectedTokens[2]);
}

TEST_CASE("LexChar: char literal with japanese character") {
	// Arrange
	U8String source = u8"'あ'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("あ"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with solo backslash") {
	// Arrange
	U8String source = u8"'\\'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("\\"), startLoc,
						TokenError::UnterminatedCharLiteral);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with solo backslash and unterminated") {
	// Arrange
	U8String source = u8"'\\";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("\\"), startLoc,
						TokenError::UnterminatedCharLiteral);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with qotation mark") {
	// Arrange
	U8String source = u8"'\"'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("\""), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with quotation mark (escaped)") {
	// Arrange
	U8String source = u8"'\\\"'";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::CharLiteral, U8String("\""), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

// LexSeparator tests
TEST_CASE("LexSeparator: single separator") {
	// Arrange
	U8String source = u8";";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Separator, U8String(";"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexSeparator: single separator tokens") {
	// Arrange
	U8String source = u8";,(){}[]:";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Separator, U8String(";"), {1, 1, 0}),
										 Token(TokenType::Separator, U8String(","), {1, 2, 1}),
										 Token(TokenType::Separator, U8String("("), {1, 3, 2}),
										 Token(TokenType::Separator, U8String(")"), {1, 4, 3}),
										 Token(TokenType::Separator, U8String("{"), {1, 5, 4}),
										 Token(TokenType::Separator, U8String("}"), {1, 6, 5}),
										 Token(TokenType::Separator, U8String("["), {1, 7, 6}),
										 Token(TokenType::Separator, U8String("]"), {1, 8, 7}),
										 Token(TokenType::Separator, U8String(":"), {1, 9, 8})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexSeparator: separators with whitespace") {
	// Arrange
	U8String source = u8" ; , ( ) { } [ ] : ";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Separator, U8String(u8";"), {1, 2, 1}),
										 Token(TokenType::Separator, U8String(u8","), {1, 4, 3}),
										 Token(TokenType::Separator, U8String(u8"("), {1, 6, 5}),
										 Token(TokenType::Separator, U8String(u8")"), {1, 8, 7}),
										 Token(TokenType::Separator, U8String(u8"{"), {1, 10, 9}),
										 Token(TokenType::Separator, U8String(u8"}"), {1, 12, 11}),
										 Token(TokenType::Separator, U8String(u8"["), {1, 14, 13}),
										 Token(TokenType::Separator, U8String(u8"]"), {1, 16, 15}),
										 Token(TokenType::Separator, U8String(u8":"), {1, 18, 17})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexSeparator: separators across multiple lines") {
	// Arrange
	U8String source = u8";\n,\n(\n)\n{\n}\n[\n]\n:";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Separator, U8String(";"), {1, 1, 0}),
										 Token(TokenType::Separator, U8String(","), {2, 1, 2}),
										 Token(TokenType::Separator, U8String("("), {3, 1, 4}),
										 Token(TokenType::Separator, U8String(")"), {4, 1, 6}),
										 Token(TokenType::Separator, U8String("{"), {5, 1, 8}),
										 Token(TokenType::Separator, U8String("}"), {6, 1, 10}),
										 Token(TokenType::Separator, U8String("["), {7, 1, 12}),
										 Token(TokenType::Separator, U8String("]"), {8, 1, 14}),
										 Token(TokenType::Separator, U8String(":"), {9, 1, 16})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

// LexOperator tests
TEST_CASE("LexOperator: single operator") {
	// Arrange
	U8String source = u8"+";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Operator, U8String("+"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexOperator: multiple operators") {
	// Arrange
	U8String source = u8"+ - * / % == != < > <= >= && || ! =";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Operator, U8String("+"), {1, 1, 0}),
										 Token(TokenType::Operator, U8String("-"), {1, 3, 2}),
										 Token(TokenType::Operator, U8String("*"), {1, 5, 4}),
										 Token(TokenType::Operator, U8String("/"), {1, 7, 6}),
										 Token(TokenType::Operator, U8String("%"), {1, 9, 8}),
										 Token(TokenType::Operator, U8String("=="), {1, 11, 10}),
										 Token(TokenType::Operator, U8String("!="), {1, 14, 13}),
										 Token(TokenType::Operator, U8String("<"), {1, 17, 16}),
										 Token(TokenType::Operator, U8String(">"), {1, 19, 18}),
										 Token(TokenType::Operator, U8String("<="), {1, 21, 20}),
										 Token(TokenType::Operator, U8String(">="), {1, 24, 23}),
										 Token(TokenType::Operator, U8String("&&"), {1, 27, 26}),
										 Token(TokenType::Operator, U8String("||"), {1, 30, 29}),
										 Token(TokenType::Operator, U8String("!"), {1, 33, 32}),
										 Token(TokenType::Operator, U8String("="), {1, 35, 34})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexOperator: operators with whitespace") {
	// Arrange
	U8String source = u8"  +   - \t * \n / % ";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Operator, U8String("+"), {1, 3, 2}),
										 Token(TokenType::Operator, U8String("-"), {1, 7, 6}),
										 Token(TokenType::Operator, U8String("*"), {1, 10, 9}),
										 Token(TokenType::Operator, U8String("/"), {2, 2, 13}),
										 Token(TokenType::Operator, U8String("%"), {2, 4, 15})};

	std::vector<Token> expectedTokens_recalc =
			{Token(TokenType::Operator, U8String("+"), {1, 3, 2}),
			 Token(TokenType::Operator, U8String("-"), {1, 7, 6}),
			 Token(TokenType::Operator, U8String("*"), {1, 11, 10}),
			 Token(TokenType::Operator, U8String("/"), {2, 2, 14}),
			 Token(TokenType::Operator, U8String("%"), {2, 4, 16})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens_recalc.size() + 1);
	for (size_t i = 0; i < expectedTokens_recalc.size(); ++i) {
		CHECK(tokens[i] == expectedTokens_recalc[i]);
	}
}

TEST_CASE("LexOperator: multi operators without spaces") {
	// Arrange
	U8String source = u8"==!=<><=>=&&||";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Operator, U8String("=="), {1, 1, 0}),
										 Token(TokenType::Operator, U8String("!="), {1, 3, 2}),
										 Token(TokenType::Operator, U8String("<"), {1, 5, 4}),
										 Token(TokenType::Operator, U8String(">"), {1, 6, 5}),
										 Token(TokenType::Operator, U8String("<="), {1, 7, 6}),
										 Token(TokenType::Operator, U8String(">="), {1, 9, 8}),
										 Token(TokenType::Operator, U8String("&&"), {1, 11, 10}),
										 Token(TokenType::Operator, U8String("||"), {1, 13, 12})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexOperator: lex triple character operators") {
	// Arrange
	U8String source = u8">>= <<=";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Operator, U8String(">>="), {1, 1, 0}),
										 Token(TokenType::Operator, U8String("<<="), {1, 5, 4})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexOperator: lex triple operators without spaces") {
	// Arrange
	U8String source = u8"<<=>>=";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Operator, U8String("<<="), {1, 1, 0}),
										 Token(TokenType::Operator, U8String(">>="), {1, 4, 3})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexOperator: mixed with other tokens") {
	// Arrange
	U8String source = u8"result = a + b * c - d / e;";
	Lexer lexer(std::move(source), true);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("result"),
											   {1, 1, 0}),
										 Token(TokenType::Operator, U8String("="), {1, 8, 7}),
										 Token(TokenType::Identifier, U8String("a"), {1, 10, 9}),
										 Token(TokenType::Operator, U8String("+"), {1, 12, 11}),
										 Token(TokenType::Identifier, U8String("b"), {1, 14, 13}),
										 Token(TokenType::Operator, U8String("*"), {1, 16, 15}),
										 Token(TokenType::Identifier, U8String("c"), {1, 18, 17}),
										 Token(TokenType::Operator, U8String("-"), {1, 20, 19}),
										 Token(TokenType::Identifier, U8String("d"), {1, 22, 21}),
										 Token(TokenType::Operator, U8String("/"), {1, 24, 23}),
										 Token(TokenType::Identifier, U8String("e"), {1, 26, 25}),
										 Token(TokenType::Separator, U8String(";"), {1, 27, 26})};

	CHECK(tokens.size() == expectedTokens.size() + 1);

	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexOperator: operators across multiple lines") {
	// Arrange
	U8String source = u8"+\n-\n*\n/\n%";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Operator, U8String("+"), {1, 1, 0}),
										 Token(TokenType::Operator, U8String("-"), {2, 1, 2}),
										 Token(TokenType::Operator, U8String("*"), {3, 1, 4}),
										 Token(TokenType::Operator, U8String("/"), {4, 1, 6}),
										 Token(TokenType::Operator, U8String("%"), {5, 1, 8})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

// LexIdentifierOrKeyword tests
TEST_CASE("LexIdentifierOrKeyword: single Identifier") {
	// Arrange
	U8String source = u8"variableName";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Identifier, U8String("variableName"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexIdentifierOrKeyword: single Keyword") {
	// Arrange
	U8String source = u8"if";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Keyword, U8String("if"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexSeparator: mixed with other tokens") {
	// Arrange
	U8String source = u8"x: i32 = 10;\nif (x > 5) {\n  x = x + 1;\n}";
	Lexer lexer(std::move(source), true);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("x"), {1, 1, 0}),
										 Token(TokenType::Separator, U8String(":"), {1, 2, 1}),
										 Token(TokenType::Identifier, U8String("i32"), {1, 4, 3}),
										 Token(TokenType::Operator, U8String("="), {1, 8, 7}),
										 Token(TokenType::IntLiteral, U8String("10"), {1, 10, 9}),
										 Token(TokenType::Separator, U8String(";"), {1, 12, 11}),
										 Token(TokenType::Keyword, U8String("if"), {2, 1, 13}),
										 Token(TokenType::Separator, U8String("("), {2, 4, 16}),
										 Token(TokenType::Identifier, U8String("x"), {2, 5, 17}),
										 Token(TokenType::Operator, U8String(">"), {2, 7, 19}),
										 Token(TokenType::IntLiteral, U8String("5"), {2, 9, 21}),
										 Token(TokenType::Separator, U8String(")"), {2, 10, 22}),
										 Token(TokenType::Separator, U8String("{"), {2, 12, 24}),
										 Token(TokenType::Identifier, U8String("x"), {3, 3, 28}),
										 Token(TokenType::Operator, U8String("="), {3, 5, 30}),
										 Token(TokenType::Identifier, U8String("x"), {3, 7, 32}),
										 Token(TokenType::Operator, U8String("+"), {3, 9, 34}),
										 Token(TokenType::IntLiteral, U8String("1"), {3, 11, 36}),
										 Token(TokenType::Separator, U8String(";"), {3, 12, 37}),
										 Token(TokenType::Separator, U8String("}"), {4, 1, 39})};

	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexIdentifierOrKeyword: identifiers with underscores and digits") {
	// Arrange
	U8String source = u8"_var1 var_2 var3_name";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("_var1"), {1, 1, 0}),
										 Token(TokenType::Identifier, U8String("var_2"), {1, 7, 6}),
										 Token(TokenType::Identifier, U8String("var3_name"),
											   {1, 13, 12})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords with whitespace") {
	// Arrange
	U8String source = u8"  var   if \t else \n while ";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("var"), {1, 3, 2}),
										 Token(TokenType::Keyword, U8String("if"), {1, 9, 8}),
										 Token(TokenType::Keyword, U8String("else"), {1, 14, 13}),
										 Token(TokenType::Keyword, U8String("while"), {2, 2, 20})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords across multiple lines") {
	// Arrange
	U8String source = u8"var\nif\nelse\nwhile";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("var"), {1, 1, 0}),
										 Token(TokenType::Keyword, U8String("if"), {2, 1, 4}),
										 Token(TokenType::Keyword, U8String("else"), {3, 1, 7}),
										 Token(TokenType::Keyword, U8String("while"), {4, 1, 12})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexIdentifierOrKeyword: identifiers starting with digits (illegal)") {
	// Arrange
	U8String source = u8"1variable 2var_name 3_var";
	Lexer lexer(std::move(source), true);
	std::vector<Token> expectedTokens =
			{Token(TokenType::IntLiteral, U8String("1"), {1, 1, 0}),
			 Token(TokenType::Identifier, U8String("variable"), {1, 2, 1}),
			 Token(TokenType::IntLiteral, U8String("2"), {1, 11, 10}),
			 Token(TokenType::Identifier, U8String("var_name"), {1, 12, 11}),
			 Token(TokenType::IntLiteral, U8String("3"), {1, 21, 20}),
			 Token(TokenType::Identifier, U8String("_var"), {1, 22, 21})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);
	CHECK(tokens[0] == expectedTokens[0]);
	CHECK(tokens[1] == expectedTokens[1]);
	CHECK(tokens[2] == expectedTokens[2]);
	CHECK(tokens[3] == expectedTokens[3]);
	CHECK(tokens[4] == expectedTokens[4]);
	CHECK(tokens[5] == expectedTokens[5]);
}

// LexComments tests
TEST_CASE("LexComments: single-line comment") {
	// Arrange
	U8String source = u8"// This is a single-line comment";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Comment, U8String(" This is a single-line comment"), startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexComments: multi-line comment") {
	// Arrange
	U8String source = u8"/* This is a \n multi-line comment */";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Comment, U8String(" This is a \n multi-line comment "),
						startLoc);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexComments: unclosed multi-line comment") {
	// Arrange
	U8String source = u8"/* This is an unclosed comment";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String(" This is an unclosed comment"), startLoc,
						TokenError::UnterminatedBlockComment);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

// LexIllegal tests
TEST_CASE("LexIllegal: single legal utf8-character") {
	// Arrange
	U8String source = u8"ß";
	Lexer lexer(std::move(source), true);
	SourceLoc startLoc{1, 1, 0};
	Token expectedToken(TokenType::Illegal, U8String("ß"), startLoc, TokenError::IllegalIdentifier);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == 2);
	CHECK(tokens[0] == expectedToken);
}

// General tests
TEST_CASE("LexGeneral: correct simple token sequence") {
	// Arrange
	U8String source = u8"x: i32 = 42; // variable declaration";
	Lexer lexer(std::move(source), true);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("x"), {1, 1, 0}),
										 Token(TokenType::Separator, U8String(":"), {1, 2, 1}),
										 Token(TokenType::Identifier, U8String("i32"), {1, 4, 3}),
										 Token(TokenType::Operator, U8String("="), {1, 8, 7}),
										 Token(TokenType::IntLiteral, U8String("42"), {1, 10, 9}),
										 Token(TokenType::Separator, U8String(";"), {1, 12, 11}),
										 Token(TokenType::Comment,
											   U8String(" variable declaration"), {1, 14, 13})};

	CHECK(tokens.size() == expectedTokens.size() + 1);

	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexGeneral: correct complex token sequence") {
	// Arrange
	U8String source = u8"if (x >= 10) {\n  x = x + 1;\n} else {\n  x = x - 1;\n}";
	Lexer lexer(std::move(source), true);

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	std::vector<Token> expectedTokens = {Token(TokenType::Keyword, U8String("if"), {1, 1, 0}),
										 Token(TokenType::Separator, U8String("("), {1, 4, 3}),
										 Token(TokenType::Identifier, U8String("x"), {1, 5, 4}),
										 Token(TokenType::Operator, U8String(">="), {1, 7, 6}),
										 Token(TokenType::IntLiteral, U8String("10"), {1, 10, 9}),
										 Token(TokenType::Separator, U8String(")"), {1, 12, 11}),
										 Token(TokenType::Separator, U8String("{"), {1, 14, 13}),
										 Token(TokenType::Identifier, U8String("x"), {2, 3, 17}),
										 Token(TokenType::Operator, U8String("="), {2, 5, 19}),
										 Token(TokenType::Identifier, U8String("x"), {2, 7, 21}),
										 Token(TokenType::Operator, U8String("+"), {2, 9, 23}),
										 Token(TokenType::IntLiteral, U8String("1"), {2, 11, 25}),
										 Token(TokenType::Separator, U8String(";"), {2, 12, 26}),
										 Token(TokenType::Separator, U8String("}"), {3, 1, 28}),
										 Token(TokenType::Keyword, U8String("else"), {3, 3, 30}),
										 Token(TokenType::Separator, U8String("{"), {3, 8, 35}),
										 Token(TokenType::Identifier, U8String("x"), {4, 3, 39}),
										 Token(TokenType::Operator, U8String("="), {4, 5, 41}),
										 Token(TokenType::Identifier, U8String("x"), {4, 7, 43}),
										 Token(TokenType::Operator, U8String("-"), {4, 9, 45}),
										 Token(TokenType::IntLiteral, U8String("1"), {4, 11, 47}),
										 Token(TokenType::Separator, U8String(";"), {4, 12, 48}),
										 Token(TokenType::Separator, U8String("}"), {5, 1, 50})};

	CHECK(tokens.size() == expectedTokens.size() + 1);

	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexGeneral: correct complex token sequence featuring all token types") {
	// Arrange
	U8String source = u8"ch: char = 'a'; // char literal\nif (ch == '\\n') {\n  /* multi-line \n "
					  u8"comment */\n  ch = 'b';\n}";
	Lexer lexer(std::move(source), true);

	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("ch"), {1, 1, 0}),
										 Token(TokenType::Separator, U8String(":"), {1, 3, 2}),
										 Token(TokenType::Identifier, U8String("char"), {1, 5, 4}),
										 Token(TokenType::Operator, U8String("="), {1, 10, 9}),
										 Token(TokenType::CharLiteral, U8String("a"), {1, 12, 11}),
										 Token(TokenType::Separator, U8String(";"), {1, 15, 14}),
										 Token(TokenType::Comment, U8String(" char literal"),
											   {1, 17, 16}),
										 Token(TokenType::Keyword, U8String("if"), {2, 1, 32}),
										 Token(TokenType::Separator, U8String("("), {2, 4, 35}),
										 Token(TokenType::Identifier, U8String("ch"), {2, 5, 36}),
										 Token(TokenType::Operator, U8String("=="), {2, 8, 39}),
										 Token(TokenType::CharLiteral, U8String("\n"), {2, 11, 42}),
										 Token(TokenType::Separator, U8String(")"), {2, 15, 46}),
										 Token(TokenType::Separator, U8String("{"), {2, 17, 48}),
										 Token(TokenType::Comment,
											   U8String(" multi-line \n comment "), {3, 3, 52}),
										 Token(TokenType::Identifier, U8String("ch"), {5, 3, 81}),
										 Token(TokenType::Operator, U8String("="), {5, 6, 84}),
										 Token(TokenType::CharLiteral, U8String("b"), {5, 8, 86}),
										 Token(TokenType::Separator, U8String(";"), {5, 11, 89}),
										 Token(TokenType::Separator, U8String("}"), {6, 1, 91})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);

	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		// This check now compares the full Token (type, lexeme, and loc)
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexGeneral: very long correct token sequence over multiple lines") {
	// Arrange
	U8String source =
			u8"total: i32 = 0;\nwhile (i == 0) {\n  total = total + i;\n}\n// End of loop";
	Lexer lexer(std::move(source), true);

	std::vector<Token> expectedTokens =
			{Token(TokenType::Identifier, U8String("total"), {1, 1, 0}),
			 Token(TokenType::Separator, U8String(":"), {1, 6, 5}),
			 Token(TokenType::Identifier, U8String("i32"), {1, 8, 7}),
			 Token(TokenType::Operator, U8String("="), {1, 12, 11}),
			 Token(TokenType::IntLiteral, U8String("0"), {1, 14, 13}),
			 Token(TokenType::Separator, U8String(";"), {1, 15, 14}),
			 Token(TokenType::Keyword, U8String("while"), {2, 1, 16}),
			 Token(TokenType::Separator, U8String("("), {2, 7, 22}),
			 Token(TokenType::Identifier, U8String("i"), {2, 8, 23}),
			 Token(TokenType::Operator, U8String("=="), {2, 10, 25}),
			 Token(TokenType::IntLiteral, U8String("0"), {2, 13, 28}),
			 Token(TokenType::Separator, U8String(")"), {2, 14, 29}),
			 Token(TokenType::Separator, U8String("{"), {2, 16, 31}),
			 Token(TokenType::Identifier, U8String("total"), {3, 3, 35}),
			 Token(TokenType::Operator, U8String("="), {3, 9, 41}),
			 Token(TokenType::Identifier, U8String("total"), {3, 11, 43}),
			 Token(TokenType::Operator, U8String("+"), {3, 17, 49}),
			 Token(TokenType::Identifier, U8String("i"), {3, 19, 51}),
			 Token(TokenType::Separator, U8String(";"), {3, 20, 52}),
			 Token(TokenType::Separator, U8String("}"), {4, 1, 54}),
			 Token(TokenType::Comment, U8String(" End of loop"), {5, 1, 56})};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);

	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		// This check now compares the full Token (type, lexeme, and loc)
		CHECK(tokens[i] == expectedTokens[i]);
	}
}

TEST_CASE("LexGeneral: incorrect simple token sequence") {
	// Arrange
	U8String source = u8"a: i32 = 10 'a";
	Lexer lexer(std::move(source), true);

	std::vector<Token> expectedTokens = {Token(TokenType::Identifier, U8String("a"), {1, 1, 0}),
										 Token(TokenType::Separator, U8String(":"), {1, 2, 1}),
										 Token(TokenType::Identifier, U8String("i32"), {1, 4, 3}),
										 Token(TokenType::Operator, U8String("="), {1, 8, 7}),
										 Token(TokenType::IntLiteral, U8String("10"), {1, 10, 9}),
										 Token(TokenType::Illegal, U8String("a"), {1, 13, 12},
											   TokenError::UnterminatedCharLiteral)};

	// Act
	std::vector<Token> tokens = lexer.tokenize();

	// Assert
	CHECK(tokens.size() == expectedTokens.size() + 1);

	for (size_t i = 0; i < expectedTokens.size(); ++i) {
		// This check now compares the full Token (type, lexeme, and loc)
		CHECK(tokens[i] == expectedTokens[i]);
	}
}
#endif