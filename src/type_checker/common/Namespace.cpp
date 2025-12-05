#include "Namespace.h"

#include <cassert>

#include "type/PrintVisitor.h"

Namespace::Namespace(std::string name)
	: m_Name(name) {}

void Namespace::addFunction(std::string name, Box<const type::FunctionType> func) {
	assert(m_Functions.find(name) == m_Functions.end());

	m_Functions.emplace(std::move(name), std::move(func));
}

std::optional<Ref<const type::FunctionType>> Namespace::getFunction(const std::string &name) const {
	auto it = m_Functions.find(name);

	if (it != m_Functions.end())
		return *it->second;

	return std::nullopt;
}

std::ostream &operator<<(std::ostream &os, const Namespace &ns) {
	os << "Namespace \"" << ns.m_Name << "\" {\n";

	for (auto &fn : ns.m_Functions)
		os << "    " << fn.first << ": " << *fn.second << "\n";

	return os << "}\n";
}