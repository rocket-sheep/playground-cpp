#pragma once

#include "base/noop.h"

namespace playground {

[[noreturn]]
void assert_failed(const char* condition, const char* function, int lineNo, const char* message = nullptr);

#define assert_always(condition) assert_always_msg(condition, nullptr)

#define assert_always_msg(condition, message) \
    static_cast<bool>(condition) \
        ? NOOP \
        : assert_failed(#condition, __PRETTY_FUNCTION__, __LINE__, message)

#if defined(NDEBUG)
    #define assert_debug(condition) NOOP
    #define assert_debug_msg(condition, message) NOOP
#else
    #define assert_debug(condition) assert_always(condition)
    #define assert_debug_msg(condition, message) assert_always_msg(condition, message)
#endif

} // namespace playground
