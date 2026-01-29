#include "TypeCheckerContext.h"

namespace sem {
TypeCheckerContext::TypeCheckerContext(ErrorHandler &err)
	: m_GlobalNamespace(u8"global")
	, m_ErrorHandler(err) {}

void TypeCheckerContext::submitError(U8String msg, const SourceLoc &loc,
									 const ErrorLevel level) const {
	m_ErrorHandler.addError(std::move(msg), loc, level);
}

Namespace &TypeCheckerContext::getGlobalNamespace() {
	return m_GlobalNamespace;
}

const OperatorTable &TypeCheckerContext::getOperatorTable() const {
	return m_OperatorTable;
}
}