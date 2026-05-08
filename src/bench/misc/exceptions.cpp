#include <benchmark/benchmark.h>

#include <cstdint>
#include <cstring>
#include <span>
#include <vector>

#include "random/data_generator.hpp"
#include "util/units.hpp"

using namespace benchmark;
using namespace playground;

static size_t _return_code(
    const std::span<const uint64_t>& data,
    uint64_t log2p)
{
    auto f = [](const std::span<const uint64_t>& data, uint64_t log2p)
    {
        const uint64_t mask = (1 << log2p) - 1;
        for (auto&& value : data) {
            if ((value & mask) == 0)
                return true;
        }
        return false;
    };

    size_t error_count = 0;
    constexpr size_t batch_size = 1_Ki;
    for (size_t start_index = 0; start_index < data.size(); start_index += batch_size) {
        auto s = data.subspan(start_index, std::min(batch_size, data.size() - start_index));
        if (f(s, log2p))
            error_count += 1;
    }
    return error_count;
}

static size_t _exceptions(
    const std::span<const uint64_t>& data,
    uint64_t log2p)
{
    auto f = [](const std::span<const uint64_t>& data, uint64_t log2p)
    {
        const uint64_t mask = (1 << log2p) - 1;
        for (auto&& value : data) {
            if ((value & mask) == 0)
                throw std::exception();
        }
    };

    size_t error_count = 0;
    constexpr size_t batch_size = 1_Ki;
    for (size_t start_index = 0; start_index < data.size(); start_index += batch_size) {
        try {
            auto s = data.subspan(start_index, std::min(batch_size, data.size() - start_index));
            f(s, log2p);
        }
        catch (const std::exception& e) {
            error_count += 1;
        }
    }
    return error_count;
}

template <size_t f(const std::span<const uint64_t>&, uint64_t)>
static void control_flow(State& state)
{
    const uint64_t log2p = state.range(0);
    const size_t size = 1_Mi;
    DataGenerator data_generator;
    const std::vector<uint64_t> data = data_generator.random_data<uint64_t>(size);

    for (auto _ : state) {
        size_t error_count = f(std::span(data), log2p);
        DoNotOptimize(error_count);
        state.counters["error.thru"] = Counter(error_count, Counter::kIsIterationInvariantRate);
        state.counters["error.count"] = Counter(error_count, Counter::kDefaults);
    }
}

BENCHMARK(control_flow<_return_code>)->Arg(8)->Arg(16)->Arg(24);
BENCHMARK(control_flow<_exceptions>)->Arg(8)->Arg(16)->Arg(24);
