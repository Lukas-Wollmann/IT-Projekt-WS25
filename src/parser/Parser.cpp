#include "Parser.h"

#include "core/Macros.h"
#include "core/Typedef.h"
#include "lexer/Token.h"

using namespace ast;
using namespace lex;

namespace prs {
namespace {
SourceLoc makeSpanLoc(const SourceLoc &start, const SourceLoc &end) {
	SourceLoc loc = start;
	const size_t endLen = end.length == 0 ? 1 : end.length;
	const size_t endIndex = end.index + endLen;
	loc.length = endIndex > start.index ? (endIndex - start.index) : 1;
	return loc;
}
}

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
	const auto moduleLoc = m_Current->loc;
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

	auto module = std::make_unique<Module>(m_ModuleName, std::move(funcs), std::move(structs));
	module->setLoc(makeSpanLoc(moduleLoc, m_Current->loc));
	return module;
}

Box<FuncDecl> Parser::parseFuncDecl() {
	const auto &funcToken = consume(TokenType::Keyword, u8"func");
	auto name = consume(TokenType::Identifier).lexeme;
	auto params = parseParamList();

	Type returnType = TypeFactory::getUnit();

	if (m_Current->matches(TokenType::Operator, u8"->")) {
		consume(TokenType::Operator, u8"->");
		returnType = parseType();
	}

	auto body = parseBlockStmt();

	auto func = std::make_unique<FuncDecl>(std::move(name), std::move(params), returnType,
										   std::move(body));
	func->setLoc(makeSpanLoc(funcToken.loc, func->body->loc));
	return func;
}

Box<StructDecl> Parser::parseStructDecl() {
	const auto &structToken = consume(TokenType::Keyword, u8"struct");
	auto name = consume(TokenType::Identifier).lexeme;

	consume(TokenType::Separator, u8"{");

	Vec<StructField> fields;

	while (m_Current->matches(TokenType::Identifier)) {
		auto fieldName = consume(TokenType::Identifier).lexeme;
		consume(TokenType::Separator, u8":");
		auto fieldType = parseType();

		fields.emplace_back(std::move(fieldName), fieldType);

		if (m_Current->matches(TokenType::Separator, u8",")) {
			advance();

			if (m_Current->matches(TokenType::Separator, u8"}")) {
				throw ParsingError(u8"Expected another struct field.");
			}

			continue;
		}

		break;
	}

	consume(TokenType::Separator, u8"}");

	auto decl = std::make_unique<StructDecl>(std::move(name), std::move(fields));
	decl->setLoc(structToken.loc);
	return decl;
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

			if (m_Current->matches(TokenType::Separator, u8")")) {
				throw ParsingError(u8"Expected another parameter.");
			}

			continue;
		}

		break;
	}

	consume(TokenType::Separator, u8")");

	return params;
}

Type Parser::parseType() {
	if (m_Current->matches(TokenType::Identifier)) {
		auto typename_ = consume(TokenType::Identifier).lexeme;

		if (typename_ == u8"i32") {
			return TypeFactory::getI32();
		}
		if (typename_ == u8"char") {
			return TypeFactory::getChar();
		}
		if (typename_ == u8"bool") {
			return TypeFactory::getBool();
		}

		return TypeFactory::getStruct(typename_);
	}

	if (m_Current->matches(TokenType::Separator, u8"(")) {
		consume(TokenType::Separator, u8"(");
		consume(TokenType::Separator, u8")");

		return TypeFactory::getUnit();
	}

	if (m_Current->matches(TokenType::Operator, u8"*")) {
		consume(TokenType::Operator, u8"*");
		auto type = parseType();

		return TypeFactory::getPointer(type);
	}

	throw ParsingError(u8"Expected a type.");
}

Box<Stmt> Parser::parseStmt() {
	if (m_Current->matches(TokenType::Separator, u8";")) {
		const auto &semi = consume(TokenType::Separator, u8";");
		auto unit = std::make_unique<UnitLit>();
		unit->setLoc(semi.loc);
		return unit;
	}

	if (m_Current->matches(TokenType::Keyword, u8"return")) {
		const auto &returnTok = consume(TokenType::Keyword, u8"return");

		Box<Expr> returnValue = std::make_unique<UnitLit>();
		returnValue->setLoc(returnTok.loc);

		if (!m_Current->matches(TokenType::Separator, u8";"))
			returnValue = parseExpr();

		const auto &semi = consume(TokenType::Separator, u8";");

		auto stmt = std::make_unique<ReturnStmt>(std::move(returnValue));
		stmt->setLoc(makeSpanLoc(returnTok.loc, semi.loc));
		return stmt;
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
		const auto &semi = consume(TokenType::Separator, u8";");
		varDef->setLoc(makeSpanLoc(varDef->loc, semi.loc));

		return std::move(varDef);
	}

	auto expr = parseExpr();
	const auto &semi = consume(TokenType::Separator, u8";");
	expr->setLoc(makeSpanLoc(expr->loc, semi.loc));

	return expr;
}

Box<BlockStmt> Parser::parseBlockStmt() {
	Vec<Box<Stmt>> stmts;
	const auto &lbrace = consume(TokenType::Separator, u8"{");

	while (!m_Current->matches(TokenType::Separator, u8"}")) {
		if (m_Current->matches(TokenType::EndOfFile)) {
			U8String msg = std::format("Previously opened block was never closed, forgot a '}}'?");

			throw ParsingError(std::move(msg));
		}

		auto stmt = parseStmt();
		stmts.push_back(std::move(stmt));
	}

	const auto &rbrace = consume(TokenType::Separator, u8"}");

	auto block = std::make_unique<BlockStmt>(std::move(stmts));
	block->setLoc(makeSpanLoc(lbrace.loc, rbrace.loc));
	return block;
}

Box<WhileStmt> Parser::parseWhileStmt() {
	const auto &whileTok = consume(TokenType::Keyword, u8"while");

	consume(TokenType::Separator, u8"(");
	auto cond = parseExpr();
	consume(TokenType::Separator, u8")");

	auto body = parseBlockStmt();

	auto stmt = std::make_unique<WhileStmt>(std::move(cond), std::move(body));
	stmt->setLoc(whileTok.loc);
	return stmt;
}

Box<IfStmt> Parser::parseIfStmt() {
	const auto &ifTok = consume(TokenType::Keyword, u8"if");

	consume(TokenType::Separator, u8"(");
	auto cond = parseExpr();
	consume(TokenType::Separator, u8")");

	auto then = parseBlockStmt();

	if (!m_Current->matches(TokenType::Keyword, u8"else")) {
		auto else_ = std::make_unique<BlockStmt>(Vec<Box<Stmt>>{});
		else_->setLoc(ifTok.loc);
		auto stmt = std::make_unique<IfStmt>(std::move(cond), std::move(then), std::move(else_));
		stmt->setLoc(ifTok.loc);
		return stmt;
	}

	consume(TokenType::Keyword, u8"else");

	if (!m_Current->matches(TokenType::Keyword, u8"if")) {
		auto else_ = parseBlockStmt();
		auto stmt = std::make_unique<IfStmt>(std::move(cond), std::move(then), std::move(else_));
		stmt->setLoc(ifTok.loc);
		return stmt;
	}

	// If we get an if-else-if construct, normalize it into nested if-else-if construct:
	// >>>  if (a) { b; } else if (c) { d; }
	// Will then get turned into:
	// >>>  if (a) { b; } else { if (c) { d; }}

	auto elseIf = parseIfStmt();

	Vec<Box<Stmt>> elseStmts;
	elseStmts.push_back(std::move(elseIf));
	auto else_ = std::make_unique<BlockStmt>(std::move(elseStmts));
	else_->setLoc(ifTok.loc);
	auto stmt = std::make_unique<IfStmt>(std::move(cond), std::move(then), std::move(else_));
	stmt->setLoc(ifTok.loc);
	return stmt;
}

Box<VarDef> Parser::parseVarDef() {
	const auto &identToken = consume(TokenType::Identifier);
	auto ident = identToken.lexeme;
	consume(TokenType::Separator, u8":");
	auto type = parseType();
	Box<Expr> value;
	// Allow variable definition without an initializer: `a: Foo;`
	if (m_Current->matches(TokenType::Operator, u8"=")) {
		consume(TokenType::Operator, u8"=");
		value = parseExpr();
	} else {
		// Default initialization
		value = std::make_unique<DefaultInit>();
		value->setLoc(identToken.loc);
	}

	auto varDef = std::make_unique<VarDef>(std::move(ident), std::move(type), std::move(value));
	varDef->setLoc(identToken.loc);
	return varDef;
}

Box<Expr> Parser::parseExpr() {
	return parseAssignmentExpr();
}

Box<Expr> Parser::parseAssignmentExpr() {
	auto left = parseLogicalOrExpr();

	if (m_Current->matches(TokenType::Operator)) {
		const auto &op = m_Current->lexeme;
		if (op != u8"=" && op != u8"+=" && op != u8"-=" && op != u8"*=" && op != u8"/=" &&
			op != u8"%=" && op != u8"&=" && op != u8"|=" && op != u8"^=" && op != u8"<<=" &&
			op != u8">>=") {
			return left;
		}

		const auto &opToken = consume(TokenType::Operator);
		auto kind = getAssignmentKindFromString(op);
		auto right = parseAssignmentExpr();
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto assignment = std::make_unique<Assignment>(kind, std::move(left), std::move(right));
		assignment->setLoc(loc);
		return assignment;
	}

	return left;
}

Box<Expr> Parser::parseLogicalOrExpr() {
	auto left = parseLogicalAndExpr();

	while (m_Current->matches(TokenType::Operator, u8"||")) {
		consume(TokenType::Operator, u8"||");
		auto right = parseLogicalAndExpr();
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(BinaryOpKind::LogicalOr, std::move(left),
												   std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
	}

	return left;
}

Box<Expr> Parser::parseLogicalAndExpr() {
	auto left = parseBitwiseOrExpr();

	while (m_Current->matches(TokenType::Operator, u8"&&")) {
		consume(TokenType::Operator, u8"&&");
		auto right = parseBitwiseOrExpr();
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(BinaryOpKind::LogicalAnd, std::move(left),
												   std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
	}

	return left;
}

Box<Expr> Parser::parseBitwiseOrExpr() {
	auto left = parseBitwiseXorExpr();

	while (m_Current->matches(TokenType::Operator, u8"|")) {
		consume(TokenType::Operator, u8"|");
		auto right = parseBitwiseXorExpr();
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(BinaryOpKind::BitwiseOr, std::move(left),
												   std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
	}

	return left;
}

Box<Expr> Parser::parseBitwiseXorExpr() {
	auto left = parseBitwiseAndExpr();

	while (m_Current->matches(TokenType::Operator, u8"^")) {
		consume(TokenType::Operator, u8"^");
		auto right = parseBitwiseAndExpr();
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(BinaryOpKind::BitwiseXor, std::move(left),
												   std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
	}

	return left;
}

Box<Expr> Parser::parseBitwiseAndExpr() {
	auto left = parseEqualityExpr();

	while (m_Current->matches(TokenType::Operator, u8"&")) {
		consume(TokenType::Operator, u8"&");
		auto right = parseEqualityExpr();
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(BinaryOpKind::BitwiseAnd, std::move(left),
												   std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
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
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
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
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
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
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
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
		const auto loc = makeSpanLoc(left->loc, right->loc);

		auto binary = std::make_unique<BinaryExpr>(kind.value(), std::move(left), std::move(right));
		binary->setLoc(loc);
		left = std::move(binary);
	}

	return left;
}

Box<Expr> Parser::parseUnaryExpr() {
	if (!m_Current->matches(TokenType::Operator))
		return parsePostfixExpr();

	Opt<UnaryOpKind> kind = {};
	const auto &opToken = consume(TokenType::Operator);
	const auto &op = opToken.lexeme;

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
	const auto loc = makeSpanLoc(opToken.loc, expr->loc);

	auto unary = std::make_unique<UnaryExpr>(kind.value(), std::move(expr));
	unary->setLoc(loc);
	return unary;
}

Vec<Box<ast::Expr>> Parser::parseExprList() {
	Vec<Box<ast::Expr>> exprs;
	consume(TokenType::Separator, u8"(");

	while (!m_Current->matches(TokenType::Separator, u8")")) {
		auto expr = parseExpr();
		exprs.push_back(std::move(expr));

		if (m_Current->matches(TokenType::Separator, u8",")) {
			consume(TokenType::Separator, u8",");

			if (m_Current->matches(TokenType::Separator, u8")")) {
				throw ParsingError(u8"Expected another argument.");
			}

			continue;
		}

		break;
	}

	consume(TokenType::Separator, u8")");

	return exprs;
}

Vec<Box<ast::Expr>> Parser::parseBraceExprList() {
	Vec<Box<ast::Expr>> exprs;
	consume(TokenType::Separator, u8"{");

	while (!m_Current->matches(TokenType::Separator, u8"}")) {
		auto expr = parseExpr();
		exprs.push_back(std::move(expr));

		if (m_Current->matches(TokenType::Separator, u8",")) {
			consume(TokenType::Separator, u8",");

			if (m_Current->matches(TokenType::Separator, u8"}")) {
				throw ParsingError(u8"Expected another argument.");
			}

			continue;
		}

		break;
	}

	consume(TokenType::Separator, u8"}");

	return exprs;
}

Box<Expr> Parser::parseFieldAccess(Box<Expr> base) {
	consume(TokenType::Separator, u8".");

	// Allow optional '*' prefix before the field identifier: . *ident
	bool hadStar = false;
	SourceLoc starLoc;
	if (m_Current->matches(TokenType::Operator, u8"*")) {
		const auto &starTok = consume(TokenType::Operator, u8"*");
		hadStar = true;
		starLoc = starTok.loc;
	}

	const auto &fieldToken = consume(TokenType::Identifier);
	auto field = fieldToken.lexeme;
	const auto loc = makeSpanLoc(base->loc, fieldToken.loc);

	auto access = std::make_unique<FieldAccess>(std::move(base), std::move(field));
	access->setLoc(loc);

	if (hadStar) {
		auto unary = std::make_unique<UnaryExpr>(UnaryOpKind::Dereference, std::move(access));
		unary->setLoc(makeSpanLoc(starLoc, access->loc));
		return unary;
	}

	return access;
}

Box<Expr> Parser::parsePostfixExpr() {
	// Support leading `*ident` as base (binds only to the identifier, not the whole postfix chain).
	Box<Expr> left;
	if (m_Current->matches(TokenType::Operator, u8"*") && peek().matches(TokenType::Identifier)) {
		const auto &starTok = consume(TokenType::Operator, u8"*");
		// parsePrimaryExpr will consume the identifier and produce a VarRef (or StructInit)
		left = parsePrimaryExpr();
		auto unary = std::make_unique<UnaryExpr>(UnaryOpKind::Dereference, std::move(left));
		unary->setLoc(makeSpanLoc(starTok.loc, unary->operand->loc));
		left = std::move(unary);
	} else {
		left = parsePrimaryExpr();
	}

	while (m_Current->matches(TokenType::Separator)) {
		if (m_Current->matches(TokenType::Separator, u8"(")) {
			const auto leftLoc = left->loc;
			auto args = parseExprList();
			const auto endLoc = args.empty() ? leftLoc : args.back()->loc;

			auto call = std::make_unique<FuncCall>(std::move(left), std::move(args));
			call->setLoc(makeSpanLoc(leftLoc, endLoc));
			left = std::move(call);

			continue;
		}

		if (m_Current->matches(TokenType::Separator, u8".")) {
			left = parseFieldAccess(std::move(left));
			continue;
		}

		break;
	}

	return left;
}

Box<Expr> Parser::parsePrimaryExpr() {
	if (m_Current->matches(TokenType::Identifier)) {
		const auto &identTok = consume(TokenType::Identifier);
		auto ident = identTok.lexeme;

		if (m_Current->matches(TokenType::Separator, u8"{")) {
			auto *structType = static_cast<StructType *>(TypeFactory::getStruct(ident));
			auto args = parseBraceExprList();
			auto init = std::make_unique<StructInit>(structType, std::move(args));
			const auto endLoc = init->args.empty() ? identTok.loc : init->args.back()->loc;
			init->setLoc(makeSpanLoc(identTok.loc, endLoc));
			return init;
		}

		auto varRef = std::make_unique<VarRef>(ident);
		varRef->setLoc(identTok.loc);
		return varRef;
	}

	if (m_Current->matches(TokenType::BoolLiteral)) {
		const auto &tok = consume(TokenType::BoolLiteral);
		const auto &lit = tok.lexeme;
		VERIFY(lit == u8"true" || lit == u8"false");

		auto boolLit = std::make_unique<BoolLit>(lit == u8"true");
		boolLit->setLoc(tok.loc);
		return boolLit;
	}

	if (m_Current->matches(TokenType::Keyword, u8"null")) {
		const auto &tok = consume(TokenType::Keyword, u8"null");
		auto nullLit = std::make_unique<NullLit>();
		nullLit->setLoc(tok.loc);
		return nullLit;
	}

	if (m_Current->matches(TokenType::CharLiteral)) {
		const auto &tok = consume(TokenType::CharLiteral);
		const auto &ident = tok.lexeme;
		VERIFY(ident.length() == 1);

		auto charLit = std::make_unique<CharLit>(ident[0]);
		charLit->setLoc(tok.loc);
		return charLit;
	}

	if (m_Current->matches(TokenType::IntLiteral)) {
		const auto &tok = consume(TokenType::IntLiteral);
		const auto &lit = tok.lexeme;

		auto intLit = std::make_unique<IntLit>(std::stoi(lit.asAscii()));
		intLit->setLoc(tok.loc);
		return intLit;
	}

	if (m_Current->matches(TokenType::Keyword, u8"new")) {
		const auto &newTok = consume(TokenType::Keyword, u8"new");

		auto type = parseType();
		Box<Expr> expr;

		if (m_Current->matches(TokenType::Separator, u8"(")) {
			consume(TokenType::Separator, u8"(");
			if (m_Current->matches(TokenType::Separator, u8")")) {
				// Empty parens -> default initialization
				consume(TokenType::Separator, u8")");
				expr = std::make_unique<DefaultInit>();
				expr->setLoc(newTok.loc);
			} else {
				expr = parseExpr();
				consume(TokenType::Separator, u8")");
			}
		} else if (m_Current->matches(TokenType::Separator, u8"{")) {
			if (!type->isTypeKind(TypeKind::Struct)) {
				throw ParsingError(u8"Brace initialization is only supported for struct types.");
			}

			auto *structType = static_cast<StructType *>(type);
			auto args = parseBraceExprList();
			const auto endLoc = args.empty() ? newTok.loc : args.back()->loc;
			expr = std::make_unique<StructInit>(structType, std::move(args));
			expr->setLoc(makeSpanLoc(newTok.loc, endLoc));
		} else {
			// Bare `new Type` without parens or braces -> default initialization
			expr = std::make_unique<DefaultInit>();
			expr->setLoc(newTok.loc);
		}

		auto alloc = std::make_unique<HeapAlloc>(std::move(type), std::move(expr));
		alloc->setLoc(makeSpanLoc(newTok.loc, alloc->expr->loc));
		return alloc;
	}

	if (m_Current->matches(TokenType::Separator, u8"(")) {
		const auto &lparen = consume(TokenType::Separator, u8"(");

		if (m_Current->matches(TokenType::Separator, u8")")) {
			consume(TokenType::Separator, u8")");

			auto unit = std::make_unique<UnitLit>();
			unit->setLoc(lparen.loc);
			return unit;
		}

		auto expr = parseExpr();
		consume(TokenType::Separator, u8")");

		return expr;
	}

	const auto msg = std::format("Expected an expression, found {} instead.", *m_Current);

	throw ParsingError(std::move(msg));
}
}