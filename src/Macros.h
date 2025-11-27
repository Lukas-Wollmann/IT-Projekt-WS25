#include <sstream>
#include <stdexcept>

#define UNREACHABLE() \
    throw std::logic_error(std::string(__func__) + " should never be reachable")

#define VERIFY(expr) \
    do { \
        if (!(expr)) {\
            std::stringstream ss; \
            ss << "Assertion failed: " << #expr << " at " <<__FILE__ << ":" << __LINE__; \
            throw std::runtime_error(ss.str()); \
        } \
    } while(0)