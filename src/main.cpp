#include <sstream>
#include <iostream>
#include "Token.h"
#include "Lexer.h"


int main() 
{
    std::string sourceCode = R"(
    func main() i32 {
        i32 x = 42;
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