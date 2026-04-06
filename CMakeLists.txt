cmake_minimum_required(VERSION 3.15)

project(playground)

# ----------------------------------------------------------------------
# Configuration
# ----------------------------------------------------------------------

# Default to Release configuration.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Validate configuration.
if(NOT CMAKE_BUILD_TYPE MATCHES "Debug|Sanitize|Release")
    message(FATAL_ERROR "Unknown configuration ${CMAKE_BUILD_TYPE}")
endif()

message(STATUS "Configuration = ${CMAKE_BUILD_TYPE}")

# ----------------------------------------------------------------------
# Ccache
# ----------------------------------------------------------------------

find_program(CCACHE_PATH ccache)
if(CCACHE_PATH)
    message(STATUS "Using `ccache`")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PATH}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PATH}")
endif(CCACHE_PATH)

# ----------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------

# WARNING: Dependencies have to be included *before* we define our own compilation flags!
add_subdirectory(deps)

# ----------------------------------------------------------------------
# Compiler flags
# ----------------------------------------------------------------------

# Reset flags in all configurations.
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_PROFILE "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_SANITIZE "")

# Set up various flags.
# Each of these modules appends to the `FLAGS` list, potentially based on the active configuration.
include(etc/cmake/compile-options.cmake)
include(etc/cmake/debug-prefix-map.cmake)
include(etc/cmake/dynamic-analysis.cmake)
include(etc/cmake/features.cmake)
include(etc/cmake/warnings.cmake)

# Wrap up.
# NOTE: We set both C and C++ flags at the same time.
list(JOIN FLAGS " " FLAGS)
set(CMAKE_C_FLAGS ${FLAGS})
set(CMAKE_CXX_FLAGS ${FLAGS})

# ----------------------------------------------------------------------
# Output directories
# ----------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ----------------------------------------------------------------------
# Modules
# ----------------------------------------------------------------------

add_subdirectory(src)
