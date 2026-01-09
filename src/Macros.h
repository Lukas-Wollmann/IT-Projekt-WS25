#include <sstream>
#include <stdexcept>

#define UNREACHABLE()                                                                              \
	do {                                                                                           \
		std::stringstream ss;                                                                      \
		ss << __func__ << " in " << __FILE__ << ":" << __LINE__ << " should never be reachable";   \
		throw std::logic_error(ss.str());                                                          \
	} while (0)

#define VERIFY(expr)                                                                               \
	do {                                                                                           \
		if (!(expr)) {                                                                             \
			std::stringstream ss;                                                                  \
			ss << "Assertion failed: " << #expr << " at " << __FILE__ << ":" << __LINE__;          \
			throw std::runtime_error(ss.str());                                                    \
		}                                                                                          \
	} while (0)