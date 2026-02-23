#include "Doctest.h"
#include "core/U8String.h"
#include "lexer/Lexer.h"

using namespace lex;
using enum TokenType;

//Some Overall Tests
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