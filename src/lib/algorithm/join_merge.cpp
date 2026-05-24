#include "algorithm/join_merge.hpp"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <span>
#include <stdexcept>
#include <vector>

#include "base/types.hpp"

namespace playground {

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>> join_merge_inner_2way_naive(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    std::vector<size_t> lhs_selected_indices;
    std::vector<size_t> rhs_selected_indices;

    size_t li = 0;
    size_t ri = 0;
    const size_t ls = lhs.size();
    const size_t rs = rhs.size();

    while (li < ls && ri < rs) {
        // Compare the two heads
        const auto cmp = lhs[li] <=> rhs[ri];

        // LHS is smaller: advance left lane
        if (cmp < 0) {
            ++li;
        }
        // RHS is smaller: advance right lane
        else if (cmp > 0) {
            ++ri;
        }
        // The two sides are equal: output indices, and advance both lanes
        else {
            lhs_selected_indices.push_back(li);
            rhs_selected_indices.push_back(ri);
            ++li;
            ++ri;
        }
    }

    return { lhs_selected_indices, rhs_selected_indices };
}

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>> join_merge_inner_2way_branchless(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    std::vector<size_t> lhs_selected_indices;
    std::vector<size_t> rhs_selected_indices;

    size_t li = 0;
    size_t ri = 0;
    size_t oi = 0;
    const size_t ls = lhs.size();
    const size_t rs = rhs.size();
    const size_t buffer_size = 128;

    while (li < ls && ri < rs) {
        // Resize output vectors so that we can assign values instead of pushing them
        if (oi >= lhs_selected_indices.size()) {
            lhs_selected_indices.resize(oi + buffer_size);
            rhs_selected_indices.resize(oi + buffer_size);
        }

        // Compare the two heads
        const auto cmp = lhs[li] <=> rhs[ri];

        // Unconditionnally store indices in the output vectors
        lhs_selected_indices[oi] = li;
        rhs_selected_indices[oi] = ri;

        // But only increment the output index if LHS and RHS matched.
        // That way, if they don't, the stored values will be discarded
        oi += cmp == 0;

        // Increment the input index of each lane, unless it's ahead of the other
        li += cmp <= 0;
        ri += cmp >= 0;
    }

    // Trim the indices vectors to their actual output size
    lhs_selected_indices.resize(oi);
    rhs_selected_indices.resize(oi);

    return { lhs_selected_indices, rhs_selected_indices };
}

// ----------------------------------------------------------------------
// Explicit template instantiations
// ----------------------------------------------------------------------

#define MERGE_JOIN_INNER_2WAY_NAIVE(T) \
    template std::pair<std::vector<size_t>, std::vector<size_t>> \
    join_merge_inner_2way_naive( \
        const std::span<const T>& lhs, \
        const std::span<const T>& rhs)

MERGE_JOIN_INNER_2WAY_NAIVE(uint128_t);
MERGE_JOIN_INNER_2WAY_NAIVE(uint64_t);
MERGE_JOIN_INNER_2WAY_NAIVE(uint32_t);

#define MERGE_JOIN_INNER_2WAY_BRANCHLESS(T) \
    template std::pair<std::vector<size_t>, std::vector<size_t>> \
    join_merge_inner_2way_branchless( \
        const std::span<const T>& lhs, \
        const std::span<const T>& rhs)

MERGE_JOIN_INNER_2WAY_BRANCHLESS(uint128_t);
MERGE_JOIN_INNER_2WAY_BRANCHLESS(uint64_t);
MERGE_JOIN_INNER_2WAY_BRANCHLESS(uint32_t);

} // namespace playground
