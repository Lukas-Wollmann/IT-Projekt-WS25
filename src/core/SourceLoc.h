#pragma once
#include "U8String.h"

struct SourceLoc {
	size_t line, column, index, length;
	U8String filename;

	bool operator==(const SourceLoc &other) const {
		return line == other.line && column == other.column && index == other.index &&
			   length == other.length;
	}

	bool operator!=(const SourceLoc &other) const {
		return !(*this == other);
	}
};

template <>
struct std::formatter<SourceLoc> {
	constexpr auto parse(const format_parse_context &ctx) {
		return ctx.begin();
	}

	auto format(const SourceLoc &loc, format_context &ctx) const {
		if (loc.filename.empty()) {
			return std::format_to(ctx.out(), "{{{}:{}, {}, {}}}", loc.line, loc.column, loc.index,
								  loc.length);
		}

		return std::format_to(ctx.out(), "{{{}:{}, {}, {}, {}}}", loc.filename, loc.line,
							  loc.column, loc.index, loc.length);
	}
};