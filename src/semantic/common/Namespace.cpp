#include "Namespace.h"

#include "type/PrintVisitor.h"

namespace semantic {
	using namespace type;

	FunctionDeclInfo::FunctionDeclInfo(U8String name, Ptr<const FunctionType> func)
		: m_Name(name)
		, m_Type(func) {
		VERIFY(m_Type);
	}

	const FunctionType &FunctionDeclInfo::getType() const {
		return *m_Type;
	}

	const U8String &FunctionDeclInfo::getName() const {
		return m_Name;
	}

	Namespace::Namespace(U8String name)
		: m_Name(std::move(name)) {}

	void Namespace::addFunction(U8String name, Ptr<const type::FunctionType> func) {
		VERIFY(m_Functions.find(name) == m_Functions.end());

		m_Functions.emplace(std::move(name), std::move(func));
	}

	Opt<FunctionTypePtr> Namespace::getFunction(const U8String &name) const {
		auto func = m_Functions.find(name);

		if (func == m_Functions.end())
			return std::nullopt;

		return func->second;
	}
}

std::ostream &operator<<(std::ostream &os, const semantic::Namespace &ns) {
	os << "Namespace \"" << ns.m_Name << "\" {\n";

	for (auto &fn : ns.m_Functions)
		os << "    " << fn.first << ": " << *fn.second << "\n";

	return os << "}\n";
}