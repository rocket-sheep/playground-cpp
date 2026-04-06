# Assertions
# ----------

if(CMAKE_BUILD_TYPE MATCHES "Profile|Release")
    list(APPEND FLAGS "-DNDEBUG")
endif()
