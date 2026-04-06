# Runtime checks
# --------------

if(CMAKE_BUILD_TYPE MATCHES "Sanitize")
    list(APPEND SANITIZE_FLAGS
        # Enable runtime checks:
        -fstack-protector # stack corruption detection
        -fsanitize=address # AddressSanitizer
        -fsanitize-address-use-after-scope # AddressSanitizer
        -fsanitize=undefined # UndefinedBehaviorSanitizer
        # ... and abort upon these errors:
        -fno-sanitize-recover=undefined
    )
    if(NOT ${APPLE})
        list(APPEND SANITIZE_FLAGS -fsanitize=memory)
    endif()

    add_compile_options(${SANITIZE_FLAGS})

    # WARNING: Runtime check flags must also be passed to the linker.
    list(JOIN SANITIZE_FLAGS " " SANITIZE_FLAGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_FLAGS}")
endif()
