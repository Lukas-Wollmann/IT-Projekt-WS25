#include "TypeCheckerContext.h"

namespace semantic {
	TypeCheckerContext::TypeCheckerContext()
		: m_GlobalNamespace(u8"global") {}

	void TypeCheckerContext::addError(std::string msg) {
		m_Errors.push_back(std::move(msg));
	}

	Namespace &TypeCheckerContext::getGlobalNamespace() {
		return m_GlobalNamespace;
	}

	const std::vector<std::string> &TypeCheckerContext::getErrors() const {
		return m_Errors;
	}
}