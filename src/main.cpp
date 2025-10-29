#include <sstream>
#include <fstream>
#include <iostream>
#include "Token.h"
#include "ast/AST.h"
#include "ast/PrinterVisitor.h"

int main() 
{
	// You can create a Token like that:
	const Token t = { TokenType::STRING_LITERAL, "\"Hello World!\"", 1, 1, 0 };	
	std::cout << t << std::endl;

    ParameterList params;
    params.push_back(Parameter("a", std::make_unique<ValueType>("i32")));
    params.push_back(Parameter("b", std::make_unique<ValueType>("i32")));

    std::unique_ptr<Expression> expression = std::make_unique<IntegerLiteral>(42);
    std::unique_ptr<ReturnStatement> returnStatement = std::make_unique<ReturnStatement>(std::move(expression));

    StatementList statements;
    statements.push_back(std::move(returnStatement));

    FunctionDeclaration func
    {
        "main", 
        std::move(params), 
        std::make_unique<ValueType>("i32"), 
        std::make_unique<CodeBlock>(std::move(statements))
    };

    std::cout << func;
}