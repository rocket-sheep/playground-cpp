# Macro to enable/disable/change severity of a warning, optionally based on compiler name and version.
#
# @param $0 Action = `enable` | `no_error` | `disable`
# @param $1 Warning name
# @param $2 (optional) Compiler regex: macro only executed when compiler name matches
# @param $3 (optional) Compiler minimum version: macro only executed when compiler version greater or equal
macro(warning action warning_name)
if (${ARGC} LESS 3 OR ${CMAKE_CXX_COMPILER_ID} MATCHES "${ARGV2}")
    if (${ARGC} LESS 4 OR ${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL "${ARGV3}")
        if (${action} STREQUAL "enable")
            list(APPEND WARNINGS "-W${warning_name}")
        elseif (${action} STREQUAL "no_error")
            list(APPEND WARNINGS "-Wno-error=${warning_name}")
        elseif (${action} STREQUAL "disable")
            list(APPEND WARNINGS "-Wno-${warning_name}")
        else ()
            message(FATAL_ERROR "Unknown warning action `${action}`")
        endif ()
    endif ()
endif ()
endmacro ()

# ----------------------------------------------------------------------
# Warning selection
# ----------------------------------------------------------------------

# Activate most warnings by default
# ---------------------------------

list(APPEND WARNINGS -Wall)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND WARNINGS -Wextra -Wpedantic)
endif ()

# ... except a few annoying ones:

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

warning(disable unsequenced)
warning(disable unused-parameter)

# Oddly enough, `-Wall` does not even enable everything:
warning(enable shadow)

endif ()

# ----------------------------------------------------------------------
# Warning severity
# ----------------------------------------------------------------------

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

# Make every warning an error...
list(APPEND WARNINGS -Werror)

# ... except specific warnings:

# Unused code warnings are useful, but they do not deserve a hard error.
warning(no_error unused-function)
warning(no_error unused-local-typedef)
warning(no_error unused-member-function)
warning(no_error unused-private-field)
warning(no_error unused-template)
warning(no_error unused-but-set-variable)
warning(no_error unused-variable)

# Deprecated functions can still be used
warning(no_error deprecated-declarations)

# Misc.
warning(no_error misleading-indentation)

endif ()

# ----------------------------------------------------------------------
# Wrap up
# ----------------------------------------------------------------------

# Convert semicolon-separated list to whitespace-separated list.
list(APPEND FLAGS ${WARNINGS})
