#pragma once
#include "core/Operators.h"

namespace sem {
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
	[[nodiscard]] static U8String str(Type expected, Type actual) {
		return std::format("Expected value of type '{}', got type '{}' instead.", *expected,
						   *actual);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::DereferenceNonPointerType> {
	[[nodiscard]] static U8String str(Type type) {
		return std::format("Cannot dereference the non-pointer type '{}'.", *type);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::UnaryOperatorNotFound> {
	[[nodiscard]] static U8String str(Type type, const UnaryOpKind op) {
		return std::format("Cannot use unary operator '{}' on a value of type '{}'.", op, *type);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::BinaryOperatorNotFound> {
	[[nodiscard]] static U8String str(Type left, Type right, const BinaryOpKind op) {
		return std::format("Cannot use binary operator '{}' on values of type '{}' and '{}'.", op,
						   *left, *right);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::CallOnNonFunctionType> {
	[[nodiscard]] static U8String str(Type type) {
		return std::format("Cannot call the non-function type '{}'.", *type);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::TooManyArguments> {
	[[nodiscard]] static U8String str(const size_t expected, const size_t actual) {
		return std::format("Expected {} arguments, found {}.", expected, actual);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::UnreachableStatement> {
	[[nodiscard]] static U8String str() {
		return u8"Found unreachable statement after return.";
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::SymbolRedefinition> {
	[[nodiscard]] static U8String str(const U8String &ident) {
		return std::format("Illegal redefinition of symbol: '{}'.", ident);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::NonReturningPaths> {
	[[nodiscard]] static U8String str(const U8String &ident) {
		return std::format("Not all paths inside function '{}' return a value.", ident);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::UndefinedReference> {
	[[nodiscard]] static U8String str(const U8String &ident) {
		return std::format("Undefined reference to '{}'.", ident);
	}
};

template <>
struct ErrorMessage<ErrorMessageKind::AssignToRValue> {
	[[nodiscard]] static U8String str() {
		return std::format("Left side of an assignment needs to be an l-value.");
	}
};
}
