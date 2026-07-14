#pragma once

#include <cstddef>
#include <span>
#include <utility>
#include <vector>

namespace playground {

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>>
join_merge_inner_2way_naive(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs);

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>>
join_merge_inner_2way_branchless(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs);

template <typename T>
std::vector<std::vector<size_t>>
join_merge_inner_kway_heap(
    const std::span<const std::span<const T>>& inputs);

template <typename T>
std::vector<std::vector<size_t>>
join_merge_inner_kway_branchless(
    const std::span<const std::span<const T>>& inputs);

} // namespace playground
