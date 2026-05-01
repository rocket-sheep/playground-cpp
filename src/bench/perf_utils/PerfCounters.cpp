#include "perf_utils/PerfCounters.hpp"

#include <iostream>
#include <ranges>

namespace perf_utils {

std::vector<PerfCounter> parse_enabled_counters(const std::string_view& enabled_counters_as_text)
{
    std::vector<PerfCounter> enabled_counters;

    // TODO: There should be an easier way with C++20 ranges and views
    size_t pos = 0;
    std::vector<std::string_view> counter_names;
    std::string_view s = enabled_counters_as_text;
    while ((pos = s.find(',')) != std::string::npos) {
        auto counter_name = s.substr(0, pos);
        counter_names.push_back(counter_name);
        s = s.substr(pos + 1);
    }
    counter_names.push_back(s);

    for (auto&& counter_name : counter_names) {
        if (counter_name == "cycles") {
            enabled_counters.push_back(PerfCounter::CYCLES);
        } else if (counter_name == "instructions") {
            enabled_counters.push_back(PerfCounter::INSTRUCTIONS);
        } else if (counter_name == "branches") {
            enabled_counters.push_back(PerfCounter::BRANCHES);
        } else if (counter_name == "branch_misses") {
            enabled_counters.push_back(PerfCounter::BRANCH_MISSES);
        } else if (counter_name == "l1_misses") {
            enabled_counters.push_back(PerfCounter::L1_MISSES);
        } else if (counter_name == "l2_misses") {
            enabled_counters.push_back(PerfCounter::L2_MISSES);
        } else if (counter_name == "l3_misses") {
            enabled_counters.push_back(PerfCounter::L3_MISSES);
        } else {
            // Ignore unknown counters; just warn
            std::cerr << "Unknown counter '" << counter_name << "'" << std::endl;
        }
    }

    return enabled_counters;
}

} // namespace perf_utils
