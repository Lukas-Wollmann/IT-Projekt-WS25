#include "core/U8String.h"
#include <utf8cpp/utf8.h>
#include <sstream>
#include <iostream>
#include "Token.h"
#include "Lexer.h"


int main() 
{
    U8String sourceCode = u8R"(
    func main() i32 {
        i32 x = 42;
        x: i32 =="hallo";
        h: char = 'a';
        n: char = '🥸';
        🥸🥸🥸🥸🥸🥸🥸🥸v: char = '\n';
        k: char = '\x'; // illegal escape
        s: char = 'ab'; // illegal multi-char
        /* This is a block comment
           spanning multiple lines */
        /* Unterminated block comment
        /* Oneline block comment */
        t: char = ' ; // illegal missing closing
        string y = "Hello, World!";
        return x;
    }
    )";

    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "Tokens:\n";
    for (const auto &token : tokens) {
        std::cout << token << "\n";
    }
}