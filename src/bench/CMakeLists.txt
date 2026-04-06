file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(playground-bench ${SOURCES})

target_include_directories(playground-bench
    PRIVATE .)

target_link_libraries(playground-bench
    playground-lib
    benchmark)
