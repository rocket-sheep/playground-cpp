#include <benchmark/benchmark.h>

#include <cstdint>
#include <functional>
#include <numeric>
#include <span>
#include <string>
#include <vector>

#include "algorithm/merge.hpp"
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

static std::vector<std::string> build_input_str(
    DataGenerator& data_generator,
    size_t range_size,
    float _)
{
    auto result = data_generator.random_strings(range_size);
    std::sort(result.begin(), result.end());
    return result;
}

template <
    typename T,
    std::vector<T> (*function)(const std::span<const T>&, const std::span<const T>&),
    std::vector<T> (*build_input)(DataGenerator&, size_t, float)>
static void merge_2way(State& state)
{
    const size_t range_size = 1_Mi;
    DataGenerator data_generator;
    const std::vector<T> lhs = build_input(data_generator, range_size, 0.5f);
    const std::vector<T> rhs = build_input(data_generator, range_size, 0.5f);

    size_t output_size = 0;
    BenchmarkPerfCounterRecorder bpcr(state);
    bpcr.record([&]() {
        for (auto _ : state) {
            const auto output = function(lhs, rhs);
            output_size = output.size();
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

BENCHMARK_TEMPLATE(merge_2way, uint32_t, &playground::merge_2way_naive, &build_input_int)->Name("merge_2way_naive<uint32>");
BENCHMARK_TEMPLATE(merge_2way, uint32_t, &playground::merge_2way_branchless, &build_input_int)->Name("merge_2way_branchless<uint32>");
BENCHMARK_TEMPLATE(merge_2way, uint64_t, &playground::merge_2way_naive, &build_input_int)->Name("merge_2way_naive<uint64>");
BENCHMARK_TEMPLATE(merge_2way, uint64_t, &playground::merge_2way_branchless, &build_input_int)->Name("merge_2way_branchless<uint64>");
BENCHMARK_TEMPLATE(merge_2way, std::string, &playground::merge_2way_naive, &build_input_str)->Name("merge_2way_naive<string>");
BENCHMARK_TEMPLATE(merge_2way, std::string, &playground::merge_2way_branchless, &build_input_str)->Name("merge_2way_branchless<string>");
