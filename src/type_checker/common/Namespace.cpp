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

Opt<Ref<const FunctionType>> Namespace::getFunction(const U8String &name, Vec<const Type> &params) const {
	auto overloadedFuncIt = m_Functions.find(name);

	if (overloadedFuncIt == m_Functions.end())
		return std::nullopt;
	
	auto &overloadedFunc = overloadedFuncIt->second;
	
	auto funcIt = std::find(overloadedFunc.begin(), overloadedFunc.end(), params);

	return std::nullopt;
}

std::ostream &operator<<(std::ostream &os, const Namespace &ns) {
	os << "Namespace \"" << ns.m_Name << "\" {\n";

	for (auto &fn : ns.m_Functions)
		os << "    " << fn.first << ": " << *fn.second << "\n";

	return os << "}\n";
}