#include "algorithm/join_hash.hpp"

#include <unordered_map>

#include <absl/container/flat_hash_map.h>

#include "base/types.hpp"

namespace {

template <typename T, template<typename K, typename V> typename Map>
std::pair<std::vector<size_t>, std::vector<size_t>>
join_hash_inner_2way(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    // Build an index of the right-hand side.
    // NOTE: We assume RHS is always the build side.
    // In a real-world implementation, we would choose dynamically based on the respective cardinalities.
    // NOTE: We don't bother checking for duplicates. We just assume keys are unique
    Map<T, size_t> rhs_index;
    rhs_index.reserve(rhs.size());
    for (size_t ri = 0; ri < rhs.size(); ++ri) {
        rhs_index[rhs[ri]] = ri;
    }

    // Lookup every row of LHS in RHS.
    std::vector<size_t> lhs_indices;
    std::vector<size_t> rhs_indices;
    const auto not_found = rhs_index.end();
    for (size_t li = 0; li < lhs.size(); ++li) {
        const auto it = rhs_index.find(lhs[li]);
        if (it != not_found) {
            lhs_indices.push_back(li);
            rhs_indices.push_back(it->second);
        }
    }

    return { lhs_indices, rhs_indices };
}

} // anonymous namespace

namespace playground {

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>>
join_hash_inner_2way_std(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    return join_hash_inner_2way<T, std::unordered_map>(lhs, rhs);
}

#define HASH_JOIN_INNER_2WAY_STD(T) template std::pair<std::vector<size_t>, std::vector<size_t>> \
    join_hash_inner_2way_std( \
        const std::span<const T>&, \
        const std::span<const T>&)

HASH_JOIN_INNER_2WAY_STD(uint128_t);
HASH_JOIN_INNER_2WAY_STD(uint64_t);
HASH_JOIN_INNER_2WAY_STD(uint32_t);
HASH_JOIN_INNER_2WAY_STD(uint16_t);
HASH_JOIN_INNER_2WAY_STD(uint8_t);

template <typename T>
std::pair<std::vector<size_t>, std::vector<size_t>>
join_hash_inner_2way_absl(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    return join_hash_inner_2way<T, absl::flat_hash_map>(lhs, rhs);
}

#define HASH_JOIN_INNER_2WAY_ABSL(T) template std::pair<std::vector<size_t>, std::vector<size_t>> \
    join_hash_inner_2way_absl( \
        const std::span<const T>&, \
        const std::span<const T>&)

HASH_JOIN_INNER_2WAY_ABSL(uint128_t);
HASH_JOIN_INNER_2WAY_ABSL(uint64_t);
HASH_JOIN_INNER_2WAY_ABSL(uint32_t);
HASH_JOIN_INNER_2WAY_ABSL(uint16_t);
HASH_JOIN_INNER_2WAY_ABSL(uint8_t);

} // namespace playground
