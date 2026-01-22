#pragma once
#include "U8String.h"

struct SourceLoc {
	size_t line, column, index;
};

inline std::ostream &operator<<(std::ostream &os, const SourceLoc &loc) {
	os << "{" << ":" << loc.line << ":" << loc.column;
	return os << ", " << loc.index << "}";
}