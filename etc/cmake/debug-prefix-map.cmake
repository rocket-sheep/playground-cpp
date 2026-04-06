# IMPORTANT: If the project root dir is a symbolic link, not embedding the
# real path in the debug information will prevent the debugger from resolving
# the breakpoints. Fortunately, Clang has an option to re-map paths.
# We set it project wide to allow cross-module debugging.

get_filename_component(PROJECT_SOURCE_DIR_REALPATH "${PROJECT_SOURCE_DIR}" REALPATH)
message(STATUS "Project source dir (orig): ${PROJECT_SOURCE_DIR}")
message(STATUS "Project source dir (real): ${PROJECT_SOURCE_DIR_REALPATH}")

add_compile_options(
    "-fdebug-prefix-map=${PROJECT_SOURCE_DIR}=${PROJECT_SOURCE_DIR_REALPATH}"
)
