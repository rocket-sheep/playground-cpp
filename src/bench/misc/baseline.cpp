#include <benchmark/benchmark.h>

#include <cstdint>
#include <cstring>
#include <vector>

#include "perf_utils/BenchmarkPerfCounterRecorder.hpp"
#include "random/data_generator.hpp"
#include "util/units.hpp"

using namespace benchmark;
using namespace perf_utils;
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

    state.counters["thru.in.item"] = Counter(input.size(), Counter::kIsIterationInvariantRate);
    state.counters["thru.in.byte"] = Counter(input.size() * sizeof(uint64_t), Counter::kIsIterationInvariantRate);
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
    __attribute__((noinline))
    virtual uint64_t apply(uint64_t input) const override { return input + 1; }
};

/// Operation to decrement a value
struct DecOp : public DummyOp
{
    __attribute__((noinline))
    virtual uint64_t apply(uint64_t input) const override { return input - 1; }
};

static void baseline_static_dispatch(State& state)
{
    static const IncOp op;

    const size_t size = 1_Mi;

    BenchmarkPerfCounterRecorder bpcr(state);
    bpcr.record([&]() {
        for (auto _ : state) {
            uint64_t value = 0;
            for (size_t i = 0; i < size; ++i)
                value = op.apply(value);
            DoNotOptimize(value);
        }
    });

    state.counters["thru.op"] = Counter(size, Counter::kIsIterationInvariantRate);
}

BENCHMARK(baseline_static_dispatch);

static void baseline_dynamic_dispatch_random(State& state)
{
    static const IncOp incOp;
    static const DecOp decOp;

    const size_t size = 1_Mi;
    const double probability = state.range(0) / 100.0;

    // Generate random operations with the specified probability
    DataGenerator data_generator;
    auto& random_engine = data_generator.random_engine();
    std::uniform_real_distribution<> distribution(0.0, 1.0);
    std::vector<const DummyOp*> ops;
    ops.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        ops.push_back(distribution(random_engine) < probability
            ? static_cast<const DummyOp*>(&incOp)
            : static_cast<const DummyOp*>(&decOp));
    }

    BenchmarkPerfCounterRecorder bpcr(state);
    bpcr.record([&]() {
        for (auto _ : state) {
            uint64_t value = 0;
            for (auto&& op : ops)
                value = op->apply(value);
            DoNotOptimize(value);
        }
    });

    state.counters["thru.op"] = Counter(size, Counter::kIsIterationInvariantRate);
}

BENCHMARK(baseline_dynamic_dispatch_random)->DenseRange(0, 100, 10);

static void baseline_dynamic_dispatch_predictable(State& state)
{
    static const IncOp incOp;
    static const DecOp decOp;

    // Generate a small number of operations with a 50%/50% ratio
    const size_t op_count = state.range(0);
    std::vector<const DummyOp*> ops;
    for (size_t i = 0; i < op_count; ++i) {
        ops.push_back((i % 2) == 0
            ? static_cast<const DummyOp*>(&incOp)
            : static_cast<const DummyOp*>(&decOp));
    }
    // Shuffle the operations so that they are in a random order, but stable across iterations
    DataGenerator data_generator;
    std::shuffle(ops.begin(), ops.end(), data_generator.random_engine());

    const size_t iteration_count = 1_Mi / op_count;

    BenchmarkPerfCounterRecorder bpcr(state);
    bpcr.record([&]() {
        for (auto _ : state) {
            uint64_t value = 0;
            for (size_t i = 0; i < iteration_count; ++i) {
                for (auto&& op : ops)
                    value = op->apply(value);
            }
            DoNotOptimize(value);
        }
    });

    const size_t total_op_count = op_count * iteration_count;
    state.counters["thru.op"] = Counter(total_op_count, Counter::kIsIterationInvariantRate);
}

BENCHMARK(baseline_dynamic_dispatch_predictable)->RangeMultiplier(4)->Range(4, 64_Ki);
