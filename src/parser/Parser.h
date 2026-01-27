#pragma once
#include "ast/AST.h"
#include "core/ErrorHandler.h"
#include "core/Operators.h"
#include "lexer/Token.h"
#include "type/Type.h"

namespace parser {
	struct ParsingError {
		U8String message;
	};

	struct Parser {
		using TokenIter = Vec<lexer::Token>::const_iterator;

		const Vec<lexer::Token> &m_Tokens;
		const U8String m_ModuleName;
		ErrorHandler &m_ErrorHandler;
		TokenIter m_Current;

		Parser(const Vec<lexer::Token> &tokens, ErrorHandler &err, U8String moduleName);

		static Box<ast::Module> parse(const Vec<lexer::Token> &tokens, ErrorHandler &err,
									  U8String moduleName);

		const lexer::Token &peek() const;
		void advance();
		const lexer::Token &consume(lexer::TokenType type, U8String lexeme);
		const lexer::Token &consume(lexer::TokenType type);
		void reportError(const ParsingError &e);
		void advanceToNext(lexer::TokenType type, U8String lexeme);

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

		AssignmentKind getAssignmentKindFromString(const U8String &str) const;
	};
}