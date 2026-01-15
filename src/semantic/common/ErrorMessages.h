#pragma once
#include <sstream>

#include "ast/AST.h"
#include "type/Printer.h"
#include "type/Type.h"

namespace semantic {
	enum struct ErrorMessageKind {
		TypeMissmatch,
		DereferenceNonPointerType,
		UnaryOperatorNotFound,
		BinaryOperatorNotFound,
		CallOnNonFunctionType,
		TooManyArguments,
		UnreachableStatement,
		SymbolRedefinition,
		NonReturningPaths,
		UndefinedReference,
		AssignToRValue
	};

	template <ErrorMessageKind E>
	struct ErrorMessage;

	template <>
	struct ErrorMessage<ErrorMessageKind::TypeMissmatch> {
		[[nodiscard]] static U8String str(const type::TypePtr &expected,
										  const type::TypePtr &actual) {
			std::stringstream ss;
			ss << "Expected value of type '" << *expected;
			ss << "', got type '" << *actual << "' instead.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::DereferenceNonPointerType> {
		[[nodiscard]] static U8String str(const type::TypePtr &type) {
			std::stringstream ss;
			ss << "Cannot dereference the non-pointer type '" << *type << "'.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::UnaryOperatorNotFound> {
		[[nodiscard]] static U8String str(const type::TypePtr &type, const ast::UnaryOpKind op) {
			std::stringstream ss;
			ss << "Cannot use unary operator '" << op << "' on a value of type '" << *type << "'.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::BinaryOperatorNotFound> {
		[[nodiscard]] static U8String str(const type::TypePtr &left, const type::TypePtr &right,
										  const ast::BinaryOpKind op) {
			std::stringstream ss;
			ss << "Cannot use binary operator '" << op << "' on values of type '";
			ss << *left << "' and '" << *right << "'.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::CallOnNonFunctionType> {
		[[nodiscard]] static U8String str(const type::TypePtr &type) {
			std::stringstream ss;
			ss << "Cannot call the non-function type '" << *type << "'.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::TooManyArguments> {
		[[nodiscard]] static U8String str(const size_t expected, const size_t actual) {
			std::stringstream ss;
			ss << "Expected " << expected << " arguments, found " << actual << ".";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::UnreachableStatement> {
		[[nodiscard]] static U8String str() {
			return U8String("Found unreachable statement after return.");
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::SymbolRedefinition> {
		[[nodiscard]] static U8String str(const U8String &ident) {
			std::stringstream ss;
			ss << "Illegal redefinition of symbol: '" << ident << "'.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::NonReturningPaths> {
		[[nodiscard]] static U8String str(const U8String &ident) {
			std::stringstream ss;
			ss << "Not all paths inside function '" << ident << "' return a value.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::UndefinedReference> {
		[[nodiscard]] static U8String str(const U8String &ident) {
			std::stringstream ss;
			ss << "Undefined reference to '" << ident << "'.";

			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::AssignToRValue> {
		[[nodiscard]] static U8String str() {
			std::stringstream ss;
			ss << "Left side of an assignment needs to be an l-value.";

			return U8String(ss.str());
		}
	};
}
