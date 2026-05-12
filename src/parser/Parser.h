#pragma once
#include "ast/AST.h"
#include "core/ErrorHandler.h"
#include "core/Operators.h"
#include "lexer/Token.h"
#include "type/TypeFactory.h"

namespace prs {
struct ParsingError : std::exception {
	U8String msg;

	explicit ParsingError(U8String msg);
};

///
/// Parse a vector of tokens into an abstract syntax tree. All functionality is public
/// for testing purposes, do only use the static parse(...) as an interface for this class.
///
struct Parser {
	static Box<ast::Module> parse(const Vec<lex::Token> &tokens, ErrorHandler &err,
								  U8String moduleName);

	using TokenIter = Vec<lex::Token>::const_iterator;

	const Vec<lex::Token> &m_Tokens;
	const U8String m_ModuleName;
	ErrorHandler &m_ErrorHandler;
	TokenIter m_Current;

	Parser(const Vec<lex::Token> &tokens, ErrorHandler &err, U8String moduleName);

	[[nodiscard]] const lex::Token &peek() const;
	void advance();
	const lex::Token &consume(lex::TokenType type, U8String lexeme);
	const lex::Token &consume(lex::TokenType type);
	void reportError(ParsingError &e) const;

	Box<ast::Module> parseModule();
	Box<ast::FuncDecl> parseFuncDecl();
	Box<ast::StructDecl> parseStructDecl();
	Vec<ast::Param> parseParamList();
	Type parseType();
	Box<ast::Stmt> parseStmt();
	Box<ast::BlockStmt> parseBlockStmt();
	Box<ast::WhileStmt> parseWhileStmt();
	Box<ast::IfStmt> parseIfStmt();
	Box<ast::VarDef> parseVarDef();
	Box<ast::Expr> parseExpr();
	Box<ast::Expr> parseAssignmentExpr();
	Box<ast::Expr> parseLogicalOrExpr();
	Box<ast::Expr> parseLogicalAndExpr();
	Box<ast::Expr> parseBitwiseOrExpr();
	Box<ast::Expr> parseBitwiseXorExpr();
	Box<ast::Expr> parseBitwiseAndExpr();
	Box<ast::Expr> parseEqualityExpr();
	Box<ast::Expr> parseRelationalExpr();
	Box<ast::Expr> parseAdditiveExpr();
	Box<ast::Expr> parseMultiplicativeExpr();
	Box<ast::Expr> parseUnaryExpr();
	Box<ast::Expr> parseIndexExpr();
	Box<ast::Expr> parsePostfixExpr();
	Box<ast::Expr> parsePrimaryExpr();
	Box<ast::Expr> parseFieldAccess(Box<ast::Expr> base);
	Vec<Box<ast::Expr>> parseExprList();
	Vec<Box<ast::Expr>> parseBraceExprList();

	static AssignmentKind getAssignmentKindFromString(const U8String &str);
};
}