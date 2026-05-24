#include "random/data_generator.hpp"

#include <cassert>

#include "base/types.hpp"

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

template <typename T>
std::vector<T> DataGenerator::range(
    T lower_bound_inclusive,
    T upper_bound_exclusive,
    float density)
{
    return range_internal(_random_engine, lower_bound_inclusive, upper_bound_exclusive, density);
}

template std::vector<uint128_t> DataGenerator::range(uint128_t, uint128_t, float);
template std::vector<uint64_t> DataGenerator::range(uint64_t, uint64_t, float);
template std::vector<uint32_t> DataGenerator::range(uint32_t, uint32_t, float);
template std::vector<uint16_t> DataGenerator::range(uint16_t, uint16_t, float);
template std::vector<uint8_t> DataGenerator::range(uint8_t, uint8_t, float);
template std::vector<int128_t> DataGenerator::range(int128_t, int128_t, float);
template std::vector<int64_t> DataGenerator::range(int64_t, int64_t, float);
template std::vector<int32_t> DataGenerator::range(int32_t, int32_t, float);
template std::vector<int16_t> DataGenerator::range(int16_t, int16_t, float);
template std::vector<int8_t> DataGenerator::range(int8_t, int8_t, float);

template <typename T>
std::vector<T> DataGenerator::random_data(
    size_t size,
    T lower_bound_inclusive,
    T upper_bound_exclusive)
{
    return random_data_internal(_random_engine, size, lower_bound_inclusive, upper_bound_exclusive);
}

template std::vector<uint128_t> DataGenerator::random_data<uint128_t>(size_t, uint128_t, uint128_t);
template std::vector<uint64_t> DataGenerator::random_data<uint64_t>(size_t, uint64_t, uint64_t);
template std::vector<uint32_t> DataGenerator::random_data<uint32_t>(size_t, uint32_t, uint32_t);
template std::vector<uint16_t> DataGenerator::random_data<uint16_t>(size_t, uint16_t, uint16_t);
template std::vector<uint8_t> DataGenerator::random_data<uint8_t>(size_t, uint8_t, uint8_t);
template std::vector<int128_t> DataGenerator::random_data<int128_t>(size_t, int128_t, int128_t);
template std::vector<int64_t> DataGenerator::random_data<int64_t>(size_t, int64_t, int64_t);
template std::vector<int32_t> DataGenerator::random_data<int32_t>(size_t, int32_t, int32_t);
template std::vector<int16_t> DataGenerator::random_data<int16_t>(size_t, int16_t, int16_t);
template std::vector<int8_t> DataGenerator::random_data<int8_t>(size_t, int8_t, int8_t);

std::vector<std::string> DataGenerator::random_strings(
    size_t count,
    size_t min_length,
    size_t max_length)
{
    assert(max_length <= 16);

    std::vector<std::string> result;
    result.reserve(count);

    std::uniform_int_distribution<size_t> length_distribution(min_length, max_length);
    char tmp[20];
    for (size_t i = 0; i < count; ++i) {
        const size_t length = length_distribution(_random_engine);
        const uint64_t value = _random_engine();

        // TODO: Use Base64 rather than hexadecimal for more varied output
        snprintf(tmp, sizeof(tmp), "%016llx", value);
        tmp[length] = '\0';
        result.push_back(tmp);
    }

    return result;
}
