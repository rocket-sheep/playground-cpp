#pragma once

#include <array>
#include <functional>

namespace playground {

template <typename ForwardIt, typename T, typename BinaryOp>
typename ForwardIt::value_type agg_scalar_naive(
    ForwardIt input_begin,
    ForwardIt input_end,
    T initial_value,
    BinaryOp reduce)
{
    T total = initial_value;

    // WARNING: The compiler is usually smart enough to auto-vectorize this simple loop,
    // so we must instruct it explicitly not to do so
    #pragma clang loop vectorize(disable)
    #pragma clang loop unroll(disable)
    for (auto&& it = input_begin; it != input_end; ++it)
        total = reduce(total, *it);

    return total;
}

template <typename ForwardIt>
typename ForwardIt::value_type sum_scalar_naive(ForwardIt input_begin, ForwardIt input_end)
{
    return agg_scalar_naive(input_begin, input_end, typename ForwardIt::value_type(), std::plus<>());
}

template <size_t lane_count, typename RandomIt, typename T, typename BinaryOp>
typename RandomIt::value_type agg_scalar_unrolled(
    RandomIt input_begin,
    RandomIt input_end,
    T initial_value,
    BinaryOp reduce)
{
    T total = initial_value;

    // Compute partial totals
    std::array<T, lane_count> partial_totals = {};
    const auto input_size = std::distance(input_begin, input_end);
    auto&& vectorized_end = input_end - input_size % lane_count;
    // Make sure we do not auto-vectorize the outer loop
    #pragma clang loop vectorize(disable)
    #pragma clang loop unroll(disable)
    for (auto&& it = input_begin; it < vectorized_end; it += lane_count) {
        // Force unrolling of the inner loop
        #pragma clang loop unroll(enable)
        #pragma clang loop vectorize(disable)
        for (size_t i = 0; i < lane_count; ++i)
            partial_totals[i] = reduce(partial_totals[i], *(it + i));
    }

    // Reduce partial totals
    #pragma clang loop unroll(enable)
    for (auto&& v : partial_totals)
        total = reduce(total, v);

    // Flush remainder if input size is not a multiple of the vector size
    for (auto&& it = vectorized_end; it < input_end; ++it)
        total = reduce(total, *it);

    return total;
}

template <size_t lane_count, typename RandomIt>
typename RandomIt::value_type sum_scalar_unrolled(RandomIt input_begin, RandomIt input_end)
{
    return agg_scalar_unrolled<lane_count>(input_begin, input_end, typename RandomIt::value_type(), std::plus<>());
}

template <typename ForwardIt, typename T, typename BinaryOp>
typename ForwardIt::value_type agg_scalar_auto_vectorized(
    ForwardIt input_begin,
    ForwardIt input_end,
    T initial_value,
    BinaryOp reduce)
{
    T total = initial_value;

    for (auto&& it = input_begin; it != input_end; ++it)
        total = reduce(total, *it);

    return total;
}

template <typename ForwardIt>
typename ForwardIt::value_type sum_scalar_auto_vectorized(ForwardIt input_begin, ForwardIt input_end)
{
    return agg_scalar_auto_vectorized(input_begin, input_end, typename ForwardIt::value_type(), std::plus<>());
}

} // namespace playground
