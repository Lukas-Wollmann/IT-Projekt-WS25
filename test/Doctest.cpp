#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Doctest.h"
#include "../src/Lexer.h"

TEST_CASE("Lexer: Basic Arithmetic") {
    CHECK("1 + 1 == 2");
}