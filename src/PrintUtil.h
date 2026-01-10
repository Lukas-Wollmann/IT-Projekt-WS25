#pragma once
#include <format>
#include <iostream>
#include <sstream>
#include <vector>

template <typename T>
concept Printable = requires(std::ostream &os, const T &t) {
	{ os << t } -> std::same_as<std::ostream &>;
};

template <Printable T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
	os << "{ ";

	for (size_t i = 0; i < vec.size(); ++i) {
		if (i > 0)
			os << ", ";

		os << vec[i];
	}

	return os << " }";
}