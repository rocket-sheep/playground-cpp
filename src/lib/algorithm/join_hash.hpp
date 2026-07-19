#pragma once

#include <cstddef>
#include <span>
#include <utility>
#include <vector>

namespace playground {

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>>
join_hash_inner_2way_std(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs);

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>>
join_hash_inner_2way_absl(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs);

} // namespace playground
