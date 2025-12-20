#include "Parser.h"

#include <sstream>

#include "Macros.h"
#include "SyntaxError.h"
#include "Typedef.h"
#include "lexer/Token.h"

using namespace ast;
using namespace type;
using std::make_unique;

Box<Module> Parser::parse() {
	return File();
}

Opt<Token> Parser::consume(TokenType type, Opt<U8String> string) {
	std::stringstream ss;
	if (index >= tokens.size()) {
		ss << "unexpected end of Token stream, expected" << type;
		errors.push_back(ss.str());
		return std::nullopt;
	}
	Token look_ahead = tokens.at(index++);
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
	return look_ahead;
}

void Parser::advance() {
	VERIFY(index < tokens.size() - 1);
	index++;
}

Token Parser::peek() const {
	if (index < tokens.size())
		return tokens.at(index);
	return Token(TokenType::END_OF_FILE, u8"", {0, 0, 0});
}

Box<Module> Parser::File() {
	Vec<Box<FuncDecl>> funcDecls = Vec<Box<FuncDecl>>();
	while (peek().type != TokenType::END_OF_FILE) {
		auto f = FunctionDeclaration();
		if (f.has_value())
			funcDecls.push_back(std::move(*f));
		else
			while (peek().type != TokenType::END_OF_FILE && peek().lexeme != u8"func")
				advance();
	}
	return make_unique<Module>(moduleName, std::move(funcDecls));
}

Opt<Box<FuncDecl>> Parser::FunctionDeclaration() {
	if (!consume(TokenType::KEYWORD, u8"func").has_value())
		return std::nullopt;
	Opt<Token> name = consume(TokenType::IDENTIFIER);
	if (!name.has_value())
		return std::nullopt;
	if (!consume(TokenType::KEYWORD, u8"(").has_value())
		return std::nullopt;
	Opt<Vec<Param>> params = Vec<Param>();
	if (!consume(TokenType::KEYWORD, u8")").has_value()) {
		params = ParamList();
		if (!params.has_value())
			return std::nullopt;
		if (!consume(TokenType::KEYWORD, u8")").has_value())
			return std::nullopt;
	}
	Opt<Box<type::Type>> ret = make_unique<UnitType>();
	if (consume(TokenType::OPERATOR, u8"->").has_value()) {
		if (consume(TokenType::SEPARATOR, u8"(").has_value()) {
			ret = Type();
			if (ret.has_value()) {
				if (!consume(TokenType::SEPARATOR, u8")").has_value())
					return std::nullopt;
			} else if (!consume(TokenType::SEPARATOR, u8")").has_value())
				return std::nullopt;
			else {
				ret = make_unique<UnitType>();
			}
		} else if (!consume(TokenType::IDENTIFIER).has_value())
			return std::nullopt;
	}
	auto codeBlock = CodeBlock();
	if (!codeBlock.has_value())
		return std::nullopt;
	return make_unique<FuncDecl>(moduleName, std::move(*params), std::move(*ret),
								 std::move(*codeBlock));
}

Opt<Vec<Param>> Parser::ParamList() {
	Vec<Param> params;
	while (true) {
		auto name = consume(TokenType::IDENTIFIER);
		if (!name.has_value())
			return std::nullopt;
		if (!consume(TokenType::SEPARATOR, u8":").has_value())
			return std::nullopt;
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		params.push_back({name->lexeme, std::move(*t)});
		if (!consume(TokenType::SEPARATOR, u8",").has_value())
			return params;
	}
}

Opt<Box<Type>> Parser::Type() {
	auto name = consume(TokenType::IDENTIFIER);
	if (name.has_value())
		return make_unique<Typename>(name->lexeme);
	if (consume(TokenType::OPERATOR, u8"*").has_value()) {
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		return make_unique<PointerType>(std::move(*t));
	}
	if (consume(TokenType::SEPARATOR, u8"[").has_value()) {
		auto num = consume(TokenType::NUMERIC_LITERAL);
		if (!consume(TokenType::SEPARATOR, u8"]").has_value())
			return std::nullopt;
		auto t = Type();
		if (!t.has_value())
			return std::nullopt;
		Opt<size_t> size = std::nullopt;
		if (num.has_value())
			size = std::strtoull(reinterpret_cast<const char *>(num->lexeme.ptr()), nullptr, 10);
		return make_unique<ArrayType>(std::move(*t), size);
	}
	return std::nullopt;
}

Opt<Box<ast::BlockStmt>> Parser::CodeBlock() {
	return std::nullopt;
}
