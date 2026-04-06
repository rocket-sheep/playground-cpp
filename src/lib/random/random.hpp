#pragma once

#include <random>

namespace playground {

/// We use a fixed random engine for reproducible behavior
using RandomEngine = std::mt19937_64;

} // namespace playground
