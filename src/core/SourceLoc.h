#pragma once
#include "U8String.h"

struct SourceLoc {
	size_t line, column, index;
	U8String filename = u8"";
};

inline std::ostream &operator<<(std::ostream &os, const SourceLoc &loc) {
	os << "{" << loc.filename << ":" << loc.line << ":" << loc.column;
	return os << ", " << loc.index << "}";
}