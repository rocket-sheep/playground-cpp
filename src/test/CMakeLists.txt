file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(playground-test ${SOURCES})

target_include_directories(playground-test
    PRIVATE .)

target_link_libraries(playground-test
    playground-lib
    gtest)
