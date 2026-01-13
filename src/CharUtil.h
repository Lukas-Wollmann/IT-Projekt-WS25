#pragma once

namespace util {
	constexpr bool isAlpha(char32_t c) {
		return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z');
	}

	constexpr bool isNum(char32_t c) {
		return c >= U'0' && c <= U'9';
	}

	constexpr bool isAlNum(char32_t c) {
		return isAlpha(c) || isNum(c);
	}
}