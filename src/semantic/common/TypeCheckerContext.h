#pragma once
#include <vector>

#include "Namespace.h"
#include "core/U8String.h"

namespace semantic {
	struct TypeCheckerContext {
	private:
		Namespace m_GlobalNamespace;
		std::vector<U8String> m_Errors;

	public:
		TypeCheckerContext();
		TypeCheckerContext(const TypeCheckerContext &) = delete;
		TypeCheckerContext(TypeCheckerContext &&) = delete;
		TypeCheckerContext &operator=(const TypeCheckerContext &) = delete;
		TypeCheckerContext &operator=(TypeCheckerContext &&) = delete;

		void addError(std::string msg);
        void addError(U8String msg);
		Namespace &getGlobalNamespace();

		const Vec<U8String> &getErrors() const;
	};
}