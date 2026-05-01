#pragma once

#include <functional>

#include <benchmark/benchmark.h>

#include "perf_utils/PerfCounterProfiler.hpp"

namespace perf_utils {

/// Records performance counters into a Google Benchmark state.
/// The recorder is passed a state at construction, records counters within
/// specific portions of code, and reports the result in the state when
/// exiting scope.
class BenchmarkPerfCounterRecorder
{
public:
    explicit BenchmarkPerfCounterRecorder(benchmark::State& state)
        : _state(state)
        , _profiler(PerfCounterProfiler::get())
    {}

    void record(const std::function<void()>& f)
    {
        if (_profiler.status() == PerfCounterProfiler::Status::OK) {
            _profiler.start();
            f();
            _profiler.stop();
            _perf_counters += _profiler.get_counters();
        } else {
            f();
        }
    }

    ~BenchmarkPerfCounterRecorder()
    {
        if (_profiler.status() == PerfCounterProfiler::Status::OK) {
            report_counter("perf.cycle", &PerfCounters::cycles);
            report_counter("perf.instr", &PerfCounters::instructions);
            report_counter("perf.branch.total", &PerfCounters::branches);
            report_counter("perf.branch.miss", &PerfCounters::branch_misses);
            report_counter("perf.cache.miss.l1", &PerfCounters::l1_misses);
            report_counter("perf.cache.miss.l2", &PerfCounters::l2_misses);
            report_counter("perf.cache.miss.l3", &PerfCounters::l3_misses);
        }
    }

private:
    inline void report_counter(const char* counter_name, uint64_t PerfCounters::* counter_ptr)
    {
        const auto& value = _perf_counters.*counter_ptr;
        if (value > 0)
            _state.counters[counter_name] = benchmark::Counter(value, benchmark::Counter::kAvgIterations);
    }

private:
    benchmark::State& _state;
    PerfCounterProfiler& _profiler;
    PerfCounters _perf_counters;
};

} // namespace perf_utils
