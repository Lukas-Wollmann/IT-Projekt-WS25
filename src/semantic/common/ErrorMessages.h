#pragma once
#include <sstream>

#include "ast/AST.h"
#include "type/PrintVisitor.h"
#include "type/Type.h"

namespace semantic {
	enum struct ErrorMessageKind {
		TYPE_MISSMATCH,
		DEREFERENCE_NON_POINTER_TYPE,
		UNARY_OPERATOR_NOT_FOUND,
		BINARY_OPERATOR_NOT_FOUND,
		CALL_ON_NON_FUNCTION,
		TOO_MANY_ARGUMENTS,
		UNREACHABLE_STATEMENT,
		VARIABLE_REDEFINITION,
		NON_RETURNING_PATHS,
        UNDEFINED_REFERENCE,
        ASSIGN_TO_RVALUE
	};

	template <ErrorMessageKind E>
	struct ErrorMessage;

	template <>
	struct ErrorMessage<ErrorMessageKind::TYPE_MISSMATCH> {
		U8String str(type::TypePtr expected, type::TypePtr actual) const {
			std::stringstream ss;
			ss << "Expected value of type '" << *expected << "', got type '" << *actual
			   << "' instead.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::DEREFERENCE_NON_POINTER_TYPE> {
		U8String str(type::TypePtr type) const {
			std::stringstream ss;
			ss << "Cannot dereference the non-pointer type '" << *type << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::UNARY_OPERATOR_NOT_FOUND> {
		U8String str(type::TypePtr type, ast::UnaryOpKind op) const {
			std::stringstream ss;
			ss << "Cannot use unary operator '" << op << "' on a value of '" << *type << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::BINARY_OPERATOR_NOT_FOUND> {
		U8String str(type::TypePtr left, type::TypePtr right, ast::BinaryOpKind op) const {
			std::stringstream ss;
			ss << "Cannot use binary operator '" << op << "' on values of type '" << *left
			   << "' and '" << *right << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::CALL_ON_NON_FUNCTION> {
		U8String str(type::TypePtr type) const {
			std::stringstream ss;
			ss << "Cannot call the non-function type '" << *type << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::TOO_MANY_ARGUMENTS> {
		U8String str(size_t expected, size_t actual) const {
			std::stringstream ss;
			ss << "Expected " << expected << " arguments, found " << actual << ".";
			return U8String(ss.str());
		}
	};

    template <>
	struct ErrorMessage<ErrorMessageKind::UNREACHABLE_STATEMENT> {
		U8String str() const {
			return U8String("Found unreachable statement after return.");
		}
	};

    template <>
	struct ErrorMessage<ErrorMessageKind::VARIABLE_REDEFINITION> {
		U8String str(const U8String &ident) const {
			std::stringstream ss;
			ss << "Illegal redefinition of symbol: " << ident;
			return U8String(ss.str());
		}
	};

    template <>
	struct ErrorMessage<ErrorMessageKind::NON_RETURNING_PATHS> {
		U8String str(const U8String &ident) const {
			std::stringstream ss;
			ss << "Not all paths inside function '" << ident << "' return a value.";
			return U8String(ss.str());
		}
	};

    template <>
	struct ErrorMessage<ErrorMessageKind::UNDEFINED_REFERENCE> {
		U8String str(const U8String &ident) const {
			std::stringstream ss;
			ss << "Undefined reference to '" << ident << "'.";
			return U8String(ss.str());
		}
	};

    template <>
	struct ErrorMessage<ErrorMessageKind::ASSIGN_TO_RVALUE> {
		U8String str() const {
			std::stringstream ss;
			ss << "Left side of an assignment needs to be an l-value.";
			return U8String(ss.str());
		}
	};
}
