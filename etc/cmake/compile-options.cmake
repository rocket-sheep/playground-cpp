# Language version
# ----------------

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

# Symbols
# -------

# Always include symbols.
# They are useful even in `Release` configuration, e.g. when profiling.
list(APPEND FLAGS "-g")

# Optimizations
# -------------

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    list(APPEND FLAGS "-O0")
else()
    list(APPEND FLAGS "-O3")
endif()

# Architecture
# ------------

# Enable AVX & AVX2
# list(APPEND FLAGS "-mavx" "-mavx2")
