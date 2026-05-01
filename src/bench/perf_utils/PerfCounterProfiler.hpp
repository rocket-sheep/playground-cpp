#pragma once

#if __APPLE__
    #include "perf_utils/macos/perf_macos.hpp"
#else
    #include "perf_noop.hpp"
#endif
