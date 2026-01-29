#pragma once
#include <format>

#include "Visitor.h"

namespace type {
///
/// Turn a type into a string representation for debug purposes.
///
struct Printer : ConstVisitor<void> {
	using Iterator = std::format_context::iterator;

private:
	Iterator m_Out;

public:
	explicit Printer(Iterator out);

	Iterator printType(const Type &t);

private:
	void print(const U8String &text);

	void visit(const Typename &t) override;
	void visit(const PointerType &t) override;
	void visit(const ArrayType &t) override;
	void visit(const FunctionType &t) override;
	void visit(const ErrorType &t) override;
	void visit(const UnitType &t) override;
};
}

template <typename T>
	requires std::derived_from<T, type::Type>
struct std::formatter<T> {
	constexpr auto parse(const std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const T &t, std::format_context &ctx) const {
		return type::Printer(ctx.out()).printType(t);
	}
};

template <>
struct std::formatter<type::TypeList> {
	constexpr auto parse(const std::format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const type::TypeList &l, std::format_context &ctx) const {
		for (size_t i = 0; i < l.size(); ++i) {
			if (i)
				std::format_to(ctx.out(), ", ");

			std::format_to(ctx.out(), "{}", *l[i]);
		}

		return ctx.out();
	}
};