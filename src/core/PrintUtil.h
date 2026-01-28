#pragma once
#include <format>
#include <iostream>

#include "Typedef.h"

template <typename T>
concept Printable = requires(std::ostream &os, const T &t) {
	{ os << t } -> std::same_as<std::ostream &>;
};

template <Printable T>
std::ostream &operator<<(std::ostream &os, const Vec<T> &vec) {
	os << "{ ";

	for (size_t i = 0; i < vec.size(); ++i) {
		if (i > 0)
			os << ", ";

		os << vec[i];
	}

	return os << " }";
}

namespace util {
	template <typename... Args>
	void print(std::format_string<Args...> fmt, Args &&...args) {
		std::cout << std::format(fmt, std::forward<Args>(args)...);
	}
}