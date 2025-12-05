#include <fstream>

#include "Lexer.h"
#include "core/U8String.h"
#include "utf8cpp/utf8.h"

using it = utf8::iterator<std::u8string::const_iterator>;

int main() {
    std::ifstream file("test/TestCode/0_prototype_and_comments/Test_0_1.txt");
    std::string code;
    std::string line;
    while (getline(file, line)) {
        code.append(line);
        code.append("\n");
    }
    code.append("\n0");
    for (auto t : code) {
        std::cout << std::hex << (int) (t) << ", ";
    }
    std::cout << code << "\n";
    std::cout << U8String(code) << "\n Test3";

    Lexer lexer = Lexer(U8String(code));
    std::vector<Token> tokens = lexer.tokenize();
    for (Token t : tokens) {
        std::cout << t << "\n";
    }
    return 0;
}