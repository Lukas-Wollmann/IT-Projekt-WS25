#pragma once
#include <sstream>

#include "ast/AST.h"
#include "type/PrintVisitor.h"
#include "type/Type.h"

namespace semantic {
	enum struct ErrorKind {
		ASSIGNMENT_INCOMPATIBLE_TYPES,
		ASSIGNMENT_OPERATOR_INCOMPATIBLE_TYPES,
		UNKNOWN_SYMBOL,
		CANNOT_ASSIGN_TO_RVALUE,
		UNREACHABLE_STATEMENT,
		IF_CONDITION_INVALID_TYPE,
		WHILE_CONDITION_INVALID_TYPE,
		RETURN_TYPE_MISMATCH,
		VARIABLE_REDEFINITION,
		VARIABLE_DECL_TYPE_MISMATCH,
		ARRAY_ELEMENT_TYPE_MISMATCH,
		BINARY_OPERATOR_NOT_FOUND,
		UNARY_OPERATOR_NOT_FOUND,
		DEREFERENCE_NON_POINTER,
		FUNC_CALL_NON_FUNCTION,
		FUNC_CALL_ARG_MISMATCH,
		MISSING_RETURN_PATH,
		ARG_TYPE_MISSMATCH_PARAM
	};

	template <ErrorKind E>
	struct Error;

	template <>
	struct Error<ErrorKind::ASSIGNMENT_INCOMPATIBLE_TYPES> {
		U8String str(type::TypePtr left, type::TypePtr right) const {
			std::stringstream ss;
			ss << "Cannot assign using type '" << *right << " to type '" << *left << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::ASSIGNMENT_OPERATOR_INCOMPATIBLE_TYPES> {
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
	struct Error<ErrorKind::UNKNOWN_SYMBOL> {
		U8String str(const U8String &ident) const {
			std::stringstream ss;
			ss << "Undefined identifier: " << ident;
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::CANNOT_ASSIGN_TO_RVALUE> {
		U8String str() const {
			return U8String("Cannot assign to r-value.");
		}
	};

	template <>
	struct Error<ErrorKind::UNREACHABLE_STATEMENT> {
		U8String str() const {
			return U8String("Unreachable statements detected after a return statement.");
		}
	};

	template <>
	struct Error<ErrorKind::IF_CONDITION_INVALID_TYPE> {
		U8String str(type::TypePtr type) const {
			std::stringstream ss;
			ss << "Cannot accept type " << *type << " inside an if condition.";
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::WHILE_CONDITION_INVALID_TYPE> {
		U8String str(type::TypePtr type) const {
			std::stringstream ss;
			ss << "Cannot accept type " << *type << " inside a while condition.";
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::RETURN_TYPE_MISMATCH> {
		U8String str(type::TypePtr given, type::TypePtr expected) const {
			std::stringstream ss;
			ss << "The type " << *given << " does not match function declaration. ";
			ss << "Expected type: " << *expected;
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::VARIABLE_REDEFINITION> {
		U8String str(const U8String &ident) const {
			std::stringstream ss;
			ss << "Illegal redefinition of symbol: " << ident;
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::VARIABLE_DECL_TYPE_MISMATCH> {
		U8String str(type::TypePtr expected, type::TypePtr given) const {
			std::stringstream ss;
			ss << "Mismatching types at variable declaration: ";
			ss << "Expected " << *expected << " but got " << *given;
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::ARRAY_ELEMENT_TYPE_MISMATCH> {
		U8String str(type::TypePtr expected, type::TypePtr found) const {
			std::stringstream ss;
			ss << "Array element type mismatch: expected '" << *expected << "', found '" << *found
			   << "'.";
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::BINARY_OPERATOR_NOT_FOUND> {
		U8String str(type::TypePtr left, type::TypePtr right, ast::BinaryOpKind op) const {
			std::stringstream ss;
			ss << "Found no binary operator '" << op << "' for types " << *left << " and "
			   << *right;
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::UNARY_OPERATOR_NOT_FOUND> {
		U8String str(const type::TypePtr type, ast::UnaryOpKind op) const {
			std::stringstream ss;
			ss << "Found no unary operator '" << op << "' for type " << *type;
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::DEREFERENCE_NON_POINTER> {
		U8String str(const type::TypePtr type) const {
			std::stringstream ss;
			ss << "Cannot dereference a value of type " << *type << ", expected a pointer type.";
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::FUNC_CALL_NON_FUNCTION> {
		U8String str() const {
			return U8String("Cannot call a non-function-type expression");
		}
	};

	template <>
	struct Error<ErrorKind::FUNC_CALL_ARG_MISMATCH> {
		U8String str(size_t expected, size_t given) const {
			std::stringstream ss;
			ss << "Provided wrong number of arguments. Expected " << expected << " but got "
			   << given;
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::MISSING_RETURN_PATH> {
		U8String str(const U8String &funcName) const {
			std::stringstream ss;
			ss << "Not all control flow paths in function '" << funcName << "' return a value.";
			return U8String(ss.str());
		}
	};

	template <>
	struct Error<ErrorKind::ARG_TYPE_MISSMATCH_PARAM> {
		U8String str(type::TypePtr arg, type::TypePtr param) const {
			std::stringstream ss;
			ss << "Canno't use a value of type '" << *arg << "' for a paramteter of type '"
			   << *param << "'.";
			return U8String(ss.str());
		}
	};
}
