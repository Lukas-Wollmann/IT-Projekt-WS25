#include <fstream>

#include "ast/PrintVisitor.h"
#include "core/U8String.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "utf8cpp/utf8.h"

using it = utf8::iterator<std::u8string::const_iterator>;

int main() {
	U8String s = u8"func add(i: i32) -> i32 { return true;}";
	Lexer lexer(std::move(s));

	std::vector<Token> tokens = lexer.tokenize();
	for (Token t : tokens) {
		std::cout << t << "\n";
	}
	Parser p(tokens, "Test");
	auto tree = p.parse();
	std::cout << *tree << std::endl;
	return 0;
}