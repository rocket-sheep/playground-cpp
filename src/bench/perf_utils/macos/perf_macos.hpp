#pragma once

#if !__APPLE__
    #error This profiler only works on macOS
#endif

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <vector>

#include "perf_utils/PerfCounters.hpp"

namespace perf_utils {

struct EventSpec;

class PerfCounterProfiler
{
public:
    enum class Status {
        UNKNOWN = -1,
        OK = 0,
        LIB_INIT_FAILED,
        PERMISSION_DENIED,
        DB_LOAD_FAILED,
        CONFIGURATION_FAILED,
        COUNTING_FAILED,
    };

public:
    inline static void enable_counters(const std::span<const PerfCounter>& perf_counters)
    {
        _enabled_counters.clear();
        std::copy(perf_counters.begin(), perf_counters.end(), std::back_inserter(_enabled_counters));
    }

    inline static PerfCounterProfiler& get()
    {
        if (_instance._status == Status::UNKNOWN)
            _instance._status = _instance._init();
        return _instance;
    }

    inline Status status() const { return _status; }
    PerfCounters get_counters() const;

    Status start();
    Status stop();

private:
    PerfCounterProfiler() = default;
    Status _init();

private:
    static PerfCounterProfiler _instance;
    static std::vector<PerfCounter> _enabled_counters;

private:
    std::vector<const EventSpec*> _enabled_event_specs;
    Status _status = Status::UNKNOWN;
    uint32_t classes = 0;
    static constexpr size_t KPC_MAX_COUNTERS = 32;
    size_t _counter_map[KPC_MAX_COUNTERS] = { 0 };
    uint64_t _counters_before[KPC_MAX_COUNTERS] = { 0 };
    uint64_t _counters_after[KPC_MAX_COUNTERS] = { 0 };
};

} // namespace perf_utils
