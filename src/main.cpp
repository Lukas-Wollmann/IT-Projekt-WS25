#include <sstream>
#include <iostream>
#include "Token.h"
#include "ast/AST.h"
#include "ast/PrinterVisitor.h"
#include "ast/CodeGenVisitor.h"


int main() 
{
	// You can create a Token like that:
	const Token t = { TokenType::STRING_LITERAL, "\"Hello World!\"", 1, 1, 0 };	
	std::cout << t << std::endl;

    ParameterList params;
    params.push_back(Parameter("a", std::make_unique<ValueType>("i32")));
    params.push_back(Parameter("b", std::make_unique<ValueType>("i32")));

    StatementList statements;

    FunctionDeclaration func
    {
        "add", 
        std::move(params), 
        std::make_unique<ValueType>("i32"), 
        std::make_unique<CodeBlock>(std::move(statements))
    };

    CodeGenVisitor generator(std::cout);

    generator.visit(func);
}