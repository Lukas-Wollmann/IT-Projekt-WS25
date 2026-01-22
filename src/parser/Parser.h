#pragma once
#include "ast/AST.h"
#include "lexer/Token.h"
#include "type/Type.h"

namespace parser {
	struct ParsingError {
		U8String message;
	};

	struct Parser {
		using TokenIter = Vec<lexer::Token>::const_iterator;
		using ParsingRes = Pair<Box<ast::Module>, Vec<U8String>>;

		const Vec<lexer::Token> &m_Tokens;
		const U8String m_ModuleName;
		Vec<U8String> m_Errors;
		TokenIter m_Current;

		Parser(const Vec<lexer::Token> &tokens, U8String moduleName);

		static ParsingRes parse(const Vec<lexer::Token> &tokens, U8String moduleName);

		const lexer::Token &peek() const;
		void advance();
		lexer::Token consume(lexer::TokenType type, U8String lexeme);
		lexer::Token consume(lexer::TokenType type);
		void reportError(const ParsingError &e);
		void advanceToNext(lexer::TokenType type, Opt<U8String> lexeme = {});

		Box<ast::Module> parseModule();
		Box<ast::FuncDecl> parseFuncDecl();
		Vec<ast::Param> parseParamList();
		type::TypePtr parseType();
		Box<ast::Stmt> parseStmt();
		Box<ast::BlockStmt> parseBlockStmt();
		Box<ast::WhileStmt> parseWhileStmt();
		Box<ast::IfStmt> parseIfStmt();
		Box<ast::VarDef> parseVarDef();
		Box<ast::Expr> parseExpr();
		Box<ast::Expr> parseAssignmentExpr();
		Box<ast::Expr> parseEqualityExpr();
		Box<ast::Expr> parseRelationalExpr();
		Box<ast::Expr> parseAdditiveExpr();
		Box<ast::Expr> parseMultiplicativeExpr();
		Box<ast::Expr> parseUnaryExpr();
		Box<ast::Expr> parsePostfixExpr();
		Box<ast::Expr> parsePrimaryExpr();
		Vec<Box<ast::Expr>> parseExprList();

		ast::AssignmentKind getAssignmentKindFromString(const U8String &str) const;

		/*
		void advanceTo(lexer::TokenType type, Opt<U8String> string = std::nullopt);
			Box<ast::Module> Module();
			Opt<Box<ast::FuncDecl>> FunctionDeclaration();
			Opt<Box<type::Type>> Type();
			Opt<Box<ast::BlockStmt>> CodeBlock();
			Opt<Box<ast::Stmt>> Statement();
			Opt<Box<ast::WhileStmt>> Loop();
			Opt<Box<ast::IfStmt>> IfBlock();
			Opt<Box<ast::VarDef>> Declaration();
			Vec<Box<ast::Expr>> ExpressionList();
			Opt<Box<ast::Expr>> Expression();
			Opt<Box<ast::Expr>> AssignmentExpression();
			Opt<Box<ast::Expr>> EqualityExpression();
			Opt<Box<ast::Expr>> RelationalExpression();
			Opt<Box<ast::Expr>> AdditiveExpression();
			Opt<Box<ast::Expr>> MultiplicativeExpression();
			Opt<Box<ast::Expr>> UnaryExpression();
			Opt<Box<ast::Expr>> PostfixExpression();
			Opt<Box<ast::Expr>> PrimaryExpression();
			*/
	};
}