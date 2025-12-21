#include <fstream>

#include "PrintUtil.h"
#include "ast/PrintVisitor.h"
#include "core/U8String.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "utf8cpp/utf8.h"

using it = utf8::iterator<std::u8string::const_iterator>;

int main() {
	U8String s = u8" bla bla bla func add(){ return; } func mul(){ return; } func "
				 u8"bla\t\t\t   /*Hi*/    (){ return; }func div(){ return; }\n\n\n\n";
	Lexer lexer(std::move(s));

	std::vector<Token> tokens = lexer.tokenize();
	for (Token t : tokens) {
		std::cout << t << "\n";
	}
	Parser p(tokens, "Test");
	auto tree = p.parse();
	std::cout << *tree << std::endl;
	std::cout << p.errors << std::endl;
	return 0;
}