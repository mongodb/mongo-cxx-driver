# Use FetchContent to obtain libbson and libmongoc.

include(FetchContent)

message(STATUS "Download and configure C driver version ${LIBMONGOC_DOWNLOAD_VERSION} ... begin")

set(fetch_args "")
if ("${CMAKE_VERSION}" VERSION_GREATER_EQUAL "3.25.0")
    set(fetch_args "SYSTEM")
endif()

# Declare mongo-c-driver as a dependency
FetchContent_Declare(
    mongo-c-driver
    GIT_REPOSITORY https://github.com/mongodb/mongo-c-driver.git
    GIT_TAG ${LIBMONGOC_DOWNLOAD_VERSION}

    ${fetch_args}
)

FetchContent_GetProperties(mongo-c-driver)

if(NOT mongo-c-driver_POPULATED)
    set(OLD_ENABLE_TESTS ${ENABLE_TESTS})
    set(OLD_BUILD_TESTING ${BUILD_TESTING})
    set(OLD_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(OLD_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

    set(ENABLE_EXTRA_ALIGNMENT OFF)

    # Set ENABLE_TESTS to OFF to disable the test-libmongoc target in the C driver.
    # This prevents the LoadTests.cmake script from attempting to execute test-libmongoc.
    # test-libmongoc is not built with the "all" target.
    # Attempting to execute test-libmongoc results in an error: "Unable to find executable: NOT_FOUND"
    set(ENABLE_TESTS OFF)
    set(BUILD_TESTING OFF)
    string(REPLACE " -Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE " -Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    FetchContent_MakeAvailable(mongo-c-driver)
    set(CMAKE_CXX_FLAGS ${OLD_CMAKE_CXX_FLAGS})
    set(CMAKE_C_FLAGS ${OLD_CMAKE_C_FLAGS})
    set(ENABLE_TESTS ${OLD_ENABLE_TESTS})
    set(BUILD_TESTING ${OLD_BUILD_TESTING})
endif()

message(STATUS "Download and configure C driver version ${LIBMONGOC_DOWNLOAD_VERSION} ... end")
