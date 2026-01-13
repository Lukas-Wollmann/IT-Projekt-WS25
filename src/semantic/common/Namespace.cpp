#include "Namespace.h"

#include "type/Printer.h"

namespace semantic {
	using namespace type;

	Namespace::Namespace(U8String name)
		: m_Name(std::move(name)) {}

	void Namespace::addFunction(U8String name, FunctionTypePtr func) {
		VERIFY(!m_Functions.contains(name));

		m_Functions.emplace(std::move(name), std::move(func));
	}

	Opt<FunctionTypePtr> Namespace::getFunction(const U8String &name) const {
		const auto func = m_Functions.find(name);

		if (func == m_Functions.end())
			return {};

		return func->second;
	}

	size_t Namespace::getSize() const {
		return m_Functions.size();
	}

	std::ostream &operator<<(std::ostream &os, const Namespace &ns) {
		os << "Namespace \"" << ns.m_Name << "\" {\n";

		for (const auto &[name, fn] : ns.m_Functions)
			os << "    " << name << ": " << *fn << "\n";

		return os << "}\n";
	}
}