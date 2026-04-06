# ----------------------------------------------------------------------
# Google Benchmark
# ----------------------------------------------------------------------

set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Cf. Google Benchmark")
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Cf. Google Benchmark")
add_subdirectory(benchmark EXCLUDE_FROM_ALL)

# ----------------------------------------------------------------------
# Google Test
# ----------------------------------------------------------------------

set(INSTALL_GTEST OFF CACHE BOOL "Cf. Google Test")
add_subdirectory(googletest EXCLUDE_FROM_ALL)
