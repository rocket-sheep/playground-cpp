#pragma once

#include <cstdint>

// Binary multipliers

constexpr size_t operator""_Ki(unsigned long long x) {
    return 1024ULL * x;
}

constexpr size_t operator""_Mi(unsigned long long x) {
    return 1024_Ki * x;
}

constexpr size_t operator""_Gi(unsigned long long x) {
    return 1024_Mi * x;
}

constexpr size_t operator""_Ti(unsigned long long x) {
    return 1024_Gi * x;
}

constexpr size_t operator""_Pi(unsigned long long x) {
    return 1024_Ti * x;
}

// Decimal multipliers

// Binary multipliers

constexpr size_t operator""_K(unsigned long long x) {
    return 1000ULL * x;
}

constexpr size_t operator""_M(unsigned long long x) {
    return 1000_Ki * x;
}

constexpr size_t operator""_G(unsigned long long x) {
    return 1000_Mi * x;
}

constexpr size_t operator""_T(unsigned long long x) {
    return 1000_Gi * x;
}

constexpr size_t operator""_P(unsigned long long x) {
    return 1000_Ti * x;
}
