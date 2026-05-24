#include "algorithm/merge.hpp"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <span>
#include <stdexcept>
#include <vector>

namespace playground {

template <typename T>
void merge_2way_naive_internal(
    std::vector<T>& output,
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    output.reserve(output.size() + lhs.size() + rhs.size());

    auto l = lhs.begin();
    auto r = rhs.begin();
    const auto le = lhs.end();
    const auto re = rhs.end();

    while (l < le && r < re) {
        const auto lv = *l;
        const auto rv = *r;
        if (rv < lv) {
            output.push_back(rv);
            ++r;
        } else {
            output.push_back(lv);
            ++l;
        }
    }
    while (l < le) {
        output.push_back(*l++);
    }
    while (r < re) {
        output.push_back(*r++);
    }
}

template <typename T>
std::vector<T> merge_2way_naive(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    std::vector<T> output;
    merge_2way_naive_internal(output, lhs, rhs);
    return output;
}

template <typename T>
void merge_2way_branchless_internal(
    std::vector<T>& output,
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    output.reserve(output.size() + lhs.size() + rhs.size());

    auto l = lhs.begin();
    auto r = rhs.begin();
    const auto le = lhs.end();
    const auto re = rhs.end();

    while (l < le && r < re) {
        const auto lv = *l;
        const auto rv = *r;
        const bool rhs_is_less = rv < lv;
        l += !rhs_is_less;
        r += rhs_is_less;
        const auto v = rhs_is_less ? rv : lv;
        output.push_back(v);
    }
    while (l < le) {
        output.push_back(*l++);
    }
    while (r < re) {
        output.push_back(*r++);
    }
}

template <typename T>
std::vector<T> merge_2way_branchless(
    const std::span<const T>& lhs,
    const std::span<const T>& rhs)
{
    std::vector<T> output;
    merge_2way_branchless_internal(output, lhs, rhs);
    return output;
}

// ----------------------------------------------------------------------
// Explicit template instantiations
// ----------------------------------------------------------------------

#define MERGE_2WAY_NAIVE(T) \
    template std::vector<T> merge_2way_naive( \
        const std::span<const T>& lhs, \
        const std::span<const T>& rhs)

MERGE_2WAY_NAIVE(uint64_t);
MERGE_2WAY_NAIVE(uint32_t);
MERGE_2WAY_NAIVE(std::string);

#define MERGE_2WAY_BRANCHLESS(T) \
    template std::vector<T> merge_2way_branchless( \
        const std::span<const T>& lhs, \
        const std::span<const T>& rhs)

MERGE_2WAY_BRANCHLESS(uint64_t);
MERGE_2WAY_BRANCHLESS(uint32_t);
MERGE_2WAY_BRANCHLESS(std::string);

} // namespace playground
