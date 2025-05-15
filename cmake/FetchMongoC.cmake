# Use FetchContent to obtain bson and mongoc.

include(FetchContent)

function(fetch_mongoc)
    message(STATUS "Downloading and configuring MongoDB C Driver ${MONGOC_DOWNLOAD_VERSION}...")

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

    message(STATUS "Downloading and configuring MongoDB C Driver ${MONGOC_DOWNLOAD_VERSION}... done.")
endfunction()

set(NEED_DOWNLOAD_C_DRIVER false CACHE INTERNAL "")

# Only search for packages if targets are not already imported via add_subdirectory().
if(NOT ((TARGET bson_shared OR TARGET bson_static) AND (TARGET mongoc_shared OR TARGET mongoc_static)))
    # Both libraries are required.
    find_package(bson ${BSON_REQUIRED_VERSION} QUIET)
    if(NOT bson_FOUND)
        message(STATUS "find_package(bson ${BSON_REQUIRED_VERSION}) did not succeed")
    endif()

    find_package(mongoc ${MONGOC_REQUIRED_VERSION} QUIET)
    if(NOT mongoc_FOUND)
        message(STATUS "find_package(mongoc ${MONGOC_REQUIRED_VERSION}) did not succeed")
    endif()

    if(NOT bson_FOUND OR NOT mongoc_FOUND)
        set_property(CACHE NEED_DOWNLOAD_C_DRIVER PROPERTY VALUE true)
        message(STATUS "Required MongoDB C Driver libraries not found via find_package()")
    endif()
endif()

if($CACHE{NEED_DOWNLOAD_C_DRIVER})
    message(STATUS "MongoDB C Driver library sources will be downloaded from GitHub")
    fetch_mongoc()
endif()
