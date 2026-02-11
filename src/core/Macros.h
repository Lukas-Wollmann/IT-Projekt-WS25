#pragma once
#include <stdexcept>

#define STR_HELPER(x) #x
#define STR(x)		  STR_HELPER(x)

#define UNREACHABLE()                                                                              \
	throw std::logic_error(std::string(__func__) + " in " __FILE__                                 \
												   ":" STR(__LINE__) " should be unreachable")

#define VERIFY(expr)                                                                               \
	/* NOLINTNEXTLINE(*-avoid-do-while) */                                                         \
	do {                                                                                           \
		if (!(expr))                                                                               \
			throw std::runtime_error("Assertion failed: " #expr " at " __FILE__                    \
									 ":" STR(__LINE__));                                           \
	} while (0)
