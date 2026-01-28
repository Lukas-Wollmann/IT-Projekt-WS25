#pragma once
#include "ast/AST.h"
#include "core/U8String.h"

namespace semantic {
	struct Namespace {
	private:
		U8String m_Name;
		Map<U8String, type::FunctionTypePtr> m_Functions;

	public:
		explicit Namespace(U8String name);
		Namespace(const Namespace &) = delete;
		Namespace(Namespace &&) = delete;

		Namespace &operator=(const Namespace &) = delete;
		Namespace &operator=(Namespace &&) = delete;

		void addFunction(U8String name, type::FunctionTypePtr func);
		Opt<type::FunctionTypePtr> getFunction(const U8String &name) const;
		size_t getSize() const;
	};
}
