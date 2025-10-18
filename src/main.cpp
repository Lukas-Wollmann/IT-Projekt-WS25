#include "Token.h"

int main() {
	const Token t = {TokenType::STRING_LITERAL, "\"Hello World!\"", 1, 1, 0};
	std::cout << t << std::endl;
}
