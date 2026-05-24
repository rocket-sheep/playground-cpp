#include "debug/assert.hpp"

#include <cstdlib>
#include <iostream>

namespace playground {

void assert_failed(const char* condition, const char* function, int lineNo, const char* message)
{
    std::cerr << "Assertion failed: " << condition << ", in function " << function << ", line " << lineNo;
    if (message != nullptr) {
        std::cerr << ": " << message;
    }
    std::cerr << std::endl;
    abort();
}

} // namespace playground
