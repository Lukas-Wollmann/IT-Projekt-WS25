#include "Doctest.h"
#include "../src/Lexer.h"
#include "../src/core/U8String.h"

//LexNumber tests
TEST_CASE("LexNumber: simple integer")
{
    // Arrange
    U8String source = u8"12345";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[0].lexeme == U8String("12345"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexNumber: integer with trailing characters")
{
    // Arrange
    U8String source = u8"6789abc";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 2);
    CHECK(tokens[0].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[0].lexeme == U8String("6789"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].lexeme == U8String("abc"));
    CHECK(tokens[1].loc.line == 1);
    // 'abc' begins at column 5 (columns: '6'(1), '7'(2), '8'(3), '9'(4), 'a'(5))
    CHECK(tokens[1].loc.column == 5);
}

TEST_CASE("LexNumber: multiple integers separated by spaces")
{
    // Arrange
    U8String source = u8"42  1001\t256\n512";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 4);

    CHECK(tokens[0].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[0].lexeme == U8String("42"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[1].lexeme == U8String("1001"));
    CHECK(tokens[1].loc.line == 1);
    CHECK(tokens[1].loc.column == 5);

    CHECK(tokens[2].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[2].lexeme == U8String("256"));
    CHECK(tokens[2].loc.line == 1);
    CHECK(tokens[2].loc.column == 10);

    CHECK(tokens[3].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[3].lexeme == U8String("512"));
    CHECK(tokens[3].loc.line == 2);
    CHECK(tokens[3].loc.column == 1);
}

//LexString tests
TEST_CASE("LexString: simple string literal")
{
    // Arrange
    U8String source = u8"\"Hello, World!\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("Hello, World!"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: string with escaped characters")
{
    // Arrange
    U8String source = u8"\"Line1\\nLine2\\tTabbed\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("Line1\\nLine2\\tTabbed"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: unterminated string literal")
{
    // Arrange
    U8String source = u8"\"Unterminated string";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("Unterminated string"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: empty string literal")
{
    // Arrange
    U8String source = u8"\"\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String(""));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: string with unicode characters")
{
    // Arrange
    U8String source = u8"\"Unicode: \U0001F600 \U0001F603\""; // Grinning face emojis
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("Unicode: \U0001F600 \U0001F603"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: string with internal quotes")
{
    // Arrange
    U8String source = u8"\"She said, \\\"Hello!\\\"\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("She said, \\\"Hello!\\\""));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: string with escaped backslash")
{
    // Arrange
    U8String source = u8"\"This is a backslash: \\\\\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("This is a backslash: \\\\"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: string with various escaped characters")
{
    // Arrange
    U8String source = u8"\"Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: string with only escaped characters")
{
    // Arrange
    U8String source = u8"\"\\n\\t\\r\\\\\\'\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("\\n\\t\\r\\\\\\'"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: string with spaces and tabs")
{
    // Arrange
    U8String source = u8"\"   Leading and trailing spaces   \\t\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("   Leading and trailing spaces   \\t"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexString: multiple string literals separated by spaces")
{
    // Arrange
    U8String source = u8"\"First String\"   \"Second String\"\t\"Third String\"\n\"Fourth String\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 4);

    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(tokens[0].lexeme == U8String("First String"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::STRING_LITERAL);
    CHECK(tokens[1].lexeme == U8String("Second String"));
    // calculation: "First String" (14 chars incl quotes) + 3 spaces -> second starts at col 18
    CHECK(tokens[1].loc.line == 1);
    CHECK(tokens[1].loc.column == 18);

    CHECK(tokens[2].type == TokenType::STRING_LITERAL);
    CHECK(tokens[2].lexeme == U8String("Third String"));
    // token2 length incl quotes = 15, plus one '\t' counted as one column -> third starts at col 34
    CHECK(tokens[2].loc.line == 1);
    CHECK(tokens[2].loc.column == 34);

    CHECK(tokens[3].type == TokenType::STRING_LITERAL);
    CHECK(tokens[3].lexeme == U8String("Fourth String"));
    CHECK(tokens[3].loc.line == 2);
    CHECK(tokens[3].loc.column == 1);
}

//LexChar tests
TEST_CASE("LexChar: simple char literal")
{
    // Arrange
    U8String source = u8"'a'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String("a"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: escaped char literal")
{
    // Arrange
    U8String source = u8"'\\n'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String("\n"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: unterminated char literal")
{
    // Arrange
    U8String source = u8"'b";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("b"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: empty char literal")
{
    // Arrange
    U8String source = u8"''";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String(""));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: char literal with multiple characters")
{
    // Arrange
    U8String source = u8"'ab'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("ab"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: char literal with unicode character")
{
    // Arrange
    U8String source = u8"'\U0001F600'"; // Grinning face emoji
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String("\U0001F600"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: char literal with escaped single quote")
{
    // Arrange
    U8String source = u8"'\\''";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String("'"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: multiple char literals separated by spaces")
{
    // Arrange
    U8String source = u8"'x' 'y' 'z'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);

    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String("x"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[1].lexeme == U8String("y"));
    CHECK(tokens[1].loc.line == 1);
    // positions: "'x'" occupies cols 1-3, then space at 4, so second starts at 5
    CHECK(tokens[1].loc.column == 5);

    CHECK(tokens[2].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[2].lexeme == U8String("z"));
    CHECK(tokens[2].loc.line == 1);
    // "'y'" occupies cols 5-7, space at 8, third starts at 9
    CHECK(tokens[2].loc.column == 9);
}

TEST_CASE("LexChar: multiple chars in multiple lines")
{
    // Arrange
    U8String source = u8"'a'\n'b'\n'c'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);

    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String("a"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[1].lexeme == U8String("b"));
    CHECK(tokens[1].loc.line == 2);
    CHECK(tokens[1].loc.column == 1);

    CHECK(tokens[2].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[2].lexeme == U8String("c"));
    CHECK(tokens[2].loc.line == 3);
    CHECK(tokens[2].loc.column == 1);
}

TEST_CASE("LexChar: char literal with escaped backslash")
{
    // Arrange
    U8String source = u8"'\\\\'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String("\\"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: char literal with space character")
{
    // Arrange
    U8String source = u8"' '";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[0].lexeme == U8String(" "));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: whole sentence in char literals")
{
    // Arrange
    U8String source = u8"'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' '!'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 13);

    std::vector<U8String> expectedChars = {
        U8String("H"), U8String("e"), U8String("l"), U8String("l"), U8String("o"),
        U8String(","), U8String(" "), U8String("W"), U8String("o"), U8String("r"),
        U8String("l"), U8String("d"), U8String("!")
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::CHAR_LITERAL);
        CHECK(tokens[i].lexeme == expectedChars[i]);
        CHECK(tokens[i].loc.line == 1);
        // Each char literal takes 3 columns plus 1 space, so column = i * 4 + 1
        CHECK(tokens[i].loc.column == static_cast<size_t>(i * 4 + 1));
    }
}

TEST_CASE("LexChar: whole sentence in one char literal")
{
    // Arrange
    U8String source = u8"'Hello, World!'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("Hello, World!"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: illegal escape sequence in char literal")
{
    // Arrange
    U8String source = u8"'\\x'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("\\x"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: missing closing quote in char literal and other tokens after")
{
    // Arrange
    U8String source = u8"'a + b";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);

    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("a + b"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: missing opening quote in char literal")
{
    // Arrange
    U8String source = u8"a'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 2);

    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[0].lexeme == U8String("a"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::ILLEGAL);
    CHECK(tokens[1].lexeme == U8String(""));
    CHECK(tokens[1].loc.line == 1);
    // The illegal token starts at column 2
    CHECK(tokens[1].loc.column == 2);
}

TEST_CASE("LexChar: missing both quotes in char literal")
{
    // Arrange
    U8String source = u8"a";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);

    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[0].lexeme == U8String("a"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexChar: missing closing quote and char literal in next line")
{
    // Arrange
    U8String source = u8"'a\n'b'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 2);

    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("a"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::CHAR_LITERAL);
    CHECK(tokens[1].lexeme == U8String("b"));
    CHECK(tokens[1].loc.line == 2);
    CHECK(tokens[1].loc.column == 1);
}

TEST_CASE("LexChar: missing closing quote and char literal in same line")
{
    // Arrange
    U8String source = u8"'a 'b'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);

    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("a"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].lexeme == U8String("b"));
    CHECK(tokens[1].loc.line == 1);
    CHECK(tokens[1].loc.column == 5);

    CHECK(tokens[2].type == TokenType::ILLEGAL);
    CHECK(tokens[2].lexeme == U8String(""));
    CHECK(tokens[2].loc.line == 1);
    CHECK(tokens[2].loc.column == 6);
}

//LexSeparator tests
TEST_CASE("LexSeparator: single separator")
{
    // Arrange
    U8String source = u8";";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::SEPARATOR);
    CHECK(tokens[0].lexeme == U8String(";"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexSeparator: single separator tokens")
{
    // Arrange
    U8String source = u8";,(){}[]:";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 9);

    std::vector<U8String> expectedSeparators = {
        U8String(";"), U8String(","), U8String("("), U8String(")"),
        U8String("{"), U8String("}"), U8String("["), U8String("]"),
        U8String(":")
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::SEPARATOR);
        CHECK(tokens[i].lexeme == expectedSeparators[i]);
        CHECK(tokens[i].loc.line == 1);
        // Each separator occupies one column, so column = i + 1
        CHECK(tokens[i].loc.column == static_cast<size_t>(i + 1));
    }
}

TEST_CASE("LexSeparator: separators with whitespace")
{
    // Arrange
    U8String source = u8" ; , ( ) { } [ ] : ";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 9);

    std::vector<U8String> expectedSeparators = {
        U8String(u8";"), U8String(u8","), U8String(u8"("), U8String(u8")"),
        U8String(u8"{"), U8String(u8"}"), U8String(u8"["), U8String(u8"]"),
        U8String(u8":")
    };

    std::vector<size_t> expectedColumns = {2, 4, 6, 8, 10, 12, 14, 16, 18};

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::SEPARATOR);
        CHECK(tokens[i].lexeme == expectedSeparators[i]);
        CHECK(tokens[i].loc.line == 1);
        CHECK(tokens[i].loc.column == expectedColumns[i]);
    }
}

TEST_CASE("LexSeparator: separators across multiple lines")
{
    // Arrange
    U8String source = u8";\n,\n(\n)\n{\n}\n[\n]\n:";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 9);

    std::vector<U8String> expectedSeparators = {
        U8String(";"), U8String(","), U8String("("), U8String(")"),
        U8String("{"), U8String("}"), U8String("["), U8String("]"),
        U8String(":")
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::SEPARATOR);
        CHECK(tokens[i].lexeme == expectedSeparators[i]);
        CHECK(tokens[i].loc.line == i + 1);
        CHECK(tokens[i].loc.column == 1);
    }
}

//LexOperator tests
TEST_CASE("LexOperator: single operator")
{
    // Arrange
    U8String source = u8"+";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::OPERATOR);
    CHECK(tokens[0].lexeme == U8String("+"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexOperator: multiple operators")
{
    // Arrange
    U8String source = u8"+ - * / % == != < > <= >= && || ! =";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedOperators = {
        U8String("+"), U8String("-"), U8String("*"), U8String("/"),
        U8String("%"), U8String("=="), U8String("!="), U8String("<"),
        U8String(">"), U8String("<="), U8String(">="), U8String("&&"),
        U8String("||"), U8String("!"), U8String("=")
    };

    CHECK(tokens.size() == expectedOperators.size());

    size_t currentColumn = 1;
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::OPERATOR);
        CHECK(tokens[i].lexeme == expectedOperators[i]);
        CHECK(tokens[i].loc.line == 1);
        CHECK(tokens[i].loc.column == currentColumn);
        // Update currentColumn: operator length + 1 space
        currentColumn += tokens[i].lexeme.length() + 1;
    }
}

TEST_CASE("LexOperator: operators with whitespace")
{
    // Arrange
    U8String source = u8"  +   - \t * \n / % ";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedOperators = {
        U8String("+"), U8String("-"), U8String("*"), U8String("/"), U8String("%")
    };

    CHECK(tokens.size() == expectedOperators.size());

    std::vector<size_t> expectedLines = {1, 1, 1, 2, 2};
    std::vector<size_t> expectedColumns = {3, 7, 10, 3, 5};

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::OPERATOR);
        CHECK(tokens[i].lexeme == expectedOperators[i]);
        CHECK(tokens[i].loc.line == expectedLines[i]);
        CHECK(tokens[i].loc.column == expectedColumns[i]);
    }
}

TEST_CASE("LexOperator: multi operators without spaces")
{
    // Arrange
    U8String source = u8"==!=<><=>=&&||";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedOperators = {
        U8String("=="), U8String("!="), U8String("<"), U8String(">"),
        U8String("<="), U8String(">="), U8String("&&"), U8String("||")
    };

    CHECK(tokens.size() == expectedOperators.size());

    size_t currentColumn = 1;
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::OPERATOR);
        CHECK(tokens[i].lexeme == expectedOperators[i]);
        CHECK(tokens[i].loc.line == 1);
        CHECK(tokens[i].loc.column == currentColumn);
        // Update currentColumn: operator length
        currentColumn += tokens[i].lexeme.length();
    }
}

TEST_CASE("LexOperator: lex triple character operators")
{
    // Arrange
    U8String source = u8">>= <<=";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedOperators = {
        U8String(">>="), U8String("<<=")
    };

    CHECK(tokens.size() == expectedOperators.size());

    size_t currentColumn = 1;
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::OPERATOR);
        CHECK(tokens[i].lexeme == expectedOperators[i]);
        CHECK(tokens[i].loc.line == 1);
        CHECK(tokens[i].loc.column == currentColumn);
        // Update currentColumn: operator length + 1 space
        currentColumn += tokens[i].lexeme.length() + 1;
    }
}

TEST_CASE("LexOperator: lex triple operators without spaces")
{
    // Arrange
    U8String source = u8"<<=>>=";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedOperators = {
        U8String("<<="), U8String(">>=")
    };

    CHECK(tokens.size() == expectedOperators.size());

    size_t currentColumn = 1;
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::OPERATOR);
        CHECK(tokens[i].lexeme == expectedOperators[i]);
        CHECK(tokens[i].loc.line == 1);
        CHECK(tokens[i].loc.column == currentColumn);
        // Update currentColumn: operator length
        currentColumn += tokens[i].lexeme.length();
    }
}

TEST_CASE("LexOperator: mixed with other tokens")
{
    // Arrange
    U8String source = u8"result = a + b * c - d / e;";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<TokenType> expectedTypes = {
        TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::IDENTIFIER,
        TokenType::OPERATOR, TokenType::IDENTIFIER, TokenType::OPERATOR,
        TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::IDENTIFIER,
        TokenType::OPERATOR, TokenType::IDENTIFIER, TokenType::SEPARATOR
    };

    CHECK(tokens.size() == expectedTypes.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTypes[i]);
    }
}

TEST_CASE("LexOperator: operators across multiple lines")
{
    // Arrange
    U8String source = u8"+\n-\n*\n/\n%";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedOperators = {
        U8String("+"), U8String("-"), U8String("*"), U8String("/"), U8String("%")
    };

    CHECK(tokens.size() == expectedOperators.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::OPERATOR);
        CHECK(tokens[i].lexeme == expectedOperators[i]);
        CHECK(tokens[i].loc.line == i + 1);
        CHECK(tokens[i].loc.column == 1);
    }
}

//LexIdentifierOrKeyword tests
TEST_CASE("LexIdentifierOrKeyword: single Identifier")
{
    // Arrange
    U8String source = u8"variableName";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[0].lexeme == U8String("variableName"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexIdentifierOrKeyword: single Keyword")
{
    // Arrange
    U8String source = u8"if";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::KEYWORD);
    CHECK(tokens[0].lexeme == U8String("if"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexSeparator: mixed with other tokens")
{
    // Arrange
    U8String source = u8"x: i32 = 10;\nif (x > 5) {\n  x = x + 1;\n}";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<TokenType> expectedTypes = {
        TokenType::IDENTIFIER, TokenType::SEPARATOR, TokenType::KEYWORD, TokenType::OPERATOR,
        TokenType::NUMERIC_LITERAL, TokenType::SEPARATOR, TokenType::KEYWORD,
        TokenType::SEPARATOR, TokenType::IDENTIFIER, TokenType::OPERATOR,
        TokenType::NUMERIC_LITERAL, TokenType::SEPARATOR, TokenType::SEPARATOR,
        TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::IDENTIFIER,
        TokenType::OPERATOR, TokenType::NUMERIC_LITERAL, TokenType::SEPARATOR,
        TokenType::SEPARATOR
    };

    CHECK(tokens.size() == expectedTypes.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTypes[i]);
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers with underscores and digits")
{
    // Arrange
    U8String source = u8"_var1 var_2 var3_name";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedIdentifiers = {
        U8String("_var1"), U8String("var_2"), U8String("var3_name")
    };

    CHECK(tokens.size() == expectedIdentifiers.size());

    size_t currentColumn = 1;
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == TokenType::IDENTIFIER);
        CHECK(tokens[i].lexeme == expectedIdentifiers[i]);
        CHECK(tokens[i].loc.line == 1);
        CHECK(tokens[i].loc.column == currentColumn);
        // Update currentColumn: identifier length + 1 space
        currentColumn += tokens[i].lexeme.length() + 1;
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords with whitespace")
{
    // Arrange
    U8String source = u8"  var   if \t else \n while ";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<std::pair<TokenType, U8String>> expectedTokens = {
        {TokenType::IDENTIFIER, U8String("var")},
        {TokenType::KEYWORD, U8String("if")},
        {TokenType::KEYWORD, U8String("else")},
        {TokenType::KEYWORD, U8String("while")}
    };

    CHECK(tokens.size() == expectedTokens.size());

    std::vector<size_t> expectedLines = {1, 1, 1, 2};
    std::vector<size_t> expectedColumns = {3, 9, 13, 2};

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTokens[i].first);
        CHECK(tokens[i].lexeme == expectedTokens[i].second);
        CHECK(tokens[i].loc.line == expectedLines[i]);
        CHECK(tokens[i].loc.column == expectedColumns[i]);
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords across multiple lines")
{
    // Arrange
    U8String source = u8"var\nif\nelse\nwhile";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<std::pair<TokenType, U8String>> expectedTokens = {
        {TokenType::IDENTIFIER, U8String("var")},
        {TokenType::KEYWORD, U8String("if")},
        {TokenType::KEYWORD, U8String("else")},
        {TokenType::KEYWORD, U8String("while")}
    };

    CHECK(tokens.size() == expectedTokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTokens[i].first);
        CHECK(tokens[i].lexeme == expectedTokens[i].second);
        CHECK(tokens[i].loc.line == i + 1);
        CHECK(tokens[i].loc.column == 1);
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers starting with digits (illegal)")
{
    // Arrange
    U8String source = u8"1variable 2var_name 3_var";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<U8String> expectedLexemes = {
        U8String("1"),
        U8String("variable"),
        U8String("2"),
        U8String("var_name"),
        U8String("3"),
        U8String("_var")
    };

    CHECK(tokens.size() == expectedLexemes.size());

    CHECK(tokens[0].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[0].lexeme == expectedLexemes[0]);
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].lexeme == expectedLexemes[1]);
    CHECK(tokens[1].loc.line == 1);
    CHECK(tokens[1].loc.column == 2);

    CHECK(tokens[2].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[2].lexeme == expectedLexemes[2]);
    CHECK(tokens[2].loc.line == 1);
    CHECK(tokens[2].loc.column == 11);

    CHECK(tokens[3].type == TokenType::IDENTIFIER);
    CHECK(tokens[3].lexeme == expectedLexemes[3]);
    CHECK(tokens[3].loc.line == 1); 
    CHECK(tokens[3].loc.column == 12);

    CHECK(tokens[4].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[4].lexeme == expectedLexemes[4]);
    CHECK(tokens[4].loc.line == 1);
    CHECK(tokens[4].loc.column == 21);

    CHECK(tokens[5].type == TokenType::IDENTIFIER);
    CHECK(tokens[5].lexeme == expectedLexemes[5]);
    CHECK(tokens[5].loc.line == 1);
    CHECK(tokens[5].loc.column == 22);
}

//LexComments tests
TEST_CASE("LexComments: single-line comment")
{
    // Arrange
    U8String source = u8"// This is a single-line comment";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1); 
    CHECK(tokens[0].type == TokenType::COMMENT);
    CHECK(tokens[0].lexeme == U8String(" This is a single-line comment"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexComments: multi-line comment")
{
    // Arrange
    U8String source = u8"/* This is a \n multi-line comment */";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1); 
    CHECK(tokens[0].type == TokenType::COMMENT);
    CHECK(tokens[0].lexeme == U8String(" This is a \n multi-line comment "));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

TEST_CASE("LexComments: unclosed multi-line comment")
{
    // Arrange
    U8String source = u8"/* This is an unclosed comment";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1); 
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String(" This is an unclosed comment"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}

//LexIllegal tests
TEST_CASE("LexIllegal: single legal utf8-character")
{
    // Arrange
    U8String source = u8"ß";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("ß"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}
/*
TEST_CASE("LexIllegal: utf8 illegal characters")
{
    // Arrange
    U8String source = u8"\xFF\xFE\xFA";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("\xFF\xFE\xFA"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);
}
*/

/*
TEST_CASE("LexIllegal: utf8 char in identifier")
{
    // Arrange
    U8String source = u8"varna\xFFme = 10;";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);

    CHECK(tokens[0].type == TokenType::ILLEGAL);
    CHECK(tokens[0].lexeme == U8String("varna\xFFme"));
    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1);

    CHECK(tokens[1].type == TokenType::OPERATOR);
    CHECK(tokens[1].lexeme == U8String("="));
    CHECK(tokens[1].loc.line == 1);
    CHECK(tokens[1].loc.column == 12);

    CHECK(tokens[2].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[2].lexeme == U8String("10"));
    CHECK(tokens[2].loc.line == 1);
    CHECK(tokens[2].loc.column == 14);
}
*/   

//General tests
TEST_CASE("LexGeneral: correct simple token sequence")
{
    // Arrange
    U8String source = u8"x: i32 = 42; // variable declaration";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<TokenType> expectedTypes = {
        TokenType::IDENTIFIER, TokenType::SEPARATOR, TokenType::KEYWORD, TokenType::OPERATOR,
        TokenType::NUMERIC_LITERAL, TokenType::SEPARATOR, TokenType::COMMENT
    };

    CHECK(tokens.size() == expectedTypes.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTypes[i]);
    }
}

TEST_CASE("LexGeneral: correct complex token sequence")
{
    // Arrange
    U8String source = u8"if (x >= 10) {\n  x = x + 1;\n} else {\n  x = x - 1;\n}";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<TokenType> expectedTypes = {
        TokenType::KEYWORD, TokenType::SEPARATOR, TokenType::IDENTIFIER,
        TokenType::OPERATOR, TokenType::NUMERIC_LITERAL, TokenType::SEPARATOR,
        TokenType::SEPARATOR, TokenType::IDENTIFIER, TokenType::OPERATOR,
        TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::NUMERIC_LITERAL,
        TokenType::SEPARATOR, TokenType::SEPARATOR, TokenType::KEYWORD,
        TokenType::SEPARATOR, TokenType::IDENTIFIER, TokenType::OPERATOR,
        TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::NUMERIC_LITERAL,
        TokenType::SEPARATOR, TokenType::SEPARATOR
    };

    CHECK(tokens.size() == expectedTypes.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTypes[i]);
    }
}

TEST_CASE("LexGeneral: correct complex token sequence featuring all token types")
{
    // Arrange
    U8String source = u8"ch: char = 'a'; // char literal\nif (ch == '\\n') {\n  /* multi-line \n comment */\n  ch = 'b';\n}";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<TokenType> expectedTypes = {
        TokenType::IDENTIFIER, TokenType::SEPARATOR, TokenType::KEYWORD, TokenType::OPERATOR,
        TokenType::CHAR_LITERAL, TokenType::SEPARATOR, TokenType::COMMENT,
        TokenType::KEYWORD, TokenType::SEPARATOR, TokenType::IDENTIFIER,
        TokenType::OPERATOR, TokenType::CHAR_LITERAL, TokenType::SEPARATOR,
        TokenType::SEPARATOR, TokenType::COMMENT, TokenType::IDENTIFIER,
        TokenType::OPERATOR, TokenType::CHAR_LITERAL, TokenType::SEPARATOR,
        TokenType::SEPARATOR
    };

    CHECK(tokens.size() == expectedTypes.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTypes[i]);
    }
}

TEST_CASE("LexGeneral: very long correct token sequence over multiple lines")
{
    // Arrange
    U8String source = u8"total: i32 = 0;\nwhile (i == 0) {\n  total = total + i;\n}\n// End of loop";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<TokenType> expectedTypes = {
        TokenType::IDENTIFIER, TokenType::SEPARATOR, TokenType::KEYWORD,
        TokenType::OPERATOR, TokenType::NUMERIC_LITERAL, TokenType::SEPARATOR,
        TokenType::KEYWORD, TokenType::SEPARATOR, TokenType::IDENTIFIER,
        TokenType::OPERATOR, TokenType::NUMERIC_LITERAL, TokenType::SEPARATOR,
        TokenType::SEPARATOR, TokenType::IDENTIFIER, TokenType::OPERATOR,
        TokenType::IDENTIFIER, TokenType::OPERATOR, TokenType::IDENTIFIER,
        TokenType::SEPARATOR, TokenType::SEPARATOR, TokenType::COMMENT
    };

    CHECK(tokens.size() == expectedTypes.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i].type == expectedTypes[i]);
    }
}

TEST_CASE("LexGeneral: incorrect simple token sequence")
{
    // Arrange
    U8String source = u8"a: i32 = 10 'a";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 6);
    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].type == TokenType::SEPARATOR);
    CHECK(tokens[2].type == TokenType::KEYWORD);
    CHECK(tokens[3].type == TokenType::OPERATOR);
    CHECK(tokens[4].type == TokenType::NUMERIC_LITERAL);
    CHECK(tokens[5].type == TokenType::ILLEGAL);
}