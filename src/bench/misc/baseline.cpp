#include <benchmark/benchmark.h>

#include <cstdint>
#include <cstring>
#include <vector>

#include "random/data_generator.hpp"
#include "util/units.hpp"

using namespace benchmark;
using namespace playground;

static void baseline_memory_copy(State& state)
{
    const size_t size = state.range(0);
    DataGenerator data_generator;
    const std::vector<uint64_t> input = data_generator.random_data<uint64_t>(size);
    std::vector<uint64_t> output;
    output.resize(size);

    for (auto _ : state) {
        DoNotOptimize(std::memcpy(&output[0], input.data(), size * sizeof(uint64_t)));
    }

    state.counters["thru.items"] = Counter(input.size(), Counter::kIsIterationInvariantRate);
    state.counters["thru.bytes"] = Counter(input.size() * sizeof(uint64_t), Counter::kIsIterationInvariantRate);
}

BENCHMARK(baseline_memory_copy)->Arg(1_Mi);

/// Abstract virtual operation
struct DummyOp
{
    virtual ~DummyOp() = default;
    virtual uint64_t apply(uint64_t input) const = 0;
};

/// Operation to increment a value
struct IncOp : public DummyOp
{
    virtual uint64_t apply(uint64_t input) const override { return input + 1; }
};

/// Operation to decrement a value
struct DecOp : public DummyOp
{
    virtual uint64_t apply(uint64_t input) const override { return input - 1; }
};

static void baseline_static_dispatch(State& state)
{
    const size_t size = state.range(0);

    const IncOp incOp;
    const IncOp* op = &incOp;

    for (auto _ : state) {
        uint64_t value = 0;
        for (size_t i = 0; i < size; ++i)
            value = op->apply(value);
        DoNotOptimize(value);
    }

    state.counters["thru.ops"] = Counter(size, Counter::kIsIterationInvariantRate);
}

BENCHMARK(baseline_static_dispatch)->Arg(1_Mi);

static void baseline_dynamic_dispatch(State& state)
{
    const size_t size = state.range(0);
    const size_t probability = state.range(1);
    const uint64_t cutoff = std::numeric_limits<uint64_t>::max() * probability;

    DataGenerator data_generator;
    const auto input = data_generator.random_data<uint64_t>(size);

    const IncOp incOp;
    const DecOp decOp;

    std::vector<const DummyOp*> ops;
    ops.reserve(size);
    for (const auto v : input) {
        ops.push_back(static_cast<uint64_t>(v) < cutoff
            ? static_cast<const DummyOp*>(&incOp)
            : static_cast<const DummyOp*>(&decOp));
    }

    for (auto _ : state) {
        uint64_t value = 0;
        for (auto&& op : ops)
            value = op->apply(value);
        DoNotOptimize(value);
    }

    state.counters["thru.ops"] = Counter(size, Counter::kIsIterationInvariantRate);
}

BENCHMARK(baseline_dynamic_dispatch)->Args({ 1_Mi, 0 });
BENCHMARK(baseline_dynamic_dispatch)->Args({ 1_Mi, 50 });
BENCHMARK(baseline_dynamic_dispatch)->Args({ 1_Mi, 100 });
