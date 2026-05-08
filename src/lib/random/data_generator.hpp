#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

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
    template <typename T>
    std::vector<T> range(
        T lower_bound_inclusive,
        T upper_bound_exclusive,
        float density);

    template <typename T>
    std::vector<T> random_data(
        size_t size,
        T lower_bound_inclusive = 0,
        T upper_bound_inclusive = std::numeric_limits<T>::max());

    std::vector<std::string> random_strings(
        size_t count,
        size_t min_length = 4,
        size_t max_length = 16);

private:
    RandomEngine _random_engine;
};

} // namespace playground
