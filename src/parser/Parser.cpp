#include "Parser.h"

#include <sstream>
#include <string>

#include "Macros.h"
#include "Typedef.h"
#include "ast/Printer.h"
#include "lexer/Token.h"

using namespace ast;
using namespace type;
using namespace lexer;
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
	while (peek().type != TokenType::EndOfFile) {
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
		VERIFY(t.type != TokenType::Comment);
		return t;
	}
	return Token(TokenType::EndOfFile, u8"", {0, 0, 0});
}

Box<Module> Parser::parse() {
	return Module();
}

Box<Module> Parser::Module() {
	Vec<Box<FuncDecl>> funcDecls = Vec<Box<FuncDecl>>();
	while (peek().type != TokenType::EndOfFile) {
		auto f = FunctionDeclaration();
		if (f.has_value())
			funcDecls.push_back(std::move(f.value()));
		else
			advanceTo(TokenType::Keyword, u8"func");
	}
	return make_unique<ast::Module>(moduleName, std::move(funcDecls));
}

Opt<Box<FuncDecl>> Parser::FunctionDeclaration() {
	if (!consume(TokenType::Keyword, u8"func").has_value())
		return std::nullopt;
	Opt<Token> name = consume(TokenType::Identifier);
	if (!name.has_value())
		return std::nullopt;
	auto params = ParamList();
	if (!params.has_value())
		return std::nullopt;
	Opt<Box<type::Type>> ret = make_unique<UnitType>();
	if (peek() == Token(TokenType::Operator, u8"->")) {
		consume(TokenType::Operator, u8"->");
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
	if (!consume(TokenType::Separator, u8"(").has_value())
		return std::nullopt;
	if (peek() != Token(TokenType::Separator, u8")"))
		while (true) {
			auto name = consume(TokenType::Identifier);
			if (!name.has_value())
				return std::nullopt;
			if (!consume(TokenType::Separator, u8":").has_value())
				return std::nullopt;
			auto t = Type();
			if (!t.has_value())
				return std::nullopt;
			params.push_back({name.value().lexeme, std::move(t.value())});
			if (peek() == Token(TokenType::Separator, u8","))
				consume(TokenType::Separator, u8",");
			else
				break;
		}
	if (!consume(TokenType::Separator, u8")").has_value())
		return std::nullopt;
	return std::move(params);
}

Opt<Box<Type>> Parser::Type() {
	if (peek().type == TokenType::Identifier) {
		auto t = consume(TokenType::Identifier);
		return make_unique<Typename>(t->lexeme);
	}
	if (peek() == Token(TokenType::Separator, u8"(")) {
		consume(TokenType::Separator, u8"(");
		if (peek() == Token(TokenType::Separator, u8")")) {
			consume(TokenType::Separator, u8")");
			return make_unique<UnitType>();
		}
		Vec<Box<type::Type>> tupleTypeList;
		while (true) {
			auto t = Type();
			if (!t.has_value())
				return std::nullopt;
			tupleTypeList.push_back(std::move(t.value()));
			if (peek() == Token(TokenType::Separator, u8","))
				consume(TokenType::Separator, u8",");
			else
				break;
		}
		consume(TokenType::Separator, u8")");
	}
	if (peek() == Token(TokenType::Operator, u8"*")) {
		consume(TokenType::Operator, u8"*");
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		return make_unique<PointerType>(std::move(t.value()));
	}
	if (peek() == Token(TokenType::Separator, u8"[")) {
		consume(TokenType::Separator, u8"[");
		if (peek() != Token(TokenType::Separator, u8"]"))
			return std::nullopt;
		if (!consume(TokenType::Separator, u8"]").has_value())
			return std::nullopt;
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		return make_unique<ArrayType>(std::move(t.value()));
	}
	return std::nullopt;
}

Opt<Box<BlockStmt>> Parser::CodeBlock() {
	if (!consume(TokenType::Separator, u8"{").has_value())
		return std::nullopt;
	Vec<Box<Stmt>> statements;
	while (peek().lexeme != u8"}") {
		auto statement = Statement();
		if (statement.has_value())
			statements.push_back(std::move(statement.value()));
	}
	if (!consume(TokenType::Separator, u8"}").has_value())
		return std::nullopt;
	return make_unique<BlockStmt>(std::move(statements));
}

Opt<Box<ast::Stmt>> Parser::Statement() {
	if (peek().lexeme == u8";") {
		consume(TokenType::Separator, u8";");
		return std::nullopt;
	} else if (peek().lexeme == u8"return") {
		if (!consume(TokenType::Keyword, u8"return").has_value())
			return std::nullopt;
		if (peek() == Token(TokenType::Separator, u8";")) {
			consume(TokenType::Separator, u8";");
			return make_unique<ReturnStmt>();
		} else {
			auto expr = Expression();
			if (!expr.has_value())
				return std::nullopt;
			consume(TokenType::Separator, u8";");
			return make_unique<ReturnStmt>(std::move(expr.value()));
		}
	} else if (peek().lexeme == u8"{") {
		return CodeBlock();
	} else if (peek().lexeme == u8"if") {
		return IfBlock();
	} else if (peek().lexeme == u8"while") {
		return Loop();
	} else {
		if (peek().type == TokenType::Identifier)
			if (peek(1) == Token(TokenType::Separator, u8":"))
				return Declaration();
		return Expression();
	}
}

Opt<Box<ast::WhileStmt>> Parser::Loop() {
	consume(TokenType::Keyword, u8"while");
	consume(TokenType::Separator, u8"(");
	auto cond = Expression();
	if (!cond.has_value())
		return std::nullopt;
	consume(TokenType::Separator, u8")");
	auto body = CodeBlock();
	if (!body.has_value())
		return std::nullopt;
	return make_unique<WhileStmt>(std::move(cond.value()), std::move(body.value()));
}

Opt<Box<ast::IfStmt>> Parser::IfBlock() {
	consume(TokenType::Keyword, u8"if");
	consume(TokenType::Separator, u8"(");
	auto cond = Expression();
	if (!cond.has_value())
		return std::nullopt;
	consume(TokenType::Separator, u8")");
	auto body = CodeBlock();
	if (!body.has_value())
		return std::nullopt;
	if (peek() == Token(TokenType::Keyword, u8"else"))
		consume(TokenType::Keyword, u8"else");
	Opt<Box<Stmt>> elseBody;

	if (peek() == Token(TokenType::Keyword, u8"if")) {
		auto elseIf_ = IfBlock();

		if (!elseIf_.has_value())
			return std::nullopt;

		Vec<Box<Stmt>> body;
		body.push_back(std::move(elseIf_.value()));

		return make_unique<IfStmt>(std::move(cond.value()), make_unique<BlockStmt>(std::move(body)),
								   make_unique<BlockStmt>(Vec<Box<Stmt>>{}));
	} else {
		auto else_ = CodeBlock();

		if (else_.has_value())
			return std::nullopt;

		return make_unique<IfStmt>(std::move(cond.value()), std::move(body.value()),
								   std::move(else_.value()));
	}
}

Opt<Box<ast::VarDef>> Parser::Declaration() {
	auto id = consume(TokenType::Identifier);
	if (!id.has_value())
		return std::nullopt;
	consume(TokenType::Separator, u8":");
	auto type = Type();
	if (!type.has_value())
		return std::nullopt;
	consume(TokenType::Operator, u8"=");
	auto expr = Expression();
	if (!expr.has_value())
		return std::nullopt;
	consume(TokenType::Separator, u8";");
	return make_unique<VarDef>(id.value().lexeme, std::move(type.value()), std::move(expr.value()));
}

Vec<Box<Expr>> Parser::ExpressionList() {
	Vec<Box<Expr>> exprs;
	if (peek() == Token(TokenType::Separator, u8")") ||
		peek() == Token(TokenType::Separator, u8"}"))
		return exprs;
	while (true) {
		auto expr = Expression();
		if (!expr.has_value())
			break;
		exprs.push_back(std::move(expr.value()));
		if (peek() == Token(TokenType::Separator, u8")") ||
			peek() == Token(TokenType::Separator, u8"}"))
			break;
		consume(TokenType::Separator, u8",");
	}
	return exprs;
}

Opt<Box<ast::Expr>> Parser::Expression() { // TODO implement binary Operators somewhere idk where
	return AssignmentExpression();
}

Opt<Box<ast::Expr>> Parser::AssignmentExpression() {
	auto lhs = EqualityExpression();
	if (!lhs.has_value())
		return std::nullopt;
	if (peek().type == TokenType::Operator) {
		using ast::AssignmentKind;
		AssignmentKind opKind;
		if (peek().lexeme == u8"=")
			opKind = AssignmentKind::Simple;
		else if (peek().lexeme == u8"+=")
			opKind = AssignmentKind::Addition;
		else if (peek().lexeme == u8"-=")
			opKind = AssignmentKind::Subtraction;
		else if (peek().lexeme == u8"*=")
			opKind = AssignmentKind::Multiplication;
		else if (peek().lexeme == u8"/=")
			opKind = AssignmentKind::Division;
		else if (peek().lexeme == u8"%=")
			opKind = AssignmentKind::Modulo;
		else if (peek().lexeme == u8"&=")
			opKind = AssignmentKind::BitwiseAnd;
		else if (peek().lexeme == u8"|=")
			opKind = AssignmentKind::BitwiseOr;
		else if (peek().lexeme == u8"^=")
			opKind = AssignmentKind::BitwiseXor;
		else if (peek().lexeme == u8"<<=")
			opKind = AssignmentKind::LeftShift;
		else if (peek().lexeme == u8">>=")
			opKind = AssignmentKind::RightShift;
		else {
			errors.push_back("Unexpected operator at end of AssignmentExpression");
			return lhs;
		}
		auto op = consume(TokenType::Operator);
		if (!op.has_value())
			return std::nullopt;
		auto rhs = AssignmentExpression();
		if (!rhs.has_value())
			return std::nullopt;
		return make_unique<Assignment>(opKind, std::move(lhs.value()), std::move(rhs.value()));
	}
	return lhs;
}

Opt<Box<ast::Expr>> Parser::EqualityExpression() {
	auto lhs = RelationalExpression();
	if (!lhs.has_value())
		return std::nullopt;
	while (true) {
		if (peek().type == TokenType::Operator &&
			(peek().lexeme == u8"==" || peek().lexeme == u8"!=")) {
			auto op = consume(TokenType::Operator);
			if (!op.has_value())
				return std::nullopt;
			auto rhs = RelationalExpression();
			if (!rhs.has_value())
				return std::nullopt;
			auto tmp =
					make_unique<BinaryExpr>(op.value().lexeme == u8"==" ? BinaryOpKind::Equality
																		: BinaryOpKind::Inequality,
											std::move(lhs.value()), std::move(rhs.value()));
			lhs = std::move(tmp);
		} else
			break;
	}
	return lhs;
}

Opt<Box<ast::Expr>> Parser::RelationalExpression() {
	auto lhs = AdditiveExpression();
	if (!lhs.has_value())
		return std::nullopt;
	while (true) {
		if (peek().type == TokenType::Operator &&
			(peek().lexeme == u8"<" || peek().lexeme == u8">" || peek().lexeme == u8"<=" ||
			 peek().lexeme == u8">=")) {
			auto op = consume(TokenType::Operator);
			if (!op.has_value())
				return std::nullopt;
			BinaryOpKind opKind;
			if (op.value().lexeme == u8"<")
				opKind = BinaryOpKind::LessThan;
			else if (op.value().lexeme == u8">")
				opKind = BinaryOpKind::GreaterThan;
			else if (op.value().lexeme == u8"<=")
				opKind = BinaryOpKind::LessThanOrEqual;
			else if (op.value().lexeme == u8">=")
				opKind = BinaryOpKind::GreaterThanOrEqual;
			auto rhs = AdditiveExpression();
			if (!rhs.has_value())
				return std::nullopt;
			auto tmp =
					make_unique<BinaryExpr>(opKind, std::move(lhs.value()), std::move(rhs.value()));
			lhs = std::move(tmp);
		} else
			break;
	}
	return lhs;
}

Opt<Box<ast::Expr>> Parser::AdditiveExpression() {
	auto lhs = MultiplicativeExpression();
	if (!lhs.has_value())
		return std::nullopt;
	while (true) {
		if (peek().type == TokenType::Operator &&
			(peek().lexeme == u8"+" || peek().lexeme == u8"-")) {
			auto op = consume(TokenType::Operator);
			if (!op.has_value())
				return std::nullopt;
			auto rhs = MultiplicativeExpression();
			if (!rhs.has_value())
				return std::nullopt;
			auto tmp =
					make_unique<BinaryExpr>(op.value().lexeme == u8"+" ? BinaryOpKind::Addition
																	   : BinaryOpKind::Subtraction,
											std::move(lhs.value()), std::move(rhs.value()));
			lhs = std::move(tmp);
		} else
			break;
	}
	return lhs;
}

Opt<Box<ast::Expr>> Parser::MultiplicativeExpression() {
	auto lhs = UnaryExpression();
	if (!lhs.has_value())
		return std::nullopt;
	while (true) {
		if (peek().type == TokenType::Operator &&
			(peek().lexeme == u8"*" || peek().lexeme == u8"/" || peek().lexeme == u8"%")) {
			auto op = consume(TokenType::Operator);
			if (!op.has_value())
				return std::nullopt;
			BinaryOpKind opKind;
			if (op.value().lexeme == u8"*")
				opKind = BinaryOpKind::Multiplication;
			else if (op.value().lexeme == u8"/")
				opKind = BinaryOpKind::Division;
			else if (op.value().lexeme == u8"%")
				opKind = BinaryOpKind::Modulo;
			auto rhs = UnaryExpression();
			if (!rhs.has_value())
				return std::nullopt;
			auto tmp =
					make_unique<BinaryExpr>(opKind, std::move(lhs.value()), std::move(rhs.value()));
			lhs = std::move(tmp);
		} else
			break;
	}
	return lhs;
}

Opt<Box<ast::Expr>> Parser::UnaryExpression() {
	if (peek().type == TokenType::Operator) {
		auto op = consume(TokenType::Operator);
		UnaryOpKind opKind;
		if (op.value().lexeme == u8"+")
			opKind = UnaryOpKind::Positive;
		else if (op.value().lexeme == u8"-")
			opKind = UnaryOpKind::Negative;
		else if (op.value().lexeme == u8"!")
			opKind = UnaryOpKind::LogicalNot;
		else if (op.value().lexeme == u8"~")
			opKind = UnaryOpKind::BitwiseNot;
		else if (op.value().lexeme == u8"*")
			opKind = UnaryOpKind::Dereference;
		else {
			std::stringstream ss;
			ss << "expected Unary Operator, got:" << op.value();
			errors.push_back(ss.str());
			return std::nullopt;
		}
		auto expr = UnaryExpression();
		if (!expr.has_value())
			return std::nullopt;
		return make_unique<UnaryExpr>(opKind, std::move(expr.value()));
	}
	return PostfixExpression();
}

Opt<Box<ast::Expr>> Parser::PostfixExpression() {
	auto lhs = PrimaryExpression();
	if (!lhs.has_value())
		return std::nullopt;
	while (true) {
		if (peek().type == TokenType::Separator &&
			(peek().lexeme == u8"(" || peek().lexeme == u8"[")) {
			auto op = consume(TokenType::Separator);
			if (!op.has_value())
				return std::nullopt;
			if (op.value().lexeme == u8"(") {
				auto args = ExpressionList();
				consume(TokenType::Separator, u8")");
				auto tmp = make_unique<FuncCall>(std::move(lhs.value()), std::move(args));
				lhs = std::move(tmp);
			} else {
				auto index = Expression();
				auto op = consume(TokenType::Operator, u8"]");
				auto tmp = make_unique<BinaryExpr>(BinaryOpKind::Index, std::move(lhs.value()),
												   std::move(index.value()));
				lhs = std::move(tmp);
			}

		} else
			break;
	}
	return lhs;
}

Opt<Box<ast::Expr>> Parser::PrimaryExpression() {
	if (peek().type == TokenType::Identifier)
		return make_unique<VarRef>(consume(TokenType::Identifier).value().lexeme);
	if (peek().type == TokenType::BoolLiteral)
		return make_unique<BoolLit>(consume(TokenType::BoolLiteral).value().lexeme == u8"true");
	if (peek().type == TokenType::CharLiteral)
		return make_unique<CharLit>(consume(TokenType::CharLiteral).value().lexeme[0]);
	if (peek().type == TokenType::StringLiteral)
		return make_unique<StringLit>(consume(TokenType::StringLiteral).value().lexeme);
	if (peek().type == TokenType::IntLiteral)
		return make_unique<IntLit>(std::stoi(reinterpret_cast<const char *>(
				consume(TokenType::IntLiteral).value().lexeme.ptr())));
	if (peek().lexeme == u8"(") {
		consume(TokenType::Separator, u8"(");
		auto expr = Expression();
		if (!expr.has_value())
			return std::nullopt;
		consume(TokenType::Separator, u8")");
		return expr;
	}
	if (peek().lexeme == u8"new") {
		consume(TokenType::Keyword, u8"new");
		auto expr = Expression();
		if (!expr.has_value())
			return std::nullopt;
		return make_unique<HeapAlloc>(std::move(expr.value()));
	}
	if (peek().lexeme == u8"[") {
		auto type = Type();
		consume(TokenType::Separator, u8"{");
		auto args = ExpressionList();
		consume(TokenType::Separator, u8"}");
		return make_unique<ArrayExpr>(std::move(type.value()), std::move(args));
	}
	std::stringstream ss;
	ss << "unexpected Token:" << peek();
	index++;
	errors.push_back(ss.str());
	return std::nullopt;
}
