#pragma once

#include <cstddef>
#include <span>
#include <utility>
#include <vector>

namespace playground {

template <typename T>
std::vector<T> merge_2way_naive(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs);

template <typename T>
std::vector<T> merge_2way_branchless(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs);

} // namespace playground
