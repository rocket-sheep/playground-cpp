#pragma once

#include <cstdint>
#include <limits>

#include "random/random.hpp"

namespace playground {

class DataGenerator
{
public:
    DataGenerator(uint32_t seed = 1234567890)
    {
        _random_engine.seed(seed);
    }

    inline RandomEngine& random_engine() { return _random_engine; }

public:
    std::vector<uint64_t> range_ui64(
        uint64_t lower_bound_inclusive,
        uint64_t upper_bound_exclusive,
        float density);

    std::vector<uint32_t> range_ui32(
        uint32_t lower_bound_inclusive,
        uint32_t upper_bound_exclusive,
        float density);

    std::vector<uint64_t> random_data_ui64(
        size_t size,
        uint64_t lower_bound_inclusive = 0,
        uint64_t upper_bound_inclusive = std::numeric_limits<uint64_t>::max());

    std::vector<uint32_t> random_data_ui32(
        size_t size,
        uint32_t lower_bound_inclusive = 0,
        uint32_t upper_bound_inclusive = std::numeric_limits<uint32_t>::max());

    template <typename T>
    std::vector<T> random_data(
        size_t size,
        T lower_bound_inclusive = 0,
        T upper_bound_inclusive = std::numeric_limits<T>::max());

    template <>
    inline std::vector<uint64_t> random_data(
        size_t size,
        uint64_t lower_bound_inclusive,
        uint64_t upper_bound_inclusive)
    {
        return random_data_ui64(size, lower_bound_inclusive, upper_bound_inclusive);
    }

    template <>
    inline std::vector<uint32_t> random_data(
        size_t size,
        uint32_t lower_bound_inclusive,
        uint32_t upper_bound_inclusive)
    {
        return random_data_ui32(size, lower_bound_inclusive, upper_bound_inclusive);
    }

private:
    RandomEngine _random_engine;
};

} // namespace playground
