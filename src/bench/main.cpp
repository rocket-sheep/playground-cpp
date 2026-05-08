#include <cstdlib>

#include <benchmark/benchmark.h>

#include "perf_utils/PerfCounterProfiler.hpp"
#include "perf_utils/PerfCounters.hpp"

using namespace benchmark;
using namespace perf_utils;

int main(int argc, const char* argv[])
{
    const char* perf_counter_names = std::getenv("PERF_COUNTERS");
    if (perf_counter_names != nullptr) {
        const auto enabled_counters = parse_enabled_counters(perf_counter_names);
        PerfCounterProfiler::enable_counters(enabled_counters);
    }

    char** const argv_nonconst = const_cast<char**>(argv);
    Initialize(&argc, argv_nonconst);
    if (ReportUnrecognizedArguments(argc, argv_nonconst))
        return 1;
    RunSpecifiedBenchmarks();

    return 0;
}
