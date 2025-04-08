# Use FetchContent to obtain libbson and libmongoc.

include(FetchContent)

function(fetch_mongoc)
    message(STATUS "Download and configure C driver version ${LIBMONGOC_DOWNLOAD_VERSION} ... end")

    set(fetch_args "")
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.25.0")
        list(APPEND fetch_args "SYSTEM")
    endif()

    # Declare mongo-c-driver as a dependency
    FetchContent_Declare(
        mongo-c-driver
        GIT_REPOSITORY https://github.com/mongodb/mongo-c-driver.git
        GIT_TAG ${MONGOC_DOWNLOAD_VERSION}

        ${fetch_args}
    )

    FetchContent_GetProperties(mongo-c-driver)

    if(NOT mongo-c-driver_POPULATED)
        # Must ensure BUILD_VERSION is not inherited either as a normal variable or as a cache variable.
        unset(BUILD_VERSION)
        unset(OLD_CACHE_BUILD_VERSION)
        if(DEFINED CACHE{BUILD_VERSION})
            set(OLD_CACHE_BUILD_VERSION $CACHE{BUILD_VERSION})
            unset(BUILD_VERSION CACHE)
        endif()

        # Disable unnecessary targets and potential conflicts with C++ Driver options.
        set(ENABLE_TESTS OFF)
        set(BUILD_TESTING OFF)
        string(REPLACE " -Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE " -Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

        FetchContent_MakeAvailable(mongo-c-driver)

        # Restore prior value of BUILD_VERSION cache variable only if was previously set.
        if(DEFINED OLD_CACHE_BUILD_VERSION)
            set(BUILD_VERSION ${OLD_CACHE_BUILD_VERSION} CACHE STRING "Library version (for both bsoncxx and mongocxx)")
        endif()
    endif()

    message(STATUS "Download and configure C driver version ${LIBMONGOC_DOWNLOAD_VERSION} ... end")
endfunction()

fetch_mongoc()
