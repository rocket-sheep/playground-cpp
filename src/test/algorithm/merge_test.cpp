#include <gtest/gtest.h>

#include "algorithm/merge.hpp"

using namespace playground;

static void _test_merge_2way(
    std::vector<uint64_t> (*function)(const std::span<const uint64_t>&, const std::span<const uint64_t>&))
{
    std::vector<uint64_t> lhs { 0, 1, 4, 5, 7 };
    std::vector<uint64_t> rhs { 0, 2, 3, 5, 6, 8 };
    const std::vector<uint64_t> expected { 0, 0, 1, 2, 3, 4, 5, 5, 6, 7, 8 };

    const auto observed = function(lhs, rhs);
    EXPECT_EQ(observed, expected);

    const auto observed_swapped = function(rhs, lhs);
    EXPECT_EQ(observed_swapped, expected);
}

TEST(merge, merge_2way_naive)
{
    _test_merge_2way([](const auto& lhs, const auto& rhs) {
        return merge_2way_naive(lhs, rhs);
    });
}


TEST(merge, merge_2way_branchless)
{
    _test_merge_2way([](const auto& lhs, const auto& rhs) {
        return merge_2way_branchless(lhs, rhs);
    });
}
