#include "Lexer.h"

#include <algorithm>

#include "CharUtil.h"
#include "Macros.h"

namespace lexer {
	const U8String s_Operators[] = {u8"+",	u8"-",	u8"*",	 u8"/",	 u8"=",	 u8"!",	 u8"<",
									u8">",	u8"&",	u8"|",	 u8"^",	 u8"%",	 u8"~",	 u8"&&",
									u8"||", u8"==", u8"<=",	 u8">=", u8"!=", u8"<<", u8">>",
									u8"+=", u8"-=", u8"*=",	 u8"/=", u8"%=", u8"^=", u8"&=",
									u8"|=", u8"->", u8"<<=", u8">>="};

	constexpr char32_t s_Separators[] = {U';', U',', U'(', U')', U'{',
										 U'}', U'[', U']', U':', U'.'};

	const U8String s_Keywords[] = {u8"if", u8"else", u8"while", u8"return", u8"func", u8"new"};

	Vec<Token> Lexer::tokenize(const U8String &source, const bool comments) {
		Vec<Token> tokens;
		Lexer lexer(source);

		while (true) {
			Token token = lexer.nextToken();

			if (comments || token.type != TokenType::Comment)
				tokens.push_back(token);

			if (token.type == TokenType::EndOfFile)
				break;
		}

		return tokens;
	}

	Lexer::Lexer(const U8String &source)
		: m_Source(source)
		, m_Iter(m_Source.begin())
		, m_Current(m_Source.empty() ? '\0' : *m_Iter)
		, m_CurrentLoc(1, 1, 0) {}

	Token Lexer::nextToken() {
		skipWhitespace();

		if (isAtEnd())
			return {TokenType::EndOfFile, u8"", m_CurrentLoc};

		static constexpr std::array funcs = {&Lexer::tryLexSingleLineComment,
											 &Lexer::tryLexMultiLineComment,
											 &Lexer::tryLexIdentifier,
											 &Lexer::tryLexIntLiteral,
											 &Lexer::tryLexStringLiteral,
											 &Lexer::tryLexCharLiteral,
											 &Lexer::tryLexOperator,
											 &Lexer::tryLexSeparator};

		for (const auto fn : funcs) {
			if (auto token = (this->*fn)())
				return token.value();
		}

		return lexIllegal();
	}

	bool Lexer::isAtEnd() const {
		return m_Current == U'\0';
	}

	void Lexer::advance() {
		if (isAtEnd())
			UNREACHABLE();

		if (m_Current == U'\n') {
			++m_CurrentLoc.line;
			m_CurrentLoc.column = 1;
		} else {
			++m_CurrentLoc.column;
		}

		++m_CurrentLoc.index;
		++m_Iter;
		m_Current = m_Iter == m_Source.end() ? U'\0' : *m_Iter;
	}

	void Lexer::skipWhitespace() {
		while (true) {
			switch (m_Current) {
				case U' ':
				case U'\t':
				case U'\r':
				case U'\n': advance(); break;
				default:	return;
			}
		}
	}

	char32_t Lexer::peek() const {
		auto it = m_Iter;
		++it;

		return it == m_Source.end() ? U'\0' : *it;
	}

	bool Lexer::doesMatch(const U8String &str) const {
		auto it = m_Iter;

		for (const char32_t c : str) {
			if (it == m_Source.end() || *it != c)
				return false;

			++it;
		}

		return true;
	}

	char32_t Lexer::getEscapedChar(const char32_t c) {
		switch (c) {
			case U'\\': return U'\\';
			case U'\'': return U'\'';
			case U'n':	return U'\n';
			case U't':	return U'\t';
			case U'r':	return U'\r';
			case U'0':	return U'\0';
			case U'"':	return U'"';
			default:	return c;
		}
	}

	Opt<Token> Lexer::tryLexIdentifier() {
		if (!(util::isAlpha(m_Current) || m_Current == U'_'))
			return {};

		SourceLoc start = m_CurrentLoc;
		U8String lexeme;

		lexeme += m_Current;
		advance();

		while (util::isAlNum(m_Current) || m_Current == U'_') {
			lexeme += m_Current;
			advance();
		}

		if (lexeme == u8"true" || lexeme == u8"false")
			return Token(TokenType::BoolLiteral, std::move(lexeme), start);

		const auto begin = std::begin(s_Keywords);
		const auto end = std::end(s_Keywords);
		const auto it = std::find(begin, end, lexeme);
		const auto type = it == end ? TokenType::Identifier : TokenType::Keyword;

		return Token(type, std::move(lexeme), start);
	}

	Opt<Token> Lexer::tryLexIntLiteral() {
		if (!util::isNum(m_Current))
			return {};

		SourceLoc start = m_CurrentLoc;
		U8String lexeme;

		while (util::isNum(m_Current)) {
			lexeme += U8String(m_Current);
			advance();
		}

		return Token(TokenType::IntLiteral, std::move(lexeme), start);
	}

	Opt<Token> Lexer::tryLexStringLiteral() {
		if (m_Current != U'"')
			return {};

		advance();

		SourceLoc start = m_CurrentLoc;
		U8String lexeme;

		bool escape = false;
		while (m_Current != U'"' || escape) {
			if (isAtEnd() || m_Current == U'\n')
				return Token(TokenType::Illegal, std::move(lexeme), start,
							 TokenError::UnterminatedStringLiteral);

			if (m_Current == U'\\' && !escape) {
				escape = true;
				advance();

				continue;
			}

			const char32_t escapedChar = escape ? getEscapedChar(m_Current) : m_Current;
			escape = false;

			lexeme += escapedChar;
			advance();
		}

		advance();

		return Token(TokenType::StringLiteral, std::move(lexeme), start);
	}

	Opt<Token> Lexer::tryLexCharLiteral() {
		if (m_Current != U'\'')
			return {};

		advance();

		const SourceLoc start = m_CurrentLoc;
		U8String lexeme;

		bool escape = false;
		while (m_Current != U'\'' || escape) {
			if (isAtEnd() || m_Current == U'\n')
				return Token(TokenType::Illegal, std::move(lexeme), start,
							 TokenError::UnterminatedCharLiteral);

			if (m_Current == U'\\' && !escape) {
				escape = true;
				advance();

				continue;
			}

			const char32_t escapedChar = escape ? getEscapedChar(m_Current) : m_Current;
			escape = false;

			lexeme += escapedChar;
			advance();
		}

		advance();

		if (lexeme.empty())
			return Token(TokenType::Illegal, std::move(lexeme), start,
						 TokenError::EmptyCharLiteral);

		if (lexeme.length() > 1)
			return Token(TokenType::Illegal, std::move(lexeme), start,
						 TokenError::MultipleCharsInCharLiteral);

		return Token(TokenType::CharLiteral, std::move(lexeme), start);
	}

	Opt<Token> Lexer::tryLexSeparator() {
		const auto begin = std::begin(s_Separators);
		const auto end = std::end(s_Separators);

		if (std::find(begin, end, m_Current) == end)
			return {};

		const SourceLoc start = m_CurrentLoc;
		const char32_t separator = m_Current;
		advance();

		return Token(TokenType::Separator, separator, start);
	}

	Opt<Token> Lexer::tryLexSingleLineComment() {
		if (!(m_Current == U'/' && peek() == U'/'))
			return {};

		advance();
		advance();

		const SourceLoc start = m_CurrentLoc;
		U8String lexeme;

		while (m_Current != U'\n' && !isAtEnd()) {
			lexeme += m_Current;
			advance();
		}

		return Token(TokenType::Comment, std::move(lexeme), start);
	}

	Opt<Token> Lexer::tryLexMultiLineComment() {
		if (!(m_Current == U'/' && peek() == U'*'))
			return {};

		advance();
		advance();

		const SourceLoc start = m_CurrentLoc;
		U8String lexeme;

		while (!(m_Current == U'*' && peek() == U'/')) {
			if (isAtEnd())
				return Token(TokenType::Illegal, std::move(lexeme), start,
							 TokenError::UnterminatedBlockComment);

			lexeme += m_Current;
			advance();
		}

		advance();
		advance();

		return Token(TokenType::Comment, std::move(lexeme), start);
	}

	Opt<Token> Lexer::tryLexOperator() {
		const SourceLoc start = m_CurrentLoc;
		Opt<Ref<const U8String>> bestMatch = {};

		for (auto &op : s_Operators) {
			if (!doesMatch(op))
				continue;

			if (!bestMatch.has_value() || op.length() > bestMatch->get().length())
				bestMatch = op;
		}

		if (!bestMatch.has_value())
			return {};

		const size_t len = bestMatch.value().get().length();

		for (size_t i = 0; i < len; ++i)
			advance();

		return Token(TokenType::Operator, bestMatch.value().get(), start);
	}

	Token Lexer::lexIllegal() {
		const SourceLoc start = m_CurrentLoc;
		const U8String lexeme = m_Current;

		advance();

		return {TokenType::Illegal, lexeme, start, TokenError::IllegalIdentifier};
	}
}
