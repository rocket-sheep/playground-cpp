#pragma once

namespace playground {

template <typename T>
inline constexpr size_t bitsizeof()
{
    return sizeof(T) * 8;
}

inline constexpr bool is_power_of_two(size_t n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

} // namespace playground
