#include <fstream>

#include "lexer/Lexer.h"
#include "core/U8String.h"
#include "utf8cpp/utf8.h"

using it = utf8::iterator<std::u8string::const_iterator>;

int main() {
    U8String s = u8"func add() -> i32 {}";
    Lexer lexer(std::move(s));

    std::vector<Token> tokens = lexer.tokenize();
    for (Token t : tokens) {
        std::cout << t << "\n";
    }
    
    return 0;
}