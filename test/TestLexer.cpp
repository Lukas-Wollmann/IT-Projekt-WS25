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
    Token expectedToken(TokenType::NUMERIC_LITERAL, U8String("12345"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexNumber: integer with trailing characters")
{
    // Arrange
    U8String source = u8"6789abc";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::NUMERIC_LITERAL, U8String("6789"), {1, 1, 0}),
        Token(TokenType::IDENTIFIER, U8String("abc"), {1, 5, 4})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 2);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexNumber: multiple integers separated by spaces")
{
    // Arrange
    U8String source = u8"42  1001\t256\n512";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::NUMERIC_LITERAL, U8String("42"), {1, 1, 0}),
        Token(TokenType::NUMERIC_LITERAL, U8String("1001"), {1, 5, 4}),
        Token(TokenType::NUMERIC_LITERAL, U8String("256"), {1, 10, 9}),
        Token(TokenType::NUMERIC_LITERAL, U8String("512"), {2, 1, 13})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 4);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
    CHECK(tokens[2] == expectedTokens[2]);
    CHECK(tokens[3] == expectedTokens[3]);
}

//LexString tests
TEST_CASE("LexString: simple string literal")
{
    // Arrange
    U8String source = u8"\"Hello, World!\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("Hello, World!"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with escaped characters")
{
    // Arrange
    U8String source = u8"\"Line1\\nLine2\\tTabbed\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("Line1\\nLine2\\tTabbed"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: unterminated string literal")
{
    // Arrange
    U8String source = u8"\"Unterminated string";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("Unterminated string"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: empty string literal")
{
    // Arrange
    U8String source = u8"\"\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String(""), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with unicode characters")
{
    // Arrange
    U8String source = u8"\"Unicode: \U0001F600 \U0001F603\""; // Grinning face emojis
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("Unicode: \U0001F600 \U0001F603"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with internal quotes")
{
    // Arrange
    U8String source = u8"\"She said, \\\"Hello!\\\"\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("She said, \\\"Hello!\\\""), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with escaped backslash")
{
    // Arrange
    U8String source = u8"\"This is a backslash: \\\\\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("This is a backslash: \\\\"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with various escaped characters")
{
    // Arrange
    U8String source = u8"\"Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("Tab:\\t NewLine:\\n CarriageReturn:\\r Quote:\\' Backslash:\\\\"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with only escaped characters")
{
    // Arrange
    U8String source = u8"\"\\n\\t\\r\\\\\\'\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("\\n\\t\\r\\\\\\'"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: string with spaces and tabs")
{
    // Arrange
    U8String source = u8"\"   Leading and trailing spaces   \\t\"";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::STRING_LITERAL, U8String("   Leading and trailing spaces   \\t"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexString: multiple string literals separated by spaces")
{
    // Arrange
    U8String source = u8"\"First String\"   \"Second String\"\t\"Third String\"\n\"Fourth String\"";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::STRING_LITERAL, U8String("First String"), {1, 1, 0}),
        Token(TokenType::STRING_LITERAL, U8String("Second String"), {1, 18, 17}),
        Token(TokenType::STRING_LITERAL, U8String("Third String"), {1, 34, 33}),
        Token(TokenType::STRING_LITERAL, U8String("Fourth String"), {2, 1, 48})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 4);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
    CHECK(tokens[2] == expectedTokens[2]);
    CHECK(tokens[3] == expectedTokens[3]);
}

//LexChar tests
TEST_CASE("LexChar: simple char literal")
{
    // Arrange
    U8String source = u8"'a'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::CHAR_LITERAL, U8String("a"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: escaped char literal")
{
    // Arrange
    U8String source = u8"'\\n'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::CHAR_LITERAL, U8String("\n"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: unterminated char literal")
{
    // Arrange
    U8String source = u8"'b";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("b"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: empty char literal")
{
    // Arrange
    U8String source = u8"''";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String(""), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with multiple characters")
{
    // Arrange
    U8String source = u8"'ab'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("ab"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with unicode character")
{
    // Arrange
    U8String source = u8"'\U0001F600'"; // Grinning face emoji
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::CHAR_LITERAL, U8String("\U0001F600"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with escaped single quote")
{
    // Arrange
    U8String source = u8"'\\''";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::CHAR_LITERAL, U8String("'"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: multiple char literals separated by spaces")
{
    // Arrange
    U8String source = u8"'x' 'y' 'z'";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::CHAR_LITERAL, U8String("x"), {1, 1, 0}),
        Token(TokenType::CHAR_LITERAL, U8String("y"), {1, 5, 4}),
        Token(TokenType::CHAR_LITERAL, U8String("z"), {1, 9, 8})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
    CHECK(tokens[2] == expectedTokens[2]);
}

TEST_CASE("LexChar: multiple chars in multiple lines")
{
    // Arrange
    U8String source = u8"'a'\n'b'\n'c'";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::CHAR_LITERAL, U8String("a"), {1, 1, 0}),
        Token(TokenType::CHAR_LITERAL, U8String("b"), {2, 1, 4}),
        Token(TokenType::CHAR_LITERAL, U8String("c"), {3, 1, 8})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
    CHECK(tokens[2] == expectedTokens[2]);
}

TEST_CASE("LexChar: char literal with escaped backslash")
{
    // Arrange
    U8String source = u8"'\\\\'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::CHAR_LITERAL, U8String("\\"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: char literal with space character")
{
    // Arrange
    U8String source = u8"' '";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::CHAR_LITERAL, U8String(" "), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: whole sentence in char literals")
{
    // Arrange
    U8String source = u8"'H' 'e' 'l' 'l' 'o' ',' ' ' 'W' 'o' 'r' 'l' 'd' '!'";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::CHAR_LITERAL, U8String("H"), {1, 1, 0}),
        Token(TokenType::CHAR_LITERAL, U8String("e"), {1, 5, 4}),
        Token(TokenType::CHAR_LITERAL, U8String("l"), {1, 9, 8}),
        Token(TokenType::CHAR_LITERAL, U8String("l"), {1, 13, 12}),
        Token(TokenType::CHAR_LITERAL, U8String("o"), {1, 17, 16}),
        Token(TokenType::CHAR_LITERAL, U8String(","), {1, 21, 20}),
        Token(TokenType::CHAR_LITERAL, U8String(" "), {1, 25, 24}),
        Token(TokenType::CHAR_LITERAL, U8String("W"), {1, 29, 28}),
        Token(TokenType::CHAR_LITERAL, U8String("o"), {1, 33, 32}),
        Token(TokenType::CHAR_LITERAL, U8String("r"), {1, 37, 36}),
        Token(TokenType::CHAR_LITERAL, U8String("l"), {1, 41, 40}),
        Token(TokenType::CHAR_LITERAL, U8String("d"), {1, 45, 44}),
        Token(TokenType::CHAR_LITERAL, U8String("!"), {1, 49, 48})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexChar: whole sentence in one char literal")
{
    // Arrange
    U8String source = u8"'Hello, World!'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("Hello, World!"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: illegal escape sequence in char literal")
{
    // Arrange
    U8String source = u8"'\\x'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("\\x"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: missing closing quote in char literal and other tokens after")
{
    // Arrange
    U8String source = u8"'a + b";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("a + b"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: missing opening quote in char literal")
{
    // Arrange
    U8String source = u8"a'";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("a"), {1, 1, 0}),
        Token(TokenType::ILLEGAL, U8String(""), {1, 2, 1})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 2);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexChar: missing both quotes in char literal")
{
    // Arrange
    U8String source = u8"a";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::IDENTIFIER, U8String("a"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexChar: missing closing quote and char literal in next line")
{
    // Arrange
    U8String source = u8"'a\n'b'";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::ILLEGAL, U8String("a"), {1, 1, 0}),
        Token(TokenType::CHAR_LITERAL, U8String("b"), {2, 1, 3})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 2);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexChar: missing closing quote and char literal in same line")
{
    // Arrange
    U8String source = u8"'a 'b'";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::ILLEGAL, U8String("a "), {1, 1, 0}),
        Token(TokenType::IDENTIFIER, U8String("b"), {1, 5, 4}),
        Token(TokenType::ILLEGAL, U8String(""), {1, 6, 5})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
    CHECK(tokens[2] == expectedTokens[2]);
}

TEST_CASE("LexChar: char literal with japanese character")
{
    // Arrange
    U8String source = u8"'あ'";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::CHAR_LITERAL, U8String("あ"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

//LexSeparator tests
TEST_CASE("LexSeparator: single separator")
{
    // Arrange
    U8String source = u8";";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::SEPARATOR, U8String(";"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexSeparator: single separator tokens")
{
    // Arrange
    U8String source = u8";,(){}[]:";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::SEPARATOR, U8String(";"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String(","), {1, 2, 1}),
        Token(TokenType::SEPARATOR, U8String("("), {1, 3, 2}),
        Token(TokenType::SEPARATOR, U8String(")"), {1, 4, 3}),
        Token(TokenType::SEPARATOR, U8String("{"), {1, 5, 4}),
        Token(TokenType::SEPARATOR, U8String("}"), {1, 6, 5}),
        Token(TokenType::SEPARATOR, U8String("["), {1, 7, 6}),
        Token(TokenType::SEPARATOR, U8String("]"), {1, 8, 7}),
        Token(TokenType::SEPARATOR, U8String(":"), {1, 9, 8})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexSeparator: separators with whitespace")
{
    // Arrange
    U8String source = u8" ; , ( ) { } [ ] : ";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::SEPARATOR, U8String(u8";"), {1, 2, 1}),
        Token(TokenType::SEPARATOR, U8String(u8","), {1, 4, 3}),
        Token(TokenType::SEPARATOR, U8String(u8"("), {1, 6, 5}),
        Token(TokenType::SEPARATOR, U8String(u8")"), {1, 8, 7}),
        Token(TokenType::SEPARATOR, U8String(u8"{"), {1, 10, 9}),
        Token(TokenType::SEPARATOR, U8String(u8"}"), {1, 12, 11}),
        Token(TokenType::SEPARATOR, U8String(u8"["), {1, 14, 13}),
        Token(TokenType::SEPARATOR, U8String(u8"]"), {1, 16, 15}),
        Token(TokenType::SEPARATOR, U8String(u8":"), {1, 18, 17})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexSeparator: separators across multiple lines")
{
    // Arrange
    U8String source = u8";\n,\n(\n)\n{\n}\n[\n]\n:";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::SEPARATOR, U8String(";"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String(","), {2, 1, 2}),
        Token(TokenType::SEPARATOR, U8String("("), {3, 1, 4}),
        Token(TokenType::SEPARATOR, U8String(")"), {4, 1, 6}),
        Token(TokenType::SEPARATOR, U8String("{"), {5, 1, 8}),
        Token(TokenType::SEPARATOR, U8String("}"), {6, 1, 10}),
        Token(TokenType::SEPARATOR, U8String("["), {7, 1, 12}),
        Token(TokenType::SEPARATOR, U8String("]"), {8, 1, 14}),
        Token(TokenType::SEPARATOR, U8String(":"), {9, 1, 16})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

//LexOperator tests
TEST_CASE("LexOperator: single operator")
{
    // Arrange
    U8String source = u8"+";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::OPERATOR, U8String("+"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexOperator: multiple operators")
{
    // Arrange
    U8String source = u8"+ - * / % == != < > <= >= && || ! =";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::OPERATOR, U8String("+"), {1, 1, 0}),
        Token(TokenType::OPERATOR, U8String("-"), {1, 3, 2}),
        Token(TokenType::OPERATOR, U8String("*"), {1, 5, 4}),
        Token(TokenType::OPERATOR, U8String("/"), {1, 7, 6}),
        Token(TokenType::OPERATOR, U8String("%"), {1, 9, 8}),
        Token(TokenType::OPERATOR, U8String("=="), {1, 11, 10}),
        Token(TokenType::OPERATOR, U8String("!="), {1, 14, 13}),
        Token(TokenType::OPERATOR, U8String("<"), {1, 17, 16}),
        Token(TokenType::OPERATOR, U8String(">"), {1, 19, 18}),
        Token(TokenType::OPERATOR, U8String("<="), {1, 21, 20}),
        Token(TokenType::OPERATOR, U8String(">="), {1, 24, 23}),
        Token(TokenType::OPERATOR, U8String("&&"), {1, 27, 26}),
        Token(TokenType::OPERATOR, U8String("||"), {1, 30, 29}),
        Token(TokenType::OPERATOR, U8String("!"), {1, 33, 32}),
        Token(TokenType::OPERATOR, U8String("="), {1, 35, 34})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexOperator: operators with whitespace")
{
    // Arrange
    U8String source = u8"  +   - \t * \n / % ";
    Lexer lexer(source);
     std::vector<Token> expectedTokens = {
        Token(TokenType::OPERATOR, U8String("+"), {1, 3, 2}),
        Token(TokenType::OPERATOR, U8String("-"), {1, 7, 6}),
        Token(TokenType::OPERATOR, U8String("*"), {1, 10, 9}),
        Token(TokenType::OPERATOR, U8String("/"), {2, 2, 13}),
        Token(TokenType::OPERATOR, U8String("%"), {2, 4, 15}) 
    };

    std::vector<Token> expectedTokens_recalc = {
        Token(TokenType::OPERATOR, U8String("+"), {1, 3, 2}),
        Token(TokenType::OPERATOR, U8String("-"), {1, 7, 6}),
        Token(TokenType::OPERATOR, U8String("*"), {1, 11, 10}),
        Token(TokenType::OPERATOR, U8String("/"), {2, 2, 14}),
        Token(TokenType::OPERATOR, U8String("%"), {2, 4, 16})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens_recalc.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens_recalc[i]);
    }
}

TEST_CASE("LexOperator: multi operators without spaces")
{
    // Arrange
    U8String source = u8"==!=<><=>=&&||";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::OPERATOR, U8String("=="), {1, 1, 0}),
        Token(TokenType::OPERATOR, U8String("!="), {1, 3, 2}),
        Token(TokenType::OPERATOR, U8String("<"), {1, 5, 4}),
        Token(TokenType::OPERATOR, U8String(">"), {1, 6, 5}),
        Token(TokenType::OPERATOR, U8String("<="), {1, 7, 6}),
        Token(TokenType::OPERATOR, U8String(">="), {1, 9, 8}),
        Token(TokenType::OPERATOR, U8String("&&"), {1, 11, 10}),
        Token(TokenType::OPERATOR, U8String("||"), {1, 13, 12})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexOperator: lex triple character operators")
{
    // Arrange
    U8String source = u8">>= <<=";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::OPERATOR, U8String(">>="), {1, 1, 0}),
        Token(TokenType::OPERATOR, U8String("<<="), {1, 5, 4})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexOperator: lex triple operators without spaces")
{
    // Arrange
    U8String source = u8"<<=>>=";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::OPERATOR, U8String("<<="), {1, 1, 0}),
        Token(TokenType::OPERATOR, U8String(">>="), {1, 4, 3})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
}

TEST_CASE("LexOperator: mixed with other tokens")
{
    // Arrange
    U8String source = u8"result = a + b * c - d / e;";
    Lexer lexer(source);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("result"), {1, 1, 0}),
        Token(TokenType::OPERATOR, U8String("="), {1, 8, 7}),
        Token(TokenType::IDENTIFIER, U8String("a"), {1, 10, 9}),
        Token(TokenType::OPERATOR, U8String("+"), {1, 12, 11}),
        Token(TokenType::IDENTIFIER, U8String("b"), {1, 14, 13}),
        Token(TokenType::OPERATOR, U8String("*"), {1, 16, 15}),
        Token(TokenType::IDENTIFIER, U8String("c"), {1, 18, 17}),
        Token(TokenType::OPERATOR, U8String("-"), {1, 20, 19}),
        Token(TokenType::IDENTIFIER, U8String("d"), {1, 22, 21}),
        Token(TokenType::OPERATOR, U8String("/"), {1, 24, 23}),
        Token(TokenType::IDENTIFIER, U8String("e"), {1, 26, 25}),
        Token(TokenType::SEPARATOR, U8String(";"), {1, 27, 26})
    };

    CHECK(tokens.size() == expectedTokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexOperator: operators across multiple lines")
{
    // Arrange
    U8String source = u8"+\n-\n*\n/\n%";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::OPERATOR, U8String("+"), {1, 1, 0}),
        Token(TokenType::OPERATOR, U8String("-"), {2, 1, 2}),
        Token(TokenType::OPERATOR, U8String("*"), {3, 1, 4}),
        Token(TokenType::OPERATOR, U8String("/"), {4, 1, 6}),
        Token(TokenType::OPERATOR, U8String("%"), {5, 1, 8})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

//LexIdentifierOrKeyword tests
TEST_CASE("LexIdentifierOrKeyword: single Identifier")
{
    // Arrange
    U8String source = u8"variableName";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::IDENTIFIER, U8String("variableName"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexIdentifierOrKeyword: single Keyword")
{
    // Arrange
    U8String source = u8"if";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::KEYWORD, U8String("if"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexSeparator: mixed with other tokens")
{
    // Arrange
    U8String source = u8"x: i32 = 10;\nif (x > 5) {\n  x = x + 1;\n}";
    Lexer lexer(source);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("x"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String(":"), {1, 2, 1}),
        Token(TokenType::KEYWORD, U8String("i32"), {1, 4, 3}),
        Token(TokenType::OPERATOR, U8String("="), {1, 8, 7}),
        Token(TokenType::NUMERIC_LITERAL, U8String("10"), {1, 10, 9}),
        Token(TokenType::SEPARATOR, U8String(";"), {1, 12, 11}),
        Token(TokenType::KEYWORD, U8String("if"), {2, 1, 13}),
        Token(TokenType::SEPARATOR, U8String("("), {2, 4, 16}),
        Token(TokenType::IDENTIFIER, U8String("x"), {2, 5, 17}),
        Token(TokenType::OPERATOR, U8String(">"), {2, 7, 19}),
        Token(TokenType::NUMERIC_LITERAL, U8String("5"), {2, 9, 21}),
        Token(TokenType::SEPARATOR, U8String(")"), {2, 10, 22}),
        Token(TokenType::SEPARATOR, U8String("{"), {2, 12, 24}),
        Token(TokenType::IDENTIFIER, U8String("x"), {3, 3, 28}),
        Token(TokenType::OPERATOR, U8String("="), {3, 5, 30}),
        Token(TokenType::IDENTIFIER, U8String("x"), {3, 7, 32}),
        Token(TokenType::OPERATOR, U8String("+"), {3, 9, 34}),
        Token(TokenType::NUMERIC_LITERAL, U8String("1"), {3, 11, 36}),
        Token(TokenType::SEPARATOR, U8String(";"), {3, 12, 37}),
        Token(TokenType::SEPARATOR, U8String("}"), {4, 1, 39})
    };

    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers with underscores and digits")
{
    // Arrange
    U8String source = u8"_var1 var_2 var3_name";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("_var1"), {1, 1, 0}),
        Token(TokenType::IDENTIFIER, U8String("var_2"), {1, 7, 6}),
        Token(TokenType::IDENTIFIER, U8String("var3_name"), {1, 13, 12})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords with whitespace")
{
    // Arrange
    U8String source = u8"  var   if \t else \n while ";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("var"), {1, 3, 2}),
        Token(TokenType::KEYWORD, U8String("if"), {1, 9, 8}),
        Token(TokenType::KEYWORD, U8String("else"), {1, 14, 13}),
        Token(TokenType::KEYWORD, U8String("while"), {2, 2, 20})
    };
    
    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers and keywords across multiple lines")
{
    // Arrange
    U8String source = u8"var\nif\nelse\nwhile";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("var"), {1, 1, 0}),
        Token(TokenType::KEYWORD, U8String("if"), {2, 1, 4}),
        Token(TokenType::KEYWORD, U8String("else"), {3, 1, 7}),
        Token(TokenType::KEYWORD, U8String("while"), {4, 1, 12})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexIdentifierOrKeyword: identifiers starting with digits (illegal)")
{
    // Arrange
    U8String source = u8"1variable 2var_name 3_var";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::NUMERIC_LITERAL, U8String("1"), {1, 1, 0}),
        Token(TokenType::IDENTIFIER, U8String("variable"), {1, 2, 1}),
        Token(TokenType::NUMERIC_LITERAL, U8String("2"), {1, 11, 10}),
        Token(TokenType::IDENTIFIER, U8String("var_name"), {1, 12, 11}),
        Token(TokenType::NUMERIC_LITERAL, U8String("3"), {1, 21, 20}),
        Token(TokenType::IDENTIFIER, U8String("_var"), {1, 22, 21})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
    CHECK(tokens[2] == expectedTokens[2]);
    CHECK(tokens[3] == expectedTokens[3]);
    CHECK(tokens[4] == expectedTokens[4]);
    CHECK(tokens[5] == expectedTokens[5]);
}

//LexComments tests
TEST_CASE("LexComments: single-line comment")
{
    // Arrange
    U8String source = u8"// This is a single-line comment";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::COMMENT, U8String(" This is a single-line comment"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexComments: multi-line comment")
{
    // Arrange
    U8String source = u8"/* This is a \n multi-line comment */";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::COMMENT, U8String(" This is a \n multi-line comment "), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexComments: unclosed multi-line comment")
{
    // Arrange
    U8String source = u8"/* This is an unclosed comment";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String(" This is an unclosed comment"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

//LexIllegal tests
TEST_CASE("LexIllegal: single legal utf8-character")
{
    // Arrange
    U8String source = u8"ß";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("ß"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexIllegal: utf8 illegal characters")
{
    // Arrange
    U8String source = u8"\xFF\xFE\xFA";
    Lexer lexer(source);
    SourceLoc startLoc{1, 1, 0};
    Token expectedToken(TokenType::ILLEGAL, U8String("\xFF\xFE\xFA"), startLoc);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == expectedToken);
}

TEST_CASE("LexIllegal: utf8 char in identifier")
{
    // Arrange
    U8String source = u8"varna\xFFme = 10;";
    Lexer lexer(source);
    std::vector<Token> expectedTokens = {
        Token(TokenType::ILLEGAL, U8String("varna\xFFme"), {1, 1, 0}),
        Token(TokenType::OPERATOR, U8String("="), {1, 10, 9}), // Original col 12 was wrong for u8
        Token(TokenType::NUMERIC_LITERAL, U8String("10"), {1, 12, 11}) // Original col 14 was wrong for u8
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == 3);
    CHECK(tokens[0] == expectedTokens[0]);
    CHECK(tokens[1] == expectedTokens[1]);
    CHECK(tokens[2] == expectedTokens[2]);
}

//General tests
TEST_CASE("LexGeneral: correct simple token sequence")
{
    // Arrange
    U8String source = u8"x: i32 = 42; // variable declaration";
    Lexer lexer(source);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("x"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String(":"), {1, 2, 1}),
        Token(TokenType::KEYWORD, U8String("i32"), {1, 4, 3}),
        Token(TokenType::OPERATOR, U8String("="), {1, 8, 7}),
        Token(TokenType::NUMERIC_LITERAL, U8String("42"), {1, 10, 9}),
        Token(TokenType::SEPARATOR, U8String(";"), {1, 12, 11}),
        Token(TokenType::COMMENT, U8String(" variable declaration"), {1, 14, 13})
    };

    CHECK(tokens.size() == expectedTokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexGeneral: correct complex token sequence")
{
    // Arrange
    U8String source = u8"if (x >= 10) {\n  x = x + 1;\n} else {\n  x = x - 1;\n}";
    Lexer lexer(source);

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    std::vector<Token> expectedTokens = {
        Token(TokenType::KEYWORD, U8String("if"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String("("), {1, 4, 3}),
        Token(TokenType::IDENTIFIER, U8String("x"), {1, 5, 4}),
        Token(TokenType::OPERATOR, U8String(">="), {1, 7, 6}),
        Token(TokenType::NUMERIC_LITERAL, U8String("10"), {1, 10, 9}),
        Token(TokenType::SEPARATOR, U8String(")"), {1, 12, 11}),
        Token(TokenType::SEPARATOR, U8String("{"), {1, 14, 13}),
        Token(TokenType::IDENTIFIER, U8String("x"), {2, 3, 17}),
        Token(TokenType::OPERATOR, U8String("="), {2, 5, 19}),
        Token(TokenType::IDENTIFIER, U8String("x"), {2, 7, 21}),
        Token(TokenType::OPERATOR, U8String("+"), {2, 9, 23}),
        Token(TokenType::NUMERIC_LITERAL, U8String("1"), {2, 11, 25}),
        Token(TokenType::SEPARATOR, U8String(";"), {2, 12, 26}),
        Token(TokenType::SEPARATOR, U8String("}"), {3, 1, 28}),
        Token(TokenType::KEYWORD, U8String("else"), {3, 3, 30}),
        Token(TokenType::SEPARATOR, U8String("{"), {3, 8, 35}),
        Token(TokenType::IDENTIFIER, U8String("x"), {4, 3, 39}),
        Token(TokenType::OPERATOR, U8String("="), {4, 5, 41}),
        Token(TokenType::IDENTIFIER, U8String("x"), {4, 7, 43}),
        Token(TokenType::OPERATOR, U8String("-"), {4, 9, 45}),
        Token(TokenType::NUMERIC_LITERAL, U8String("1"), {4, 11, 47}),
        Token(TokenType::SEPARATOR, U8String(";"), {4, 12, 48}),
        Token(TokenType::SEPARATOR, U8String("}"), {5, 1, 50})
    };

    CHECK(tokens.size() == expectedTokens.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexGeneral: correct complex token sequence featuring all token types")
{
    // Arrange
    U8String source = u8"ch: char = 'a'; // char literal\nif (ch == '\\n') {\n  /* multi-line \n comment */\n  ch = 'b';\n}";
    Lexer lexer(source);
    
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("ch"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String(":"), {1, 3, 2}),
        Token(TokenType::KEYWORD, U8String("char"), {1, 5, 4}),
        Token(TokenType::OPERATOR, U8String("="), {1, 10, 9}),
        Token(TokenType::CHAR_LITERAL, U8String("a"), {1, 12, 11}),
        Token(TokenType::SEPARATOR, U8String(";"), {1, 15, 14}),
        Token(TokenType::COMMENT, U8String(" char literal"), {1, 17, 16}),
        Token(TokenType::KEYWORD, U8String("if"), {2, 1, 34}),
        Token(TokenType::SEPARATOR, U8String("("), {2, 4, 37}),
        Token(TokenType::IDENTIFIER, U8String("ch"), {2, 5, 38}),
        Token(TokenType::OPERATOR, U8String("=="), {2, 8, 41}),
        Token(TokenType::CHAR_LITERAL, U8String("\n"), {2, 11, 44}),
        Token(TokenType::SEPARATOR, U8String(")"), {2, 15, 48}),
        Token(TokenType::SEPARATOR, U8String("{"), {2, 17, 50}),
        Token(TokenType::COMMENT, U8String(" multi-line \n comment "), {3, 3, 54}),
        Token(TokenType::IDENTIFIER, U8String("ch"), {5, 3, 85}),
        Token(TokenType::OPERATOR, U8String("="), {5, 6, 88}),
        Token(TokenType::CHAR_LITERAL, U8String("b"), {5, 8, 90}),
        Token(TokenType::SEPARATOR, U8String(";"), {5, 11, 93}),
        Token(TokenType::SEPARATOR, U8String("}"), {6, 1, 95})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());

    for (size_t i = 0; i < expectedTokens.size(); ++i) {
        // This check now compares the full Token (type, lexeme, and loc)
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexGeneral: very long correct token sequence over multiple lines")
{
    // Arrange
    U8String source = u8"total: i32 = 0;\nwhile (i == 0) {\n  total = total + i;\n}\n// End of loop";
    Lexer lexer(source);
    
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("total"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String(":"), {1, 6, 5}),
        Token(TokenType::KEYWORD, U8String("i32"), {1, 8, 7}),
        Token(TokenType::OPERATOR, U8String("="), {1, 12, 11}),
        Token(TokenType::NUMERIC_LITERAL, U8String("0"), {1, 14, 13}),
        Token(TokenType::SEPARATOR, U8String(";"), {1, 15, 14}),
        Token(TokenType::KEYWORD, U8String("while"), {2, 1, 16}),
        Token(TokenType::SEPARATOR, U8String("("), {2, 7, 22}),
        Token(TokenType::IDENTIFIER, U8String("i"), {2, 8, 23}),
        Token(TokenType::OPERATOR, U8String("=="), {2, 10, 25}),
        Token(TokenType::NUMERIC_LITERAL, U8String("0"), {2, 13, 28}),
        Token(TokenType::SEPARATOR, U8String(")"), {2, 14, 29}),
        Token(TokenType::SEPARATOR, U8String("{"), {2, 16, 31}),
        Token(TokenType::IDENTIFIER, U8String("total"), {3, 3, 35}),
        Token(TokenType::OPERATOR, U8String("="), {3, 9, 41}),
        Token(TokenType::IDENTIFIER, U8String("total"), {3, 11, 43}),
        Token(TokenType::OPERATOR, U8String("+"), {3, 17, 49}),
        Token(TokenType::IDENTIFIER, U8String("i"), {3, 19, 51}),
        Token(TokenType::SEPARATOR, U8String(";"), {3, 20, 52}),
        Token(TokenType::SEPARATOR, U8String("}"), {4, 1, 54}),
        Token(TokenType::COMMENT, U8String(" End of loop"), {5, 1, 56})
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());

    for (size_t i = 0; i < expectedTokens.size(); ++i) {
        // This check now compares the full Token (type, lexeme, and loc)
        CHECK(tokens[i] == expectedTokens[i]);
    }
}

TEST_CASE("LexGeneral: incorrect simple token sequence")
{
    // Arrange
    U8String source = u8"a: i32 = 10 'a";
    Lexer lexer(source);
    
    std::vector<Token> expectedTokens = {
        Token(TokenType::IDENTIFIER, U8String("a"), {1, 1, 0}),
        Token(TokenType::SEPARATOR, U8String(":"), {1, 2, 1}),
        Token(TokenType::KEYWORD, U8String("i32"), {1, 4, 3}),
        Token(TokenType::OPERATOR, U8String("="), {1, 8, 7}),
        Token(TokenType::NUMERIC_LITERAL, U8String("10"), {1, 10, 9}),
        Token(TokenType::ILLEGAL, U8String("a"), {1, 13, 12}) // Unterminated char literal
    };

    // Act
    std::vector<Token> tokens = lexer.tokenize();

    // Assert
    CHECK(tokens.size() == expectedTokens.size());

    for (size_t i = 0; i < expectedTokens.size(); ++i) {
        // This check now compares the full Token (type, lexeme, and loc)
        CHECK(tokens[i] == expectedTokens[i]);
    }
}