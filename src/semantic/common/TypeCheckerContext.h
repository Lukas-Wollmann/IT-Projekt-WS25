#pragma once
#include "Namespace.h"
#include "OperatorTable.h"
#include "core/U8String.h"

namespace semantic {
	struct TypeCheckerContext {
	private:
		Namespace m_GlobalNamespace;
		OperatorTable m_OperatorTable;
		Vec<U8String> m_Errors;

	public:
		TypeCheckerContext();
		TypeCheckerContext(const TypeCheckerContext &) = delete;
		TypeCheckerContext(TypeCheckerContext &&) = delete;

		TypeCheckerContext &operator=(const TypeCheckerContext &) = delete;
		TypeCheckerContext &operator=(TypeCheckerContext &&) = delete;

		void addError(U8String msg);
		Namespace &getGlobalNamespace();
		const OperatorTable &getOperatorTable() const;

		const Vec<U8String> &getErrors() const;
	};
}