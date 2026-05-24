#include <benchmark/benchmark.h>

#include <cstdint>
#include <functional>
#include <numeric>
#include <span>
#include <string>
#include <vector>

#include "algorithm/join_merge.hpp"
#include "base/types.hpp"
#include "perf_utils/BenchmarkPerfCounterRecorder.hpp"
#include "random/data_generator.hpp"
#include "util/units.hpp"

using namespace benchmark;
using namespace perf_utils;
using namespace playground;

template <typename T>
static std::vector<T> build_input_int(
    DataGenerator& data_generator,
    size_t range_size,
    float density)
{
    return data_generator.range<T>(0, range_size, density);
}

template <
    typename T,
    std::pair<std::vector<size_t>, std::vector<size_t>> (*function)(const std::span<const T>&, const std::span<const T>&),
    std::vector<T> (*build_input)(DataGenerator&, size_t, float)>
static void _bench_join_inner_2way(State& state)
{
    const size_t range_size = 1_Mi;
    const float rhs_density = 0.5f / state.range(0);
    DataGenerator data_generator;
    const std::vector<T> lhs = build_input(data_generator, range_size, 0.5f);
    const std::vector<T> rhs = build_input(data_generator, range_size, rhs_density);

    size_t output_size = 0;
    BenchmarkPerfCounterRecorder bpcr(state);
    bpcr.record([&]() {
        for (auto _ : state) {
            const auto [lhs_output, rhs_output] = function(lhs, rhs);
            output_size = lhs_output.size();
        }
    });

    const size_t total_input_size = lhs.size() + rhs.size();
    state.counters["size.in.item"] = Counter(total_input_size, Counter::kDefaults);
    state.counters["size.out.item"] = Counter(output_size, Counter::kDefaults);
    state.counters["thru.in.item"] = Counter(total_input_size, Counter::kIsIterationInvariantRate);
    state.counters["thru.in.byte"] = Counter(total_input_size * sizeof(T), Counter::kIsIterationInvariantRate);
    state.counters["thru.out.item"] = Counter(output_size, Counter::kIsIterationInvariantRate);
    state.counters["thru.out.byte"] = Counter(output_size * sizeof(T), Counter::kIsIterationInvariantRate);
}

BENCHMARK_TEMPLATE(_bench_join_inner_2way, uint32_t, &playground::join_merge_inner_2way_naive, &build_input_int)
    ->RangeMultiplier(2)->Range(1, 16)->Name("join_merge_inner_2way_naive<uint32>");
BENCHMARK_TEMPLATE(_bench_join_inner_2way, uint32_t, &playground::join_merge_inner_2way_branchless, &build_input_int)
    ->RangeMultiplier(2)->Range(1, 16)->Name("join_merge_inner_2way_branchless<uint32>");

BENCHMARK_TEMPLATE(_bench_join_inner_2way, uint64_t, &playground::join_merge_inner_2way_naive, &build_input_int)
    ->RangeMultiplier(2)->Range(1, 16)->Name("join_merge_inner_2way_naive<uint64>");
BENCHMARK_TEMPLATE(_bench_join_inner_2way, uint64_t, &playground::join_merge_inner_2way_branchless, &build_input_int)
    ->RangeMultiplier(2)->Range(1, 16)->Name("join_merge_inner_2way_branchless<uint64>");

BENCHMARK_TEMPLATE(_bench_join_inner_2way, uint128_t, &playground::join_merge_inner_2way_naive, &build_input_int)
    ->RangeMultiplier(2)->Range(1, 16)->Name("join_merge_inner_2way_naive<uint128>");
BENCHMARK_TEMPLATE(_bench_join_inner_2way, uint128_t, &playground::join_merge_inner_2way_branchless, &build_input_int)
    ->RangeMultiplier(2)->Range(1, 16)->Name("join_merge_inner_2way_branchless<uint128>");
