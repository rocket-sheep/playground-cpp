#include <gtest/gtest.h>

#include <cstdint>
#include <numeric>
#include <vector>

#include "algorithm/agg_scalar.hpp"

using namespace playground;

TEST(agg_scalar, sum_scalar_naive)
{
    const size_t n = 1023; // forcing a non-power of two
    std::vector<uint64_t> input(n);
    std::iota(input.begin(), input.end(), 1);

    const uint64_t sum = sum_scalar_naive(input.begin(), input.end());

    // Sum of N first integers is N * (N + 1) / 2
    EXPECT_EQ(sum, n * (n + 1) / 2);
}

TEST(agg_scalar, sum_scalar_unrolled)
{
    const size_t n = 1023; // forcing a non-power of two
    std::vector<uint64_t> input(n);
    std::iota(input.begin(), input.end(), 1);

    const uint64_t sum = sum_scalar_unrolled<4>(input.begin(), input.end());

    // Sum of N first integers is N * (N + 1) / 2
    EXPECT_EQ(sum, n * (n + 1) / 2);
}

TEST(agg_scalar, sum_scalar_auto_vectorized)
{
    const size_t n = 1023; // forcing a non-power of two
    std::vector<uint64_t> input(n);
    std::iota(input.begin(), input.end(), 1);

    const uint64_t sum = sum_scalar_auto_vectorized(input.begin(), input.end());

    // Sum of N first integers is N * (N + 1) / 2
    EXPECT_EQ(sum, n * (n + 1) / 2);
}
