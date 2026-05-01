#pragma once

#include <cstdint>
#include <ostream>
#include <string_view>
#include <vector>

namespace perf_utils {

/// Available perf counters
enum class PerfCounter : uint32_t {
    CYCLES = 0,
    INSTRUCTIONS,
    BRANCHES,
    BRANCH_MISSES,
    L1_MISSES,
    L2_MISSES,
    L3_MISSES
};

std::vector<PerfCounter> parse_enabled_counters(const std::string_view& enabled_counters_as_text);

struct PerfCounters
{
    uint64_t cycles = 0;
    uint64_t instructions = 0;
    uint64_t branches = 0;
    uint64_t branch_misses = 0;
    uint64_t l1_misses = 0;
    uint64_t l2_misses = 0;
    uint64_t l3_misses = 0;
};

inline PerfCounters operator+(const PerfCounters& lhs, const PerfCounters& rhs)
{
    return {
        .cycles = lhs.cycles + rhs.cycles,
        .instructions = lhs.instructions + rhs.instructions,
        .branches = lhs.branches + rhs.branches,
        .branch_misses = lhs.branch_misses + rhs.branch_misses,
        .l1_misses = lhs.l1_misses + rhs.l1_misses,
        .l2_misses = lhs.l2_misses + rhs.l2_misses,
        .l3_misses = lhs.l3_misses + rhs.l3_misses,
    };
}

inline PerfCounters& operator+=(PerfCounters& lhs, const PerfCounters& rhs)
{
    lhs.cycles += rhs.cycles;
    lhs.instructions += rhs.instructions;
    lhs.branches += rhs.branches;
    lhs.branch_misses += rhs.branch_misses;
    lhs.l1_misses += rhs.l1_misses;
    lhs.l2_misses += rhs.l2_misses;
    lhs.l3_misses += rhs.l3_misses;
    return lhs;
}

inline std::ostream& operator<<(std::ostream& out, const PerfCounters& perf_counters)
{
    return out
        << "{cycles: " << perf_counters.cycles
        << ", instructions: " << perf_counters.instructions
        << ", branches: " << perf_counters.branches
        << ", branch_misses: " << perf_counters.branch_misses
        << ", l1_misses: " << perf_counters.l1_misses
        << ", l2_misses: " << perf_counters.l2_misses
        << ", l3_misses: " << perf_counters.l3_misses
        << "}";
}

} // namespace perf_utils
