#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "ast/AST.h"
#include "core/U8String.h"

namespace semantic {
	struct Namespace;
}

std::ostream &operator<<(std::ostream &os, const semantic::Namespace &ns);

namespace semantic {
	struct FunctionDeclInfo {
	private:
		U8String m_Name;
		Ptr<const type::FunctionType> m_Type;

	public:
		FunctionDeclInfo(U8String, Ptr<const type::FunctionType> func);
		FunctionDeclInfo(const FunctionDeclInfo &) = delete;
		FunctionDeclInfo(FunctionDeclInfo &&) = default;

		FunctionDeclInfo &operator=(const FunctionDeclInfo &) = delete;
		FunctionDeclInfo &operator=(FunctionDeclInfo &&) = delete;

		const type::FunctionType &getType() const;
		const U8String &getName() const;
	};

	struct Namespace {
	private:
		U8String m_Name;
		std::unordered_map<U8String, type::FunctionTypePtr> m_Functions;

	public:
		Namespace(U8String name);
		Namespace(const Namespace &) = delete;
		Namespace(Namespace &&) = delete;

		Namespace &operator=(const Namespace &) = delete;
		Namespace &operator=(Namespace &&) = delete;

		void addFunction(U8String name, Ptr<const type::FunctionType> func);
		Opt<type::FunctionTypePtr> getFunction(const U8String &name) const;

		friend std::ostream & ::operator<<(std::ostream &os, const Namespace &ns);
	};
}
