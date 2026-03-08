#include "Parser.h"

#include "core/Macros.h"
#include "core/Typedef.h"
#include "lexer/Token.h"

using namespace ast;
using namespace type;
using namespace lex;

namespace prs {
ParsingError::ParsingError(U8String msg)
	: msg(std::move(msg)) {}

Box<Module> Parser::parse(const Vec<Token> &tokens, ErrorHandler &err, U8String moduleName) {
	Parser parser(tokens, err, std::move(moduleName));

	return parser.parseModule();
}

Parser::Parser(const Vec<Token> &tokens, ErrorHandler &err, U8String moduleName)
	: m_Tokens(std::move(tokens))
	, m_ModuleName(std::move(moduleName))
	, m_ErrorHandler(err)
	, m_Current(tokens.begin()) {}

const Token &Parser::peek() const {
	const auto nextToken = m_Current + 1;
	VERIFY(nextToken != m_Tokens.end());

	return *nextToken;
}

void Parser::advance() {
	++m_Current;
	VERIFY(m_Current != m_Tokens.end());
}

const Token &Parser::consume(const TokenType type, U8String lexeme) {
	if (!m_Current->matches(type, lexeme)) {
		U8String msg = std::format("Expected '{}' but found {} instead.", lexeme, *m_Current);

		throw ParsingError(std::move(msg));
	}

	const auto oldCurrent = m_Current;
	++m_Current;

	return *oldCurrent;
}

const Token &Parser::consume(TokenType type) {
	if (!m_Current->matches(type)) {
		U8String msg = std::format("Expected {} but found {} instead.", type, *m_Current);

		throw ParsingError(std::move(msg));
	}

	const auto oldCurrent = m_Current;
	++m_Current;

	return *oldCurrent;
}

void Parser::reportError(ParsingError &e) const {
	m_ErrorHandler.addError(std::move(e.msg), m_Current->loc);
}

Box<Module> Parser::parseModule() {
	Vec<Box<FuncDecl>> funcs;
	Vec<Box<StructDecl>> structs;

	while (!m_Current->matches(TokenType::EndOfFile)) {
		try {
			if (m_Current->matches(TokenType::Keyword, u8"struct")) {
				structs.push_back(parseStructDecl());
			} else if (m_Current->matches(TokenType::Keyword, u8"func")) {
				funcs.push_back(parseFuncDecl());
			} else {
				constexpr auto msg = "Expected 'func' or 'struct' declaration, found {} instead.";
				throw ParsingError(std::format(msg, *m_Current));
			}
		} catch (ParsingError &e) {
			reportError(e);

			while (!m_Current->matches(TokenType::EndOfFile) &&
				   !m_Current->matches(TokenType::Keyword, u8"func") &&
				   !m_Current->matches(TokenType::Keyword, u8"struct")) {
				advance();
			}
		}
	}

	return std::make_unique<Module>(m_ModuleName, std::move(funcs), std::move(structs));
}

Box<FuncDecl> Parser::parseFuncDecl() {
	consume(TokenType::Keyword, u8"func");
	auto name = consume(TokenType::Identifier).lexeme;
	auto params = parseParamList();

	TypePtr returnType = std::make_shared<UnitType>();

	if (m_Current->matches(TokenType::Operator, u8"->")) {
		consume(TokenType::Operator, u8"->");
		returnType = parseType();
	}

	auto body = parseBlockStmt();

	return std::make_unique<FuncDecl>(std::move(name), std::move(params), std::move(returnType),
									  std::move(body));
}

Box<StructDecl> Parser::parseStructDecl() {
	consume(TokenType::Keyword, u8"struct");
	auto name = consume(TokenType::Identifier).lexeme;

	consume(TokenType::Separator, u8"{");

	Vec<StructField> fields;

	while (m_Current->matches(TokenType::Identifier)) {
		auto fieldName = consume(TokenType::Identifier).lexeme;
		consume(TokenType::Separator, u8":");
		auto fieldType = parseType();

		fields.emplace_back(std::move(fieldName), std::move(fieldType));

		if (m_Current->matches(TokenType::Separator, u8",")) {
			advance();
			continue;
		}

		break;
	}

	consume(TokenType::Separator, u8"}");

	return std::make_unique<StructDecl>(std::move(name), std::move(fields));
}

Vec<Param> Parser::parseParamList() {
	Vec<Param> params;
	consume(TokenType::Separator, u8"(");

	while (m_Current->matches(TokenType::Identifier)) {
		auto ident = consume(TokenType::Identifier).lexeme;
		consume(TokenType::Separator, u8":");
		auto type = parseType();

		params.emplace_back(std::move(ident), std::move(type));

		if (m_Current->matches(TokenType::Separator, u8",")) {
			consume(TokenType::Separator, u8",");
			continue;
		}

		break;
	}

	consume(TokenType::Separator, u8")");

	return params;
}

TypePtr Parser::parseType() {
	if (m_Current->matches(TokenType::Identifier)) {
		auto typename_ = consume(TokenType::Identifier).lexeme;

		return std::make_shared<Typename>(typename_);
	}

	if (m_Current->matches(TokenType::Separator, u8"(")) {
		consume(TokenType::Separator, u8"(");
		consume(TokenType::Separator, u8")");

		return std::make_shared<UnitType>();
	}

	if (m_Current->matches(TokenType::Operator, u8"*")) {
		consume(TokenType::Operator, u8"*");
		auto type = parseType();

		return std::make_unique<PointerType>(std::move(type));
	}

	throw ParsingError(u8"Expected a type.");
}

Box<Stmt> Parser::parseStmt() {
	if (m_Current->matches(TokenType::Separator, u8";")) {
		consume(TokenType::Separator, u8";");

		return std::make_unique<UnitLit>();
	}

	if (m_Current->matches(TokenType::Keyword, u8"return")) {
		consume(TokenType::Keyword, u8"return");

		Box<Expr> returnValue = std::make_unique<UnitLit>();

		if (!m_Current->matches(TokenType::Separator, u8";"))
			returnValue = parseExpr();

		consume(TokenType::Separator, u8";");

		return std::make_unique<ReturnStmt>(std::move(returnValue));
	}

	if (m_Current->matches(TokenType::Separator, u8"{"))
		return parseBlockStmt();

	if (m_Current->matches(TokenType::Keyword, u8"if"))
		return parseIfStmt();

	if (m_Current->matches(TokenType::Keyword, u8"while"))
		return parseWhileStmt();

	bool isCurrentIdent = m_Current->matches(TokenType::Identifier);

	if (isCurrentIdent && peek().matches(TokenType::Separator, u8":")) {
		auto varDef = parseVarDef();
		consume(TokenType::Separator, u8";");

		return std::move(varDef);
	}

	auto expr = parseExpr();
	consume(TokenType::Separator, u8";");

	return expr;
}

Box<BlockStmt> Parser::parseBlockStmt() {
	Vec<Box<Stmt>> stmts;
	consume(TokenType::Separator, u8"{");

	while (!m_Current->matches(TokenType::Separator, u8"}")) {
		if (m_Current->matches(TokenType::EndOfFile)) {
			U8String msg = std::format("Previously opened block was never closed, forgot a '}}'?");

			throw ParsingError(std::move(msg));
		}

		auto stmt = parseStmt();
		stmts.push_back(std::move(stmt));
	}

	consume(TokenType::Separator, u8"}");

	return std::make_unique<BlockStmt>(std::move(stmts));
}

Box<WhileStmt> Parser::parseWhileStmt() {
	consume(TokenType::Keyword, u8"while");

	consume(TokenType::Separator, u8"(");
	auto cond = parseExpr();
	consume(TokenType::Separator, u8")");

	auto body = parseBlockStmt();

	return std::make_unique<WhileStmt>(std::move(cond), std::move(body));
}

Box<IfStmt> Parser::parseIfStmt() {
	consume(TokenType::Keyword, u8"if");

	consume(TokenType::Separator, u8"(");
	auto cond = parseExpr();
	consume(TokenType::Separator, u8")");

	auto then = parseBlockStmt();

	if (!m_Current->matches(TokenType::Keyword, u8"else")) {
		auto else_ = std::make_unique<BlockStmt>(Vec<Box<Stmt>>{});

		return std::make_unique<IfStmt>(std::move(cond), std::move(then), std::move(else_));
	}

	consume(TokenType::Keyword, u8"else");

	if (!m_Current->matches(TokenType::Keyword, u8"if")) {
		auto else_ = parseBlockStmt();

		return std::make_unique<IfStmt>(std::move(cond), std::move(then), std::move(else_));
	}

	// If we get an if-else-if construct, normalize it into nested if-else-if construct:
	// >>>  if (a) { b; } else if (c) { d; }
	// Will then get turned into:
	// >>>  if (a) { b; } else { if (c) { d; }}

	auto elseIf = parseIfStmt();

	Vec<Box<Stmt>> elseStmts;
	elseStmts.push_back(std::move(elseIf));
	auto else_ = std::make_unique<BlockStmt>(std::move(elseStmts));

	return std::make_unique<IfStmt>(std::move(cond), std::move(then), std::move(else_));
}

Box<VarDef> Parser::parseVarDef() {
	auto ident = consume(TokenType::Identifier).lexeme;
	consume(TokenType::Separator, u8":");
	auto type = parseType();

	consume(TokenType::Operator, u8"=");
	auto value = parseExpr();

	return std::make_unique<VarDef>(std::move(ident), std::move(type), std::move(value));
}

Box<Expr> Parser::parseExpr() {
	return parseAssignmentExpr();
}

Box<Expr> Parser::parseAssignmentExpr() {
	auto left = parseEqualityExpr();

	if (m_Current->matches(TokenType::Operator)) {
		const auto op = consume(TokenType::Operator).lexeme;
		auto kind = getAssignmentKindFromString(op);
		auto right = parseAssignmentExpr();

		return std::make_unique<Assignment>(kind, std::move(left), std::move(right));
	}

	return left;
}

AssignmentKind Parser::getAssignmentKindFromString(const U8String &str) {
	using enum AssignmentKind;

	static Map<U8String, AssignmentKind> map = {{u8"=", Simple},	   {u8"+=", Addition},
												{u8"-=", Subtraction}, {u8"*=", Multiplication},
												{u8"/=", Division},	   {u8"%=", Modulo},
												{u8"&=", BitwiseAnd},  {u8"|=", BitwiseOr},
												{u8"^=", BitwiseXor},  {u8"<<=", LeftShift},
												{u8">>=", RightShift}};

	auto it = map.find(str);
	VERIFY(it != map.end());

	return it->second;
}

Box<Expr> Parser::parseEqualityExpr() {
	auto left = parseRelationalExpr();

	while (m_Current->matches(TokenType::Operator)) {
		Opt<BinaryOpKind> kind = {};
		const auto &op = m_Current->lexeme;

		if (op == u8"==")
			kind = BinaryOpKind::Equality;
		else if (op == u8"!=")
			kind = BinaryOpKind::Inequality;
		else
			break;

		consume(TokenType::Operator);
		auto right = parseRelationalExpr();

		left = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
	}

	return left;
}

Box<Expr> Parser::parseRelationalExpr() {
	auto left = parseAdditiveExpr();

	while (m_Current->matches(TokenType::Operator)) {
		Opt<BinaryOpKind> kind = {};
		const auto &op = m_Current->lexeme;

		if (op == u8"<")
			kind = BinaryOpKind::LessThan;
		else if (op == u8">")
			kind = BinaryOpKind::GreaterThan;
		else if (op == u8"<=")
			kind = BinaryOpKind::LessThanOrEqual;
		else if (op == u8">=")
			kind = BinaryOpKind::GreaterThanOrEqual;
		else
			break;

		consume(TokenType::Operator);
		auto right = parseAdditiveExpr();

		left = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
	}

	return left;
}

Box<Expr> Parser::parseAdditiveExpr() {
	auto left = parseMultiplicativeExpr();

	while (m_Current->matches(TokenType::Operator)) {
		Opt<BinaryOpKind> kind = {};
		const auto &op = m_Current->lexeme;

		if (op == u8"+")
			kind = BinaryOpKind::Addition;
		else if (op == u8"-")
			kind = BinaryOpKind::Subtraction;
		else
			break;

		consume(TokenType::Operator);
		auto right = parseMultiplicativeExpr();

		left = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
	}

	return left;
}

Box<Expr> Parser::parseMultiplicativeExpr() {
	auto left = parseUnaryExpr();

	while (m_Current->matches(TokenType::Operator)) {
		Opt<BinaryOpKind> kind = {};
		const auto &op = m_Current->lexeme;

		if (op == u8"*")
			kind = BinaryOpKind::Multiplication;
		else if (op == u8"/")
			kind = BinaryOpKind::Division;
		else if (op == u8"%")
			kind = BinaryOpKind::Modulo;
		else
			break;

		consume(TokenType::Operator);
		auto right = parseUnaryExpr();

		left = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
	}

	return left;
}

Box<Expr> Parser::parseUnaryExpr() {
	if (!m_Current->matches(TokenType::Operator))
		return parsePostfixExpr();

	Opt<UnaryOpKind> kind = {};
	const auto &op = consume(TokenType::Operator).lexeme;

	if (op == u8"+")
		kind = UnaryOpKind::Positive;
	else if (op == u8"-")
		kind = UnaryOpKind::Negative;
	else if (op == u8"!")
		kind = UnaryOpKind::LogicalNot;
	else if (op == u8"~")
		kind = UnaryOpKind::BitwiseNot;
	else if (op == u8"*")
		kind = UnaryOpKind::Dereference;
	else {
		const auto msg = std::format("Operator '{}' can not be used as unary operator.", op);

		throw ParsingError(std::move(msg));
	}

	auto expr = parseUnaryExpr();

	return std::make_unique<UnaryExpr>(kind.value(), std::move(expr));
}

Vec<Box<ast::Expr>> Parser::parseExprList() {
	Vec<Box<ast::Expr>> exprs;
	consume(TokenType::Separator, u8"(");

	while (!m_Current->matches(TokenType::Separator, u8")")) {
		auto expr = parseExpr();
		exprs.push_back(std::move(expr));

		if (m_Current->matches(TokenType::Separator, u8",")) {
			consume(TokenType::Separator, u8",");
			continue;
		}

		break;
	}

	consume(TokenType::Separator, u8")");

	return exprs;
}

Box<Expr> Parser::parsePostfixExpr() {
	auto left = parsePrimaryExpr();

	while (m_Current->matches(TokenType::Separator)) {
		if (m_Current->matches(TokenType::Separator, u8"(")) {
			auto args = parseExprList();

			left = std::make_unique<FuncCall>(std::move(left), std::move(args));

			continue;
		}

		break;
	}

	return left;
}

Box<Expr> Parser::parsePrimaryExpr() {
	if (m_Current->matches(TokenType::Identifier)) {
		const auto &ident = consume(TokenType::Identifier).lexeme;

		return std::make_unique<VarRef>(ident);
	}

	if (m_Current->matches(TokenType::BoolLiteral)) {
		const auto &lit = consume(TokenType::BoolLiteral).lexeme;
		VERIFY(lit == u8"true" || lit == u8"false");

		return std::make_unique<BoolLit>(lit == u8"true");
	}

	if (m_Current->matches(TokenType::CharLiteral)) {
		const auto &ident = consume(TokenType::CharLiteral).lexeme;
		VERIFY(ident.length() == 1);

		return std::make_unique<CharLit>(ident[0]);
	}

	if (m_Current->matches(TokenType::IntLiteral)) {
		const auto &lit = consume(TokenType::IntLiteral).lexeme;

		return std::make_unique<IntLit>(std::stoi(lit.asAscii()));
	}

	if (m_Current->matches(TokenType::Keyword, u8"new")) {
		consume(TokenType::Keyword, u8"new");

		auto type = parseType();

		consume(TokenType::Separator, u8"(");
		auto expr = parseExpr();
		consume(TokenType::Separator, u8")");

		return std::make_unique<HeapAlloc>(std::move(type), std::move(expr));
	}

	if (m_Current->matches(TokenType::Separator, u8"(")) {
		consume(TokenType::Separator, u8"(");

		if (m_Current->matches(TokenType::Separator, u8")")) {
			consume(TokenType::Separator, u8")");

			return std::make_unique<UnitLit>();
		}

		auto expr = parseExpr();
		consume(TokenType::Separator, u8")");

		return expr;
	}

	const auto msg = std::format("Expected an expression, found {} instead.", *m_Current);

	throw ParsingError(std::move(msg));
}
}