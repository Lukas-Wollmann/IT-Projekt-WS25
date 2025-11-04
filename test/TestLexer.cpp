#include "Doctest.h"
#include "../src/Lexer.h"

static void checkToken(const Token &t, TokenType type, const std::string &lexeme) {
    CHECK(t.type == type);
    CHECK(t.lexeme == lexeme);
}

TEST_CASE("Lexer: single plus operator") {
    Lexer lexer("+");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, "+");
}

TEST_CASE("Lexer: single minus operator") {
    Lexer lexer("-");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, "-");
}

TEST_CASE("Lexer: single multiply operator") {
    Lexer lexer("*");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, "*");
}

TEST_CASE("Lexer: single divide operator") {
    Lexer lexer("/");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, "/");
}

TEST_CASE("Lexer: parentheses") {
    Lexer lexer("( )");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 2);
    checkToken(tokens[0], TokenType::SEPARATOR, "(");
    checkToken(tokens[1], TokenType::SEPARATOR, ")");
}

TEST_CASE("Lexer: curly braces") {
    Lexer lexer("{ }");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 2);
    checkToken(tokens[0], TokenType::SEPARATOR, "{");
    checkToken(tokens[1], TokenType::SEPARATOR, "}");
}

TEST_CASE("Lexer: square brackets") {
    Lexer lexer("[ ]");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 2);
    checkToken(tokens[0], TokenType::SEPARATOR, "[");
    checkToken(tokens[1], TokenType::SEPARATOR, "]");
}

TEST_CASE("Lexer: single separator") {
    Lexer lexer(";");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::SEPARATOR, ";");
}

TEST_CASE("Lexer: comma separator") {
    Lexer lexer(",");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::SEPARATOR, ",");
}

TEST_CASE("Lexer: colon separator") {
    Lexer lexer(":");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::SEPARATOR, ":");
}

TEST_CASE("Lexer: comparison operator") {
    Lexer lexer(">");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, ">");
}

TEST_CASE("Lexer: less than operator") {
    Lexer lexer("<");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, "<");
}

TEST_CASE("Lexer: double equals") {
    Lexer lexer("==");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, "==");
}

TEST_CASE("Lexer: not equals") {
    Lexer lexer("!=");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::OPERATOR, "!=");
}

TEST_CASE("Lexer: longer number") {
    Lexer lexer("42069");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::NUMERIC_LITERAL, "42069");
}

TEST_CASE("Lexer: zero") {
    Lexer lexer("0");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::NUMERIC_LITERAL, "0");
}

TEST_CASE("Lexer: return keyword") {
    Lexer lexer("return");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::KEYWORD, "return");
}

TEST_CASE("Lexer: func keyword") {
    Lexer lexer("func");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::KEYWORD, "func");
}

TEST_CASE("Lexer: all type keywords") {
    Lexer lexer("i32 u32 f32");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 3);
    checkToken(tokens[0], TokenType::KEYWORD, "i32");
    checkToken(tokens[1], TokenType::KEYWORD, "u32");
    checkToken(tokens[2], TokenType::KEYWORD, "f32");
}

TEST_CASE("Lexer: single identifier") {
    Lexer lexer("hello");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::IDENTIFIER, "hello");
}

TEST_CASE("Lexer: single keyword") {
    Lexer lexer("if");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);
    checkToken(tokens[0], TokenType::KEYWORD, "if");
}

TEST_CASE("Lexer: two numbers with operator") {
    Lexer lexer("1 + 2");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 3);
    checkToken(tokens[0], TokenType::NUMERIC_LITERAL, "1");
    checkToken(tokens[1], TokenType::OPERATOR, "+");
    checkToken(tokens[2], TokenType::NUMERIC_LITERAL, "2");
}

TEST_CASE("Lexer: simple assignment") {
    Lexer lexer("x = 5");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 3);
    checkToken(tokens[0], TokenType::IDENTIFIER, "x");
    checkToken(tokens[1], TokenType::OPERATOR, "=");
    checkToken(tokens[2], TokenType::NUMERIC_LITERAL, "5");
}

TEST_CASE("Lexer: empty string") {
    Lexer lexer("");
    auto tokens = lexer.tokenize();
    
    CHECK(tokens.size() == 0);
}

TEST_CASE("Lexer: whitespace only") {
    Lexer lexer("   \t\n  ");
    auto tokens = lexer.tokenize();
    
    CHECK(tokens.size() == 0);
}

TEST_CASE("Lexer: multiple keywords") {
    Lexer lexer("if else while");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 3);
    checkToken(tokens[0], TokenType::KEYWORD, "if");
    checkToken(tokens[1], TokenType::KEYWORD, "else");
    checkToken(tokens[2], TokenType::KEYWORD, "while");
}

TEST_CASE("Lexer: variable declaration with arithmetic") {
    const std::string src = "result: i32 = 20 + 20;";
    Lexer lexer(src);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 8);

    checkToken(tokens[0], TokenType::IDENTIFIER, "result");
    checkToken(tokens[1], TokenType::SEPARATOR, ":");       // oder OPERATOR, falls ihr ':' so behandelt
    checkToken(tokens[2], TokenType::KEYWORD, "i32");    // oder KEYWORD, falls ihr Typen so klassifiziert
    checkToken(tokens[3], TokenType::OPERATOR, "=");
    checkToken(tokens[4], TokenType::NUMERIC_LITERAL, "20");
    checkToken(tokens[5], TokenType::OPERATOR, "+");
    checkToken(tokens[6], TokenType::NUMERIC_LITERAL, "20");
    checkToken(tokens[7], TokenType::SEPARATOR, ";");
}

#if 0
TEST_CASE("Lexer: pointer type declaration and new") {
    const std::string src = "a: *i32 = new i32(5);";
    Lexer lexer(src);
    auto tokens = lexer.tokenize();

    // Erwartete Tokens: a, :, *, i32, =, new, i32, (, 5, ), ;
    REQUIRE(tokens.size() == 11);

    checkToken(tokens[0], TokenType::IDENTIFIER, "a");
    checkToken(tokens[1], TokenType::SEPARATOR, ":");
    checkToken(tokens[2], TokenType::OPERATOR, "*");        // '*' als Operator
    checkToken(tokens[3], TokenType::IDENTIFIER, "i32");
    checkToken(tokens[4], TokenType::OPERATOR, "=");
    checkToken(tokens[5], TokenType::KEYWORD, "new");       // falls 'new' als Keyword behandelt wird
    checkToken(tokens[6], TokenType::IDENTIFIER, "i32");
    checkToken(tokens[7], TokenType::SEPARATOR, "(");
    checkToken(tokens[8], TokenType::NUMERIC_LITERAL, "5");
    checkToken(tokens[9], TokenType::SEPARATOR, ")");
    checkToken(tokens[10], TokenType::SEPARATOR, ";");
}

TEST_CASE("Lexer: simple if-statement") {
    const std::string src = "if (a > 10) { a = a + 1; }";
    Lexer lexer(src);
    auto tokens = lexer.tokenize();

    // if, (, a, >, 10, ), {, a, =, a, +, 1, ;, }
    REQUIRE(tokens.size() == 14);

    checkToken(tokens[0], TokenType::KEYWORD, "if");
    checkToken(tokens[1], TokenType::SEPARATOR, "(");
    checkToken(tokens[2], TokenType::IDENTIFIER, "a");
    checkToken(tokens[3], TokenType::OPERATOR, ">");
    checkToken(tokens[4], TokenType::NUMERIC_LITERAL, "10");
    checkToken(tokens[5], TokenType::SEPARATOR, ")");
    checkToken(tokens[6], TokenType::SEPARATOR, "{");
    checkToken(tokens[7], TokenType::IDENTIFIER, "a");
    checkToken(tokens[8], TokenType::OPERATOR, "=");
    checkToken(tokens[9], TokenType::IDENTIFIER, "a");
    checkToken(tokens[10], TokenType::OPERATOR, "+");
    checkToken(tokens[11], TokenType::NUMERIC_LITERAL, "1");
    checkToken(tokens[12], TokenType::SEPARATOR, ";");
    checkToken(tokens[13], TokenType::SEPARATOR, "}");
}

TEST_CASE("Lexer: string interpolation literal") {
    const std::string src = "\"TEST: ${a}\"";
    Lexer lexer(src);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens.size() == 1);
    // Annahme: ihr behandelt den ganzen Inhalt als ein STRING_LITERAL (inkl. ${...})
    checkToken(tokens[0], TokenType::STRING_LITERAL, "\"TEST: ${a}\"");
}

/*TEST_CASE("Lexer: comments are ignored") {
    const std::string src = "a: i32 = 5; // This is a comment\nb: i32 = 6;";
    Lexer lexer(src);
    auto tokens = lexer.tokenize();

    // Tokens: a, :, i32, =, 5, ;, b, :, i32, =, 6, ;
    REQUIRE(tokens.size() == 12);

    checkToken(tokens[0], TokenType::IDENTIFIER, "a");
    checkToken(tokens[1], TokenType::SEPARATOR, ":");
    checkToken(tokens[2], TokenType::IDENTIFIER, "i32");
    checkToken(tokens[3], TokenType::OPERATOR, "=");
    checkToken(tokens[4], TokenType::NUMERIC_LITERAL, "5");
    checkToken(tokens[5], TokenType::SEPARATOR, ";");
    checkToken(tokens[6], TokenType::IDENTIFIER, "b");
    checkToken(tokens[7], TokenType::SEPARATOR, ":");
    checkToken(tokens[8], TokenType::IDENTIFIER, "i32");
    checkToken(tokens[9], TokenType::OPERATOR, "=");
    checkToken(tokens[10], TokenType::NUMERIC_LITERAL, "6");
    checkToken(tokens[11], TokenType::SEPARATOR, ";");
}*/

TEST_CASE("Lexer: line and column tracking") {
    const std::string src = "let a = 1;\nlet b = 2;";
    Lexer lexer(src);
    auto tokens = lexer.tokenize();

    // Überprüfe ein paar Positionen: 'let' am Anfang und 'let' der zweiten Zeile
    REQUIRE(tokens.size() >= 6);

    CHECK(tokens[0].loc.line == 1);
    CHECK(tokens[0].loc.column == 1); // 'let' beginnt bei Spalte 1

    // finde das Token 'let' in der zweiten Zeile (erster Token der zweiten Zeile)
    bool foundSecondLet = false;
    for (const auto &t : tokens) {
        if (t.lexeme == "let" && t.loc.line == 2) {
            foundSecondLet = true;
            CHECK(t.loc.column == 1);
            break;
        }
    }
    CHECK(foundSecondLet == true);
}
#endif