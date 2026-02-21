#pragma once
#include "Namespace.h"
#include "OperatorTable.h"
#include "core/ErrorHandler.h"
#include "core/U8String.h"

namespace sem {
struct TypeCheckerContext {
private:
	Namespace m_GlobalNamespace;
	OperatorTable m_OperatorTable;
	ErrorHandler &m_ErrorHandler;

public:
	explicit TypeCheckerContext(ErrorHandler &err);
	TypeCheckerContext(const TypeCheckerContext &) = delete;
	TypeCheckerContext(TypeCheckerContext &&) = delete;

	TypeCheckerContext &operator=(const TypeCheckerContext &) = delete;
	TypeCheckerContext &operator=(TypeCheckerContext &&) = delete;

	void submitError(U8String msg, const SourceLoc &loc,
					 ErrorLevel level = ErrorLevel::ERROR) const;
	Namespace &getGlobalNamespace();
	const OperatorTable &getOperatorTable() const;
};
}