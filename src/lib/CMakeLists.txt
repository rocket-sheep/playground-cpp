file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(playground-lib ${SOURCES})

target_include_directories(playground-lib
    PUBLIC .
)
