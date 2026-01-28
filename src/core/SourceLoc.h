#pragma once
#include "U8String.h"

struct SourceLoc {
	size_t line, column, index, length;
};

template <>
struct std::formatter<SourceLoc> {
	constexpr auto parse(format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const SourceLoc &loc, format_context &ctx) const {
		return std::format_to(ctx.out(), "{{{}:{}, {}, {}}}", loc.line, loc.column, loc.index,
							  loc.length);
	}
};