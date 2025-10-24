#include <sstream>
#include <iostream>
#include "Token.h"
#include "AST.h"
#include "PrinterVisitor.h"


int main() 
{
	StatementList statements;
	statements.push_back(std::make_unique<IntegerLiteral>(10));
	statements.push_back(std::make_unique<StringLiteral>("TEST"));
	statements.push_back(std::make_unique<DoubleLiteral>(10.0));
	statements.push_back(std::make_unique<VariableUse>("var"));
	auto block = std::make_unique<CodeBlock>(std::move(statements));
    
    std::cout << *block << std::endl;

	// You can create a Token like that:
	const Token t = { TokenType::STRING_LITERAL, "\"Hello World!\"", 1, 1, 0 };
	
	std::cout << t << std::endl;


	// You can create an AST Node like this. Look inside the IntegerLiteral 
	// constructor to see what you need to put into make_unique.
	std::unique_ptr<const Node> intNode = std::make_unique<const IntegerLiteral>(100);	

	// A lot of times the constructor of your node takes an r-value reference
	//
	// 		StringLiteral(std::string &&value);
	//								   ^
	//							       L__ this wants an r-value reference
	//
	// There you should std::move the value into the make_unique.
	
	std::string text = "This is some Text!";
	std::unique_ptr<const Node> strNode1 = std::make_unique<const StringLiteral>(std::move(text));


	// If you put a literal object into the make_unique, then you 
	// should not move the value. A literal object already is a r-value.
	
	std::unique_ptr<const Node> strNode2 = std::make_unique<const StringLiteral>(std::string("Don't move me!"));
	//													     This is an r-value <~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	// If you want to relocate the unique_ptr, you canno't copy it - you have to
	// move it. Its because only one unique_ptr can own the pointer at a time:
	std::unique_ptr<const Node> strNodeMoved = std::move(strNode1);

	// Using strNode after it has been moved is undefined behavior and should
	// never be done. Some IDEs will warn you - some won't:
	
	// strNode->toString(std::cout);		- DON'T DO THIS
	
	std::cout << *strNodeMoved << std::endl;
}