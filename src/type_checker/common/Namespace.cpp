#include "Namespace.h"

#include <cassert>

#include "type/PrintVisitor.h"

using namespace type;

Namespace::Namespace(std::string name)
	: m_Name(name) {}

void Namespace::addFunction(U8String name, Box<const type::FunctionType> func) {
	assert(m_Functions.find(name) == m_Functions.end());

	m_Functions.emplace(std::move(name), std::move(func));
}

Opt<Ref<const FunctionType>> Namespace::getFunction(const U8String &name) const {
	auto func = m_Functions.find(name);

	if (func == m_Functions.end())
		return std::nullopt;

	return *func->second;
}

void Namespace::addInternalOperatorDecls() {
	using enum PrimitiveTypeKind;
	using enum ast::BinaryOpKind;

	// Arithmetic
	addInternalBinaryOperator(Addition, I32);
	addInternalBinaryOperator(Addition, U32);
	addInternalBinaryOperator(Addition, F32);
	addInternalBinaryOperator(Addition, String);

	addInternalBinaryOperator(Subtraction, I32);
	addInternalBinaryOperator(Subtraction, U32);
	addInternalBinaryOperator(Subtraction, F32);

	addInternalBinaryOperator(Multiplication, I32);
	addInternalBinaryOperator(Multiplication, U32);
	addInternalBinaryOperator(Multiplication, F32);

	addInternalBinaryOperator(Division, I32);
	addInternalBinaryOperator(Division, U32);
	addInternalBinaryOperator(Division, F32);

	addInternalBinaryOperator(Modulo, I32);
	addInternalBinaryOperator(Modulo, U32);

	// Comparison
	addInternalBinaryOperator(Equality, I32);
	addInternalBinaryOperator(Equality, U32);
	addInternalBinaryOperator(Equality, F32);
	addInternalBinaryOperator(Equality, Bool);
	addInternalBinaryOperator(Equality, Char);
	addInternalBinaryOperator(Equality, String);

	addInternalBinaryOperator(Inequality, I32);
	addInternalBinaryOperator(Inequality, U32);
	addInternalBinaryOperator(Inequality, F32);
	addInternalBinaryOperator(Inequality, Bool);
	addInternalBinaryOperator(Inequality, Char);
	addInternalBinaryOperator(Inequality, String);

	addInternalBinaryOperator(LessThan, I32);
	addInternalBinaryOperator(LessThan, U32);
	addInternalBinaryOperator(LessThan, F32);
	addInternalBinaryOperator(LessThan, Char);
	addInternalBinaryOperator(LessThan, String);

	
	addInternalBinaryOperator(GreaterThan, I32);
	addInternalBinaryOperator(GreaterThan, U32);
	addInternalBinaryOperator(GreaterThan, F32);
	addInternalBinaryOperator(GreaterThan, Char);
	addInternalBinaryOperator(GreaterThan, String);

	
	addInternalBinaryOperator(GreaterThanOrEqual, I32);
	addInternalBinaryOperator(GreaterThanOrEqual, U32);
	addInternalBinaryOperator(GreaterThanOrEqual, F32);
	addInternalBinaryOperator(GreaterThanOrEqual, Char);
	addInternalBinaryOperator(GreaterThanOrEqual, String);

	// Logical
	addInternalBinaryOperator(LogicalAnd, Bool);
	addInternalBinaryOperator(LogicalOr, Bool);

	// Bitwise
	addInternalBinaryOperator(BitwiseAnd, I32);
	addInternalBinaryOperator(BitwiseAnd, U32);

	addInternalBinaryOperator(BitwiseOr, I32);
	addInternalBinaryOperator(BitwiseOr, U32);

	addInternalBinaryOperator(BitwiseXor, I32);
	addInternalBinaryOperator(BitwiseXor, U32);

	addInternalBinaryOperator(LeftShift, I32);
	addInternalBinaryOperator(LeftShift, U32);

	addInternalBinaryOperator(RightShift, I32);
	addInternalBinaryOperator(RightShift, U32);
}

void Namespace::addInternalBinaryOperator(ast::BinaryOpKind op, PrimitiveTypeKind type) {
	Params params;
	params.push_back(std::make_unique<PrimitiveType>(type));
	params.push_back(std::make_unique<PrimitiveType>(type));

	auto func =
			std::make_unique<FunctionType>(std::move(params), std::make_unique<PrimitiveType>(type));

	std::stringstream ss;
	ss << "operator" << op << "<" << type << "," << type << ">";

	addFunction(U8String(ss.str()), std::move(func));
}

std::ostream &operator<<(std::ostream &os, const Namespace &ns) {
	os << "Namespace \"" << ns.m_Name << "\" {\n";

	for (auto &fn : ns.m_Functions)
		os << "    " << fn.first << ": " << *fn.second << "\n";

	return os << "}\n";
}