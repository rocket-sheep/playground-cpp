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

template <typename T>
std::vector<std::vector<size_t>> join_merge_inner_kway_heap(
    const std::span<const std::span<const T>>& inputs)
{
    const size_t k = inputs.size();
    if (k < 2)
        throw std::logic_error("A join requires at least two inputs");

    std::vector<std::vector<size_t>> selected_indices;
    selected_indices.resize(k);

    // Compute number of non-empty lanes
    size_t kk = k;
    for (size_t j = 0; j < k; ++j) {
        if (inputs[j].empty()) {
            kk -= 1;
        }
    }
    if (kk < k)
        return selected_indices;

    // Initialize heap
    std::vector<T> heap;
    heap.reserve(k);
    for (size_t j = 0; j < k; ++j) {
        heap.push_back(inputs[j][0]);
    }
    std::make_heap(heap.begin(), heap.end(), std::greater<>());

    std::vector<size_t> indices;
    indices.resize(k, 0);
    std::vector<bool> lane_matches;
    lane_matches.resize(k);
    while (true) {
        // Peek minimum value
        auto min_value = heap[0];

        // Determine matching lanes
        bool all_matched = true;
        for (size_t j = 0; j < k; ++j) {
            const bool matches = inputs[j][indices[j]] == min_value;
            lane_matches[j] = matches;
            all_matched &= matches;
        }

        if (all_matched) {
            // Output matching indices
            for (size_t j = 0; j < k; ++j) {
                selected_indices[j].push_back(indices[j]);
            }
        }

        // Increment iterators of lanes matching the minimum.
        // At the same time, pop their value from the heap.
        // (It doesn't matter in what order we pop them, as long as we pop the right count.)
        // Also, add the next value if the lane isn't empty.
        bool any_end_reached = false;
        for (size_t j = 0; j < k; ++j) {
            if (lane_matches[j]) {
                indices[j] += 1;
                std::pop_heap(heap.begin(), heap.end(), std::greater<>());
                heap.pop_back();
                if (indices[j] == inputs[j].size()) {
                    any_end_reached = true;
                } else {
                    heap.push_back(inputs[j][indices[j]]);
                    std::push_heap(heap.begin(), heap.end(), std::greater<>());
                }
            }
        }

        if (any_end_reached)
            break;
    }

    return selected_indices;
}

template <typename T>
std::vector<std::vector<size_t>> join_merge_inner_kway_branchless(
    const std::span<const std::span<const T>>& inputs)
{
    const size_t k = inputs.size();
    if (k < 2)
        throw std::logic_error("A join requires at least two inputs");

    std::vector<std::vector<size_t>> selected_indices;
    selected_indices.resize(k);

    std::vector<size_t> indices;
    indices.resize(k, 0);
    size_t oi = 0;
    const size_t buffer_size = 128;

    // Compute number of non-empty lanes
    size_t kk = k;
    for (size_t j = 0; j < k; ++j) {
        if (inputs[j].empty()) {
            kk -= 1;
        }
    }
    if (kk < k)
        return selected_indices;

    std::vector<bool> lane_matches;
    lane_matches.resize(k);
    while (true) {
        // Resize output vectors so that we can assign values instead of pushing them
        if (oi >= selected_indices[0].size()) {
            for (size_t j = 0; j < k; ++j)
                selected_indices[j].resize(oi + buffer_size);
        }

        // Compute minimum value across all lanes
        auto min_value = inputs[0][indices[0]];
        for (size_t j = 1; j < k; ++j) {
            min_value = std::min(min_value, inputs[j][indices[j]]);
        }

        // Unconditionnally store indices in the output vectors
        for (size_t j = 0; j < k; ++j)
            selected_indices[j][oi] = indices[j];

        // Determine if all lanes matched
        bool all_matched = true;
        for (size_t j = 0; j < k; ++j) {
            const bool matches = inputs[j][indices[j]] == min_value;
            lane_matches[j] = matches;
            all_matched &= matches;
        }

        // Only increment the output index if LHS and RHS matched.
        // That way, if they don't, the stored values will be discarded
        oi += all_matched;

        // Increment iterators of lanes matching the minimum
        for (size_t j = 0; j < k; ++j) {
            indices[j] += lane_matches[j];
        }

        // Determine if any end reached
        bool any_end_reached = false;
        for (size_t j = 0; j < k; ++j) {
            any_end_reached |= indices[j] == inputs[j].size();
        }
        if (any_end_reached)
            break;
    }

    // Resize selected indices to drop the extraneous buffer
    for (size_t j = 0; j < k; ++j)
        selected_indices[j].resize(oi);

    return selected_indices;
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

#define MERGE_JOIN_INNER_KWAY_HEAP(T) \
    template std::vector<std::vector<size_t>> \
    join_merge_inner_kway_heap( \
        const std::span<const std::span<const T>>& inputs)

MERGE_JOIN_INNER_KWAY_HEAP(uint128_t);
MERGE_JOIN_INNER_KWAY_HEAP(uint64_t);
MERGE_JOIN_INNER_KWAY_HEAP(uint32_t);

#define MERGE_JOIN_INNER_KWAY_BRANCHLESS(T) \
    template std::vector<std::vector<size_t>> \
    join_merge_inner_kway_branchless( \
        const std::span<const std::span<const T>>& inputs)

MERGE_JOIN_INNER_KWAY_BRANCHLESS(uint128_t);
MERGE_JOIN_INNER_KWAY_BRANCHLESS(uint64_t);
MERGE_JOIN_INNER_KWAY_BRANCHLESS(uint32_t);

} // namespace playground
