#pragma once

#if __APPLE__
    #error Please use the macOS implementation
#else

#include <span>

#include "perf_utils/PerfCounters.hpp"

namespace perf_utils {

class PerfCounterProfiler
{
public:
    enum class Status {
        OK = 0,
        NOT_SUPPORTED,
    };

public:
    inline static void enable_counters(const std::span<const PerfCounter>& perf_counters) {}

    static PerfCounterProfiler& get()
    {
        return _instance;
    }

    inline Status status() const { return Status::NOT_SUPPORTED; }
    PerfCounters get_counters() { return {}; }

    Status start() { return Status::NOT_SUPPORTED; }
    Status stop() { return Status::NOT_SUPPORTED; }

private:
    PerfCounterProfiler() = default;

private:
    static PerfCounterProfiler _instance;
};

} // namespace perf_utils

#endif
