#pragma once
#include "type/Type.h"

struct FunctionDeclaration {
private:
	std::string m_Name;
	Box<const type::FunctionType> m_Type;

public:
	FunctionDeclaration(std::string name, Box<const type::FunctionType> type);
	FunctionDeclaration(const FunctionDeclaration &) = delete;
	FunctionDeclaration(FunctionDeclaration &&) = default;

	FunctionDeclaration &operator=(const FunctionDeclaration &) = delete;
	FunctionDeclaration &operator=(FunctionDeclaration &&) = delete;

	const type::FunctionType &getType() const;
	const std::string &getName() const;
};