# Use FetchContent to obtain Catch2.

include(FetchContent)

function(fetch_catch2)
    set(fetch_args "")
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.25.0")
        list(APPEND fetch_args "SYSTEM")
    endif()

    FetchContent_Declare(
        EP_Catch2

        GIT_REPOSITORY https://github.com/catchorg/Catch2
        GIT_TAG v3.7.0
        GIT_SHALLOW TRUE
        LOG_DOWNLOAD ON

        ${fetch_args}
    )

    FetchContent_GetProperties(EP_Catch2)

    if(NOT ep_catch2_POPULATED)
        message(STATUS "Downloading Catch2...")

        # Avoid Catch2 compile warnings from being treated as errors.
        string(REPLACE " -Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        string(REPLACE " -Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

        # Ensure consistent default calling convention with test executables.
        if(MSVC)
            string(APPEND CMAKE_CXX_FLAGS " /Gv")
        endif()

        FetchContent_MakeAvailable(EP_Catch2)

        # Avoid building unnecessary targets. Use FetchContent_Declare(EXCLUDE_FROM_ALL) in CMake 3.28 and newer.
        set_property(DIRECTORY "${ep_catch2_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL ON)

        # Catch2 config vars.
        set_property(CACHE CATCH_INSTALL_DOCS PROPERTY VALUE OFF)
        set_property(CACHE CATCH_INSTALL_EXTRAS PROPERTY VALUE OFF)

        message (STATUS "Downloading Catch2... done.")
    endif()
endfunction()

fetch_catch2()
