#pragma once
#include "U8String.h"

struct SourceLoc {
	U8String filename;
	size_t line, column, index;
};