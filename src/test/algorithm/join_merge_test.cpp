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

static void _test_join_merge_inner_kway(
    std::vector<std::vector<size_t>> (*function)(const std::span<const std::span<const uint32_t>>&))
{
    std::vector<std::vector<uint32_t>> inputs = {
        { 0, 1, 2, 3, 4, 5,    7       },
        {    1,    3, 4, 5,    7,    9 },
        {       2, 3,    5, 6, 7, 8    },
        { 0, 1,    3, 4, 5,    7, 8, 9 },
        {    1, 2, 3, 4, 5, 6, 7, 8    },
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 0,    2, 3,    5, 6, 7       },
    };

    const std::vector<std::vector<size_t>> expected_indices = {
        { 3, 5, 6 },
        { 1, 3, 4 },
        { 1, 2, 4 },
        { 2, 4, 5 },
        { 2, 4, 6 },
        { 3, 5, 7 },
        { 2, 3, 5 },
    };

    for (size_t lane_count = 3; lane_count <= inputs.size(); ++lane_count)
    {
        std::vector<std::span<const uint32_t>> input_spans;
        for (size_t i = 0; i < lane_count; ++i)
            input_spans.push_back(inputs[i]);
        
        auto expected = expected_indices;
        expected.resize(lane_count);

        const auto observed = function(input_spans);
        EXPECT_EQ(observed, expected) << "(for " << lane_count << " lanes)" ;
    }
}

TEST(merge, join_merge_inner_kway_heap)
{
    _test_join_merge_inner_kway([](const auto& inputs) {
        return join_merge_inner_kway_heap(inputs);
    });
}

TEST(merge, join_merge_inner_kway_branchless)
{
    _test_join_merge_inner_kway([](const auto& inputs) {
        return join_merge_inner_kway_branchless(inputs);
    });
}
