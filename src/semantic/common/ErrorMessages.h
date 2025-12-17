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
        
		ASSIGNMENT_INCOMPATIBLE_TYPES,
		ASSIGNMENT_OPERATOR_INCOMPATIBLE_TYPES,
		UNKNOWN_SYMBOL,
		CANNOT_ASSIGN_TO_RVALUE,
		IF_CONDITION_INVALID_TYPE,
		WHILE_CONDITION_INVALID_TYPE,
		RETURN_TYPE_MISMATCH,
		VARIABLE_DECL_TYPE_MISMATCH,
		ARRAY_ELEMENT_TYPE_MISMATCH,
		ARG_TYPE_MISSMATCH_PARAM
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


	//////////////////

	template <>
	struct ErrorMessage<ErrorMessageKind::ASSIGNMENT_INCOMPATIBLE_TYPES> {
		U8String str(type::TypePtr left, type::TypePtr right) const {
			std::stringstream ss;
			ss << "Cannot assign using type '" << *right << " to type '" << *left << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::ASSIGNMENT_OPERATOR_INCOMPATIBLE_TYPES> {
		U8String str(ast::BinaryOpKind kind, type::TypePtr left, type::TypePtr right,
					 type::TypePtr res) const {
			std::stringstream ss;
			ss << "Cannot assign using '" << kind << "=': left type is '" << *left
			   << "', right side ('" << *left << " " << kind << " " << *right << "') yields type '"
			   << *res << "'. Expected type '" << *left << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::UNKNOWN_SYMBOL> {
		U8String str(const U8String &ident) const {
			std::stringstream ss;
			ss << "Undefined identifier: " << ident;
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::CANNOT_ASSIGN_TO_RVALUE> {
		U8String str() const {
			return U8String("Cannot assign to r-value.");
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::IF_CONDITION_INVALID_TYPE> {
		U8String str(type::TypePtr type) const {
			std::stringstream ss;
			ss << "Cannot accept type " << *type << " inside an if condition.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::WHILE_CONDITION_INVALID_TYPE> {
		U8String str(type::TypePtr type) const {
			std::stringstream ss;
			ss << "Cannot accept type " << *type << " inside a while condition.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::RETURN_TYPE_MISMATCH> {
		U8String str(type::TypePtr given, type::TypePtr expected) const {
			std::stringstream ss;
			ss << "The type " << *given << " does not match function declaration. ";
			ss << "Expected type: " << *expected;
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::VARIABLE_DECL_TYPE_MISMATCH> {
		U8String str(type::TypePtr expected, type::TypePtr given) const {
			std::stringstream ss;
			ss << "Mismatching types at variable declaration: ";
			ss << "Expected " << *expected << " but got " << *given;
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::ARRAY_ELEMENT_TYPE_MISMATCH> {
		U8String str(type::TypePtr expected, type::TypePtr found) const {
			std::stringstream ss;
			ss << "Array element type mismatch: expected '" << *expected << "', found '" << *found
			   << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct ErrorMessage<ErrorMessageKind::ARG_TYPE_MISSMATCH_PARAM> {
		U8String str(type::TypePtr arg, type::TypePtr param) const {
			std::stringstream ss;
			ss << "Canno't use a value of type '" << *arg << "' for a paramteter of type '"
			   << *param << "'.";
			return U8String(ss.str());
		}
	};
}
