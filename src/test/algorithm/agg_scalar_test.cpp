#include <gtest/gtest.h>

#include <cstdint>
#include <numeric>
#include <vector>

#include "algorithm/agg_scalar.hpp"

using namespace playground;

static void _test_sum_scalar(uint32_t (*function)(const std::vector<uint32_t>&))
{
    const size_t n = 1023; // forcing a non-power of two
    std::vector<uint32_t> input(n);
    std::iota(input.begin(), input.end(), 1);

    const uint32_t sum = function(input);

    // Sum of N first integers is N * (N + 1) / 2
    EXPECT_EQ(sum, n * (n + 1) / 2);
}

TEST(agg_scalar, sum_scalar_naive)
{
    _test_sum_scalar([](const auto& input) {
        return sum_scalar_naive(input.begin(), input.end());
    });
}

TEST(agg_scalar, sum_scalar_unrolled)
{
    _test_sum_scalar([](const auto& input) {
        return sum_scalar_unrolled<4>(input.begin(), input.end());
    });
}

TEST(agg_scalar, sum_scalar_auto_vectorized)
{
    _test_sum_scalar([](const auto& input) {
        return sum_scalar_auto_vectorized(input.begin(), input.end());
    });
}
