#pragma once
#include "ast/AST.h"
#include "lexer/Token.h"
#include "type/Type.h"

class Parser {
public:
	const Vec<Token> &tokens;
	Vec<std::string> errors;
	size_t index;
	U8String moduleName;

	Parser(const Vec<Token> &tokens, std::string moduleName)
		: tokens(tokens)
		, index(0)
		, moduleName(std::move(moduleName)) {}

	Box<ast::Module> parse();

private:
	Opt<Token> consume(TokenType type, Opt<U8String> string = std::nullopt);
	void advanceTo(TokenType type, Opt<U8String> string = std::nullopt);
	Token peek(u8 look_ahead = 0) const;

	Box<ast::Module> Module();
	Opt<Box<ast::FuncDecl>> FunctionDeclaration();
	Opt<Vec<ast::Param>> ParamList();
	Opt<Box<type::Type>> Type();
	Opt<Box<ast::BlockStmt>> CodeBlock();
	Opt<Box<ast::Stmt>> Statement();
	Opt<Box<ast::WhileStmt>> Loop();
	Opt<Box<ast::IfStmt>> IfBlock();
	Opt<Box<ast::VarDef>> Declaration();
	Opt<Box<ast::Expr>> Expression();
	Vec<Box<ast::Expr>> ExpressionList();
	Opt<Box<ast::FuncCall>> FunctionCall();
};