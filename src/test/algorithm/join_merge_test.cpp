#include <gtest/gtest.h>

#include "algorithm/join_merge.hpp"

using namespace playground;

static void _test_join_merge_inner_2way(
    std::pair<std::vector<size_t>, std::vector<size_t>> (*function)(const std::span<const uint32_t>&, const std::span<const uint32_t>&))
{
    std::vector<uint32_t> lhs { 0, 1, 2, 3, 4, 5, 7, 10 };
    std::vector<uint32_t> rhs { 1, 3, 5, 6, 7, 8, 9, 11 };
    const std::vector<size_t> expected_lhs { 1, 3, 5, 6 };
    const std::vector<size_t> expected_rhs { 0, 1, 2, 4 };

    const auto [observed_lhs, observed_rhs] = function(lhs, rhs);
    EXPECT_EQ(observed_lhs, expected_lhs);
    EXPECT_EQ(observed_rhs, expected_rhs);
}

TEST(merge, join_merge_inner_2way_naive)
{
    _test_join_merge_inner_2way([](const auto& lhs, const auto& rhs) {
        return join_merge_inner_2way_naive(lhs, rhs);
    });
}

TEST(merge, join_merge_inner_2way_branchless)
{
    _test_join_merge_inner_2way([](const auto& lhs, const auto& rhs) {
        return join_merge_inner_2way_branchless(lhs, rhs);
    });
}
