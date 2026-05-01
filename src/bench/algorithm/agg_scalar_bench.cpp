#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <span>
#include <vector>

#include "algorithm/agg_scalar.hpp"
#include "perf_utils/BenchmarkPerfCounterRecorder.hpp"
#include "random/data_generator.hpp"
#include "util/units.hpp"

using namespace benchmark;
using namespace perf_utils;
using namespace playground;

template <uint32_t (*function)(const std::span<const uint32_t>&)>
static void bench_agg_scalar(State& state)
{
    const size_t size = state.range(0);
    DataGenerator data_generator;
    const std::vector<uint32_t> input = data_generator.random_data<uint32_t>(size);

    BenchmarkPerfCounterRecorder bpcr(state);
    bpcr.record([&]() {
        for (auto _ : state) {
            auto result = function(input);
            DoNotOptimize(result);
        }
    });

    state.counters["thru.in.item"] = Counter(input.size(), Counter::kIsIterationInvariantRate);
    state.counters["thru.in.byte"] = Counter(input.size() * sizeof(uint32_t), Counter::kIsIterationInvariantRate);
}

static uint32_t sum_naive(const std::span<const uint32_t>& input)
{
    return sum_scalar_naive(input.begin(), input.end());
}

BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_naive)->Arg(16_Mi);

template <size_t vector_size>
static uint32_t sum_unrolled(const std::span<const uint32_t>& input)
{
    return sum_scalar_unrolled<vector_size>(input.begin(), input.end());
}

BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_unrolled<2>)->Arg(16_Mi);
BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_unrolled<4>)->Arg(16_Mi);
BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_unrolled<8>)->Arg(16_Mi);
BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_unrolled<16>)->Arg(16_Mi);
BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_unrolled<32>)->Arg(16_Mi);

static uint32_t sum_auto_vectorized(const std::span<const uint32_t>& input)
{
    return sum_scalar_auto_vectorized(input.begin(), input.end());
}

BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_auto_vectorized)->Arg(16_Mi);

static uint32_t sum_stdlib(const std::span<const uint32_t>& input)
{
    return std::reduce(input.begin(), input.end(), 0, std::plus<>());
}

BENCHMARK_TEMPLATE(bench_agg_scalar, &sum_stdlib)->Arg(16_Mi);
