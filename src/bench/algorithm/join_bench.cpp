#include <benchmark/benchmark.h>

#include <cmath>
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

template <
    typename T,
    std::vector<std::vector<size_t>> (*function)(const std::span<const std::span<const T>>&),
    std::vector<T> (*build_input)(DataGenerator&, size_t, float)>
static void join_merge_inner_kway(State& state)
{
    const size_t lane_count = state.range(0);

    // Ensure a total input size of approximately 1 Mi items across all lanes
    const size_t range_size = 1_Mi / lane_count;

    DataGenerator data_generator;
    std::vector<std::vector<T>> inputs;
    std::vector<std::span<const T>> input_spans;
    for (size_t k = 0; k < lane_count; ++k) {
        // We want the probability for one item to match to be roughly 1/2,
        // so we generate ranges with a density of (1/2)^(1/K).
        inputs.push_back(build_input(data_generator, range_size, std::pow(0.5f, 1.0f / lane_count)));
        input_spans.push_back(inputs[k]);
    }

    size_t output_size = 0;
    BenchmarkPerfCounterRecorder bpcr(state);
    bpcr.record([&]() {
        for (auto _ : state) {
            const auto outputs = function(input_spans);
            output_size = outputs[0].size();
        }
    });

    const size_t total_input_size = std::accumulate(inputs.begin(), inputs.end(), 0, [](size_t accumulated, const auto& item) { return accumulated + item.size(); });
    state.counters["size.in.item"] = Counter(total_input_size, Counter::kDefaults);
    state.counters["size.out.item"] = Counter(output_size, Counter::kDefaults);
    state.counters["thru.in.item"] = Counter(total_input_size, Counter::kIsIterationInvariantRate);
    state.counters["thru.in.byte"] = Counter(total_input_size * sizeof(T), Counter::kIsIterationInvariantRate);
    state.counters["thru.out.item"] = Counter(output_size, Counter::kIsIterationInvariantRate);
    state.counters["thru.out.byte"] = Counter(output_size * sizeof(T), Counter::kIsIterationInvariantRate);
}

BENCHMARK_TEMPLATE(join_merge_inner_kway, uint32_t, &playground::join_merge_inner_kway_heap, &build_input_int)
    ->Name("join_merge_inner_kway_heap<uint32>")->RangeMultiplier(2)->Range(4, 1024);
BENCHMARK_TEMPLATE(join_merge_inner_kway, uint32_t, &playground::join_merge_inner_kway_branchless, &build_input_int)
    ->Name("join_merge_inner_kway_branchless<uint32>")->RangeMultiplier(2)->Range(4, 1024);

BENCHMARK_TEMPLATE(join_merge_inner_kway, uint64_t, &playground::join_merge_inner_kway_heap, &build_input_int)
    ->Name("join_merge_inner_kway_heap<uint64>")->RangeMultiplier(2)->Range(4, 1024);
BENCHMARK_TEMPLATE(join_merge_inner_kway, uint64_t, &playground::join_merge_inner_kway_branchless, &build_input_int)
    ->Name("join_merge_inner_kway_branchless<uint64>")->RangeMultiplier(2)->Range(4, 1024);

BENCHMARK_TEMPLATE(join_merge_inner_kway, uint128_t, &playground::join_merge_inner_kway_heap, &build_input_int)
    ->Name("join_merge_inner_kway_heap<uint128>")->RangeMultiplier(2)->Range(4, 1024);
BENCHMARK_TEMPLATE(join_merge_inner_kway, uint128_t, &playground::join_merge_inner_kway_branchless, &build_input_int)
    ->Name("join_merge_inner_kway_branchless<uint128>")->RangeMultiplier(2)->Range(4, 1024);
