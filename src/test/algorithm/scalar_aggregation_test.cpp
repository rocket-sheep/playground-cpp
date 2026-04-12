#include <gtest/gtest.h>

#include <cstdint>
#include <numeric>
#include <vector>

#include "algorithm/scalar_aggregation.hpp"

using namespace playground;

TEST(scalar_aggregation, scalar_sum_naive)
{
    const size_t n = 1023; // forcing a non-power of two
    std::vector<uint64_t> input(n);
    std::iota(input.begin(), input.end(), 1);

    const uint64_t sum = scalar_sum_naive(input.begin(), input.end());

    // Sum of N first integers is N * (N + 1) / 2
    EXPECT_EQ(sum, n * (n + 1) / 2);
}

TEST(scalar_aggregation, scalar_sum_vectorized)
{
    const size_t n = 1023; // forcing a non-power of two
    std::vector<uint64_t> input(n);
    std::iota(input.begin(), input.end(), 1);

    const uint64_t sum = scalar_sum_vectorized<4>(input.begin(), input.end());

    // Sum of N first integers is N * (N + 1) / 2
    EXPECT_EQ(sum, n * (n + 1) / 2);
}

TEST(scalar_aggregation, scalar_sum_auto_vectorized)
{
    const size_t n = 1023; // forcing a non-power of two
    std::vector<uint64_t> input(n);
    std::iota(input.begin(), input.end(), 1);

    const uint64_t sum = scalar_sum_auto_vectorized(input.begin(), input.end());

    // Sum of N first integers is N * (N + 1) / 2
    EXPECT_EQ(sum, n * (n + 1) / 2);
}
