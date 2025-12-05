#pragma once
#include <vector>

#include "Namespace.h"

struct TypeCheckerContext {
private:
	Namespace m_GlobalNamespace;
	std::vector<std::string> m_Errors;

public:
	TypeCheckerContext();
	TypeCheckerContext(const TypeCheckerContext &) = delete;
	TypeCheckerContext(TypeCheckerContext &&) = delete;
	TypeCheckerContext &operator=(const TypeCheckerContext &) = delete;
	TypeCheckerContext &operator=(TypeCheckerContext &&) = delete;

	void addError(std::string msg);
	Namespace &getGlobalNamespace();

	const std::vector<std::string> &getErrors() const;
};