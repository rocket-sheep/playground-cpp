#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

#include "algorithm/scalar_aggregation.hpp"
#include "random/data_generator.hpp"
#include "util/units.hpp"

using namespace benchmark;
using namespace playground;

static void _scalar_sum_stdlib(State& state)
{
    const size_t size = state.range(0);
    DataGenerator data_generator;
    const std::vector<uint32_t> input = data_generator.random_data<uint32_t>(size);

    for (auto _ : state) {
        auto sum = std::reduce(input.begin(), input.end(), 0, std::plus<>());
        DoNotOptimize(sum);
    }

    state.counters["thru.in.items"] = Counter(input.size(), Counter::kIsIterationInvariantRate);
    state.counters["thru.in.bytes"] = Counter(input.size() * sizeof(uint32_t), Counter::kIsIterationInvariantRate);
}

BENCHMARK(_scalar_sum_stdlib)->Arg(16_Mi);

static void _scalar_sum_naive(State& state)
{
    const size_t size = state.range(0);
    DataGenerator data_generator;
    const std::vector<uint32_t> input = data_generator.random_data<uint32_t>(size);

    for (auto _ : state) {
        auto sum = scalar_sum_naive(input.begin(), input.end());
        DoNotOptimize(sum);
    }

    state.counters["thru.in.items"] = Counter(input.size(), Counter::kIsIterationInvariantRate);
    state.counters["thru.in.bytes"] = Counter(input.size() * sizeof(uint32_t), Counter::kIsIterationInvariantRate);
}

BENCHMARK(_scalar_sum_naive)->Arg(16_Mi);

template <size_t vector_size>
static void _scalar_sum_vectorized(State& state)
{
    const size_t size = state.range(0);
    DataGenerator data_generator;
    const std::vector<uint32_t> input = data_generator.random_data<uint32_t>(size);

    for (auto _ : state) {
        auto sum = scalar_sum_vectorized<vector_size>(input.begin(), input.end());
        DoNotOptimize(sum);
    }

    state.counters["thru.in.items"] = Counter(input.size(), Counter::kIsIterationInvariantRate);
    state.counters["thru.in.bytes"] = Counter(input.size() * sizeof(uint32_t), Counter::kIsIterationInvariantRate);
}

BENCHMARK(_scalar_sum_vectorized<2>)->Arg(16_Mi);
BENCHMARK(_scalar_sum_vectorized<4>)->Arg(16_Mi);
BENCHMARK(_scalar_sum_vectorized<8>)->Arg(16_Mi);
BENCHMARK(_scalar_sum_vectorized<16>)->Arg(16_Mi);
BENCHMARK(_scalar_sum_vectorized<32>)->Arg(16_Mi);
