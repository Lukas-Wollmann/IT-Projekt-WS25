#include "Namespace.h"

#include "type/Printer.h"

namespace sem {
Namespace::Namespace(U8String name)
	: m_Name(std::move(name)) {}

void Namespace::addFunction(U8String name, type::FunctionTypePtr func) {
	VERIFY(!m_Functions.contains(name));

	m_Functions.emplace(std::move(name), std::move(func));
}

Opt<type::FunctionTypePtr> Namespace::getFunction(const U8String &name) const {
	const auto func = m_Functions.find(name);

	if (func == m_Functions.end())
		return {};

	return func->second;
}

size_t Namespace::getSize() const {
	return m_Functions.size();
}
}