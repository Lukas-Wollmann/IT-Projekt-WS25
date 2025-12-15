#include "TypeCheckerContext.h"

namespace semantic {
	TypeCheckerContext::TypeCheckerContext()
		: m_GlobalNamespace(u8"global") {}

	void TypeCheckerContext::addError(std::string msg) {
		m_Errors.push_back(U8String(msg));
	}

	void TypeCheckerContext::addError(U8String msg) {
		m_Errors.push_back(std::move(msg));
	}

	Namespace &TypeCheckerContext::getGlobalNamespace() {
		return m_GlobalNamespace;
	}

    const OperatorTable &TypeCheckerContext::getOperatorTable() const {
		return m_OperatorTable;
	}

	const Vec<U8String> &TypeCheckerContext::getErrors() const {
		return m_Errors;
	}
}