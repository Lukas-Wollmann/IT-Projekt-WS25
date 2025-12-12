#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Declaration.h"
#include "ast/AST.h"
#include "core/U8String.h"

struct Namespace {
private:
	std::string m_Name;
	std::unordered_map<U8String, Box<const type::FunctionType>> m_Functions;

public:
	Namespace(std::string name);
	Namespace(const Namespace &) = delete;
	Namespace(Namespace &&) = delete;

	Namespace &operator=(const Namespace &) = delete;
	Namespace &operator=(Namespace &&) = delete;

	void addFunction(U8String name, Box<const type::FunctionType> func);
	Opt<Ref<const type::FunctionType>> getFunction(const U8String &name) const;

	void addInternalOperatorDecls();

private:
	void addInternalBinaryOperator(ast::BinaryOpKind op, type::PrimitiveTypeKind type);

	friend std::ostream &operator<<(std::ostream &os, const Namespace &ns);
};