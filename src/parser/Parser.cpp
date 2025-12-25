#include "Parser.h"

#include <sstream>
#include <string>

#include "Macros.h"
#include "SyntaxError.h"
#include "Typedef.h"
#include "ast/PrintVisitor.h"
#include "lexer/Token.h"

using namespace ast;
using namespace type;
using std::make_unique;

Opt<Token> Parser::consume(TokenType type, Opt<U8String> string) {
	std::stringstream ss;
	if (index >= tokens.size()) {
		ss << "unexpected end of Token stream, expected" << type;
		errors.push_back(ss.str());
		return std::nullopt;
	}
	Token look_ahead = tokens.at(index);
	if (look_ahead.type != type) {
		ss << "expected: '" << type << "', got: " << look_ahead;
		errors.push_back(ss.str());
		return std::nullopt;
	}
	if (string.has_value())
		if (*string != look_ahead.lexeme) {
			ss << "expected: '" << *string << "', got: " << look_ahead;
			errors.push_back(ss.str());
			return std::nullopt;
		}
	index++;
	return look_ahead;
}

void Parser::advanceTo(TokenType type, Opt<U8String> string) {
	while (peek().type != TokenType::END_OF_FILE) {
		if (peek().type != type)
			index++;
		else if (string.has_value() && peek().lexeme != string.value())
			index++;
		else
			return;
	}
}

Token Parser::peek(u8 look_ahead) const {
	if (index + look_ahead < tokens.size()) {
		Token t = tokens.at(index + look_ahead);
		VERIFY(t.type != TokenType::COMMENT);
		return t;
	}
	return Token(TokenType::END_OF_FILE, u8"", {0, 0, 0});
}

Box<Module> Parser::parse() {
	return Module();
}

Box<Module> Parser::Module() {
	Vec<Box<FuncDecl>> funcDecls = Vec<Box<FuncDecl>>();
	while (peek().type != TokenType::END_OF_FILE) {
		auto f = FunctionDeclaration();
		if (f.has_value())
			funcDecls.push_back(std::move(f.value()));
		else
			advanceTo(TokenType::KEYWORD, u8"func");
	}
	return make_unique<ast::Module>(moduleName, std::move(funcDecls));
}

Opt<Box<FuncDecl>> Parser::FunctionDeclaration() {
	if (!consume(TokenType::KEYWORD, u8"func").has_value())
		return std::nullopt;
	Opt<Token> name = consume(TokenType::IDENTIFIER);
	if (!name.has_value())
		return std::nullopt;
	auto params = ParamList();
	if (!params.has_value())
		return std::nullopt;
	Opt<Box<type::Type>> ret = make_unique<UnitType>();
	if (peek() == Token(TokenType::OPERATOR, u8"->")) {
		consume(TokenType::OPERATOR, u8"->");
		ret = Type();
		if (!ret.has_value())
			return std::nullopt;
	}
	auto codeBlock = CodeBlock();
	if (!codeBlock.has_value())
		return std::nullopt;
	return make_unique<FuncDecl>(name.value().lexeme, std::move(params.value()),
								 std::move(ret.value()), std::move(codeBlock.value()));
}

Opt<Vec<Param>> Parser::ParamList() {
	Vec<Param> params;
	if (!consume(TokenType::SEPARATOR, u8"(").has_value())
		return std::nullopt;
	while (true) {
		auto name = consume(TokenType::IDENTIFIER);
		if (!name.has_value())
			return std::nullopt;
		if (!consume(TokenType::SEPARATOR, u8":").has_value())
			return std::nullopt;
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		params.push_back({name.value().lexeme, std::move(t.value())});
		if (peek() == Token(TokenType::SEPARATOR, u8","))
			consume(TokenType::SEPARATOR, u8",");
		else
			break;
	}
	if (!consume(TokenType::SEPARATOR, u8")").has_value())
		return std::nullopt;
}

Opt<Box<Type>> Parser::Type() {
	if (peek().type == TokenType::IDENTIFIER) {
		auto t = consume(TokenType::IDENTIFIER);
		return make_unique<Typename>(t->lexeme);
	}
	if (peek() == Token(TokenType::SEPARATOR, u8"(")) {
		consume(TokenType::SEPARATOR, u8"(");
		if (peek() == Token(TokenType::SEPARATOR, u8")")) {
			consume(TokenType::SEPARATOR, u8")");
			return make_unique<UnitType>();
		}
		Vec<Box<type::Type>> tupleTypeList;
		while (true) {
			auto t = Type();
			if (!t.has_value())
				return std::nullopt;
			tupleTypeList.push_back(std::move(t.value()));
			if (peek() == Token(TokenType::SEPARATOR, u8","))
				consume(TokenType::SEPARATOR, u8",");
			else
				break;
		}
		consume(TokenType::SEPARATOR, u8")");
	}
	if (peek() == Token(TokenType::OPERATOR, u8"*")) {
		consume(TokenType::OPERATOR, u8"*");
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		return make_unique<PointerType>(std::move(t.value()));
	}
	if (peek() == Token(TokenType::SEPARATOR, u8"[")) {
		consume(TokenType::SEPARATOR, u8"[");
		Opt<size_t> size = std::nullopt;
		if (peek().type == TokenType::NUMERIC_LITERAL)
			size = std::stoi(reinterpret_cast<const char *>(
					consume(TokenType::NUMERIC_LITERAL)->lexeme.ptr()));
		if (peek() != Token(TokenType::SEPARATOR, u8"]"))
			return std::nullopt;
		if (!consume(TokenType::SEPARATOR, u8"]").has_value())
			return std::nullopt;
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		return make_unique<ArrayType>(std::move(t.value()), size);
	}
	return std::nullopt;
}

Opt<Box<BlockStmt>> Parser::CodeBlock() {
	if (!consume(TokenType::SEPARATOR, u8"{").has_value())
		return std::nullopt;
	Vec<Box<Stmt>> statements;
	while (peek().lexeme != u8"}") {
		auto statement = Statement();
		if (statement.has_value())
			statements.push_back(std::move(statement.value()));
	}
	if (!consume(TokenType::SEPARATOR, u8"}").has_value())
		return std::nullopt;
	return make_unique<BlockStmt>(std::move(statements));
}

Opt<Box<ast::Stmt>> Parser::Statement() {
	if (peek().lexeme == u8";") {
		consume(TokenType::SEPARATOR, u8";");
		return std::nullopt;
	} else if (peek().lexeme == u8"return") {
		if (!consume(TokenType::KEYWORD, u8"return").has_value())
			return std::nullopt;
		if (peek() == Token(TokenType::SEPARATOR, u8";")) {
			consume(TokenType::SEPARATOR, u8";");
			return make_unique<ReturnStmt>();
		} else {
			auto expr = Expression();
			if (!expr.has_value())
				return std::nullopt;
			return make_unique<ReturnStmt>(std::move(expr.value()));
		}
	} else if (peek().lexeme == u8"{") {
		return CodeBlock();
	} else if (peek().lexeme == u8"if") {
		return IfBlock();
	} else if (peek().lexeme == u8"while") {
		return Loop();
	} else {
		if (peek().type == TokenType::IDENTIFIER)
			if (peek(1) == Token(TokenType::SEPARATOR, u8":"))
				return Declaration();
		return Expression();
	}
}

Opt<Box<ast::WhileStmt>> Parser::Loop() {
	consume(TokenType::KEYWORD, u8"while");
	consume(TokenType::SEPARATOR, u8"(");
	auto cond = Expression();
	if (!cond.has_value())
		return std::nullopt;
	consume(TokenType::SEPARATOR, u8")");
	auto body = CodeBlock();
	if (!body.has_value())
		return std::nullopt;
	return make_unique<WhileStmt>(std::move(cond.value()), std::move(body.value()));
}

Opt<Box<ast::IfStmt>> Parser::IfBlock() {
	consume(TokenType::KEYWORD, u8"if");
	consume(TokenType::SEPARATOR, u8"(");
	auto cond = Expression();
	if (!cond.has_value())
		return std::nullopt;
	consume(TokenType::SEPARATOR, u8")");
	auto body = CodeBlock();
	if (!body.has_value())
		return std::nullopt;
	if (peek() == Token(TokenType::KEYWORD, u8"else"))
		consume(TokenType::KEYWORD, u8"else");
	Opt<Box<Stmt>> elseBody;
	if (peek() == Token(TokenType::KEYWORD, u8"if"))
		elseBody = IfBlock();
	else
		elseBody = CodeBlock();
	return make_unique<IfStmt>(std::move(cond.value()), std::move(body.value()),
							   std::move(elseBody.value()));
}

Opt<Box<ast::VarDef>> Parser::Declaration() {
	auto id = consume(TokenType::IDENTIFIER);
	if (!id.has_value())
		return std::nullopt;
	consume(TokenType::SEPARATOR, u8":");
	auto type = Type();
	if (!type.has_value())
		return std::nullopt;
	consume(TokenType::OPERATOR, u8"=");
	auto expr = Expression();
	if (!expr.has_value())
		return std::nullopt;
	consume(TokenType::SEPARATOR, u8";");
	return make_unique<VarDef>(id.value().lexeme, type, expr);
}

Opt<Box<ast::Expr>> Parser::Expression() {
	if (peek().type == TokenType::IDENTIFIER) {
		auto id = consume(TokenType::IDENTIFIER);
		return make_unique<VarRef>(id.value().lexeme);
	} else if (peek().type == TokenType::BOOL_LITERAL) {
		auto lit = consume(TokenType::BOOL_LITERAL);
		return make_unique<BoolLit>(lit.value().lexeme == u8"true");
	} else if (peek().type == TokenType::CHAR_LITERAL) {
		auto lit = consume(TokenType::CHAR_LITERAL);
		return make_unique<CharLit>(lit.value().lexeme[0]); // TODO Test
	} else if (peek().type == TokenType::NUMERIC_LITERAL) { // TODO add support for floating point
															// numbers. Needs Issue #43
		auto lit = consume(TokenType::NUMERIC_LITERAL);
		return make_unique<IntLit>(
				std::stoi(reinterpret_cast<const char *>(lit.value().lexeme.ptr())));
	} else if (peek().type == TokenType::STRING_LITERAL) {
		auto lit = consume(TokenType::STRING_LITERAL);
		return make_unique<StringLit>(lit.value().lexeme);
	} else if (peek() == Token(TokenType::SEPARATOR, u8"(")) {
		consume(TokenType::SEPARATOR, u8"(");
		auto expr = Expression();
		if (!expr.has_value())
			return std::nullopt;
		consume(TokenType::SEPARATOR, u8")");
		return std::move(expr);
	} else if (peek() == Token(TokenType::KEYWORD, u8"new")) {
		consume(TokenType::KEYWORD, u8"new");
		auto type = Type();
		if (!type.has_value())
			return std::nullopt;
		consume(TokenType::SEPARATOR, u8"(");
		Vec<Box<Expr>> args;
		if (peek() == Token(TokenType::SEPARATOR, u8")"))
			consume(TokenType::SEPARATOR, u8")");
		else {
			args = ExpressionList();
			consume(TokenType::SEPARATOR, u8")");
		}
		return make_unique<Instantiation>(std::move(type), std::move(args));
	} else if (peek() == Token(TokenType::SEPARATOR, u8"[")) {
		auto type = Type();
		if (!type.has_value())
			return std::nullopt;
		consume(TokenType::SEPARATOR, u8"{");
		auto exprs = ExpressionList();
		consume(TokenType::SEPARATOR, u8"}");
		return make_unique<ArrayExpr>(std::move(type), std::move(exprs));
	} else if (peek().type == TokenType::OPERATOR) {
		auto op = consume(TokenType::OPERATOR);
		UnaryOpKind opKind;
		if (op.value().lexeme == u8"!")
			opKind = UnaryOpKind::LogicalNot;
		else if (op.value().lexeme == u8"~")
			opKind = UnaryOpKind::BitwiseNot;
		else if (op.value().lexeme == u8"+")
			opKind = UnaryOpKind::Positive;
		else if (op.value().lexeme == u8"-")
			opKind = UnaryOpKind::Negative;
		else if (op.value().lexeme == u8"*")
			opKind = UnaryOpKind::Dereference;
		else
			return std::nullopt;
		auto expr = Expression();
		if (!expr.has_value())
			return std::nullopt;
		return make_unique<UnaryExpr>(opKind, std::move(expr));
	}
}

Vec<Box<Expr>> Parser::ExpressionList() {
	Vec<Box<Expr>> exprs;
	while (true) {
		if (peek() == Token(TokenType::SEPARATOR, u8")") ||
			peek() == Token(TokenType::SEPARATOR, u8"}"))
			break;
		auto expr = Expression();
		if (!expr.has_value())
			break;
		exprs.push_back(std::move(expr.value()));
	}
	return std::move(exprs);
}

Opt<Box<FuncCall>> Parser::FunctionCall() {
	auto expr = Expression();
	if (!expr.has_value())
		return std::nullopt;
	consume(TokenType::SEPARATOR, u8"(");
	auto args = ExpressionList();
	consume(TokenType::SEPARATOR, u8")");
	return make_unique<FuncCall>(std::move(expr.value()), std::move(args));
}