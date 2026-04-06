#include "random/data_generator.hpp"

#include <cassert>

using namespace playground;

namespace {

template <typename T>
std::vector<T> range_internal(
    RandomEngine& random_engine,
    T lower_bound_inclusive,
    T upper_bound_exclusive,
    float density)
{
    assert(upper_bound_exclusive >= lower_bound_inclusive);
    assert(density >= 0 && density <= 1);

    std::vector<T> output;
    std::uniform_real_distribution<float> distribution(0, 1);
    for (T v = lower_bound_inclusive; v < upper_bound_exclusive; ++v) {
        if (distribution(random_engine) < density)
            output.push_back(v);
    }

    return output;
}

template <typename T>
std::vector<T> random_data_internal(
    RandomEngine& random_engine,
    size_t size,
    T lower_bound_inclusive,
    T upper_bound_inclusive)
{
    std::vector<T> output;
    output.reserve(size);

    std::uniform_int_distribution<T> distribution(lower_bound_inclusive, upper_bound_inclusive);
    for (size_t i = 0; i < size; ++i)
        output.push_back(distribution(random_engine));

    return output;
}

} // anonymous namespace

std::vector<uint64_t> DataGenerator::range_ui64(
    uint64_t lower_bound_inclusive,
    uint64_t upper_bound_exclusive,
    float density)
{
    return range_internal(_random_engine, lower_bound_inclusive, upper_bound_exclusive, density);
}

std::vector<uint32_t> DataGenerator::range_ui32(
    uint32_t lower_bound_inclusive,
    uint32_t upper_bound_exclusive,
    float density)
{
    return range_internal(_random_engine, lower_bound_inclusive, upper_bound_exclusive, density);
}

std::vector<uint64_t> DataGenerator::random_data_ui64(
    size_t size,
    uint64_t lower_bound_inclusive,
    uint64_t upper_bound_inclusive)
{
    return random_data_internal(_random_engine, size, lower_bound_inclusive, upper_bound_inclusive);
}

std::vector<uint32_t> DataGenerator::random_data_ui32(
    size_t size,
    uint32_t lower_bound_inclusive,
    uint32_t upper_bound_inclusive)
{
    return random_data_internal(_random_engine, size, lower_bound_inclusive, upper_bound_inclusive);
}

template <typename T>
std::vector<T> DataGenerator::random_data(
    size_t size,
    T lower_bound_inclusive,
    T upper_bound_exclusive)
{
    return random_data_internal(_random_engine, size, lower_bound_inclusive, upper_bound_exclusive);
}

template std::vector<int64_t> DataGenerator::random_data<int64_t>(size_t, int64_t, int64_t);
template std::vector<int32_t> DataGenerator::random_data<int32_t>(size_t, int32_t, int32_t);
template std::vector<int16_t> DataGenerator::random_data<int16_t>(size_t, int16_t, int16_t);
template std::vector<int8_t> DataGenerator::random_data<int8_t>(size_t, int8_t, int8_t);
