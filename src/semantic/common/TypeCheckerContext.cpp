#include "TypeCheckerContext.h"

#include "core/DefaultDecls.h"

namespace sem {
TypeCheckerContext::TypeCheckerContext(ErrorHandler &err)
	: m_GlobalNamespace(u8"global")
	, m_ErrorHandler(err) {
	for (const auto &[name, type] : s_DefaultDecls)
		m_GlobalNamespace.addFunction(name, type);
}

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