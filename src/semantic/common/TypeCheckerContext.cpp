#include "TypeCheckerContext.h"

namespace semantic {
	TypeCheckerContext::TypeCheckerContext(ErrorHandler &err)
		: m_GlobalNamespace(u8"global")
		, m_ErrorHandler(err) {}

	void TypeCheckerContext::submitError(U8String msg, SourceLoc loc, ErrorLevel level) {
		m_ErrorHandler.addError(std::move(msg), loc, level);
	}

	Namespace &TypeCheckerContext::getGlobalNamespace() {
		return m_GlobalNamespace;
	}

	const OperatorTable &TypeCheckerContext::getOperatorTable() const {
		return m_OperatorTable;
	}
}