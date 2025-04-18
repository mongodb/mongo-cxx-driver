# Copyright 2009-present MongoDB, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

project(BSONCXX)

ParseVersion(${BUILD_VERSION} BSONCXX)

set(BSONCXX_VERSION_NO_EXTRA ${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}.${BSONCXX_VERSION_PATCH} CACHE INTERNAL "")
set(BSONCXX_VERSION $CACHE{BSONCXX_VERSION_NO_EXTRA}${BSONCXX_VERSION_EXTRA} CACHE INTERNAL "")
message(STATUS "bsoncxx version: $CACHE{BSONCXX_VERSION}")

option(BSONCXX_POLY_USE_IMPLS "Use bsoncxx implementations for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_STD "Use C++17 std library for stdx polyfills" OFF)

option(BSONCXX_API_OVERRIDE_DEFAULT_ABI "The default ABI namespace to use for root namespace redeclarations" OFF)
mark_as_advanced(BSONCXX_API_OVERRIDE_DEFAULT_ABI)
if(BSONCXX_API_OVERRIDE_DEFAULT_ABI)
    message(WARNING "BSONCXX_API_OVERRIDE_DEFAULT_ABI is an experimental feature")
endif()

set(BSONCXX_OUTPUT_BASENAME "bsoncxx" CACHE STRING "Output bsoncxx library base name")

# Count how many polyfill options are true-ish
set(BSONCXX_POLY_OPTIONS_SET 0)

foreach(BSONCXX_POLY_OPTION ${BSONCXX_POLY_USE_IMPLS} ${BSONCXX_POLY_USE_STD})
    if(${BSONCXX_POLY_OPTION})
        MATH(EXPR BSONCXX_POLY_OPTIONS_SET "${BSONCXX_POLY_OPTIONS_SET}+1")
    endif()
endforeach()

if(BSONCXX_POLY_OPTIONS_SET GREATER 1)
    # You can't ask for more than one polyfill
    message(FATAL_ERROR "Cannnot specify more than one bsoncxx polyfill choice")
elseif(BSONCXX_POLY_OPTIONS_SET EQUAL 0)
    # You can just not say, in which case we endeavor to pick a sane default:
    if(NOT CMAKE_CXX_STANDARD LESS 17)
        # If we are in C++17 mode, use the C++17 versions
        set(BSONCXX_POLY_USE_STD ON)
        message(STATUS "Auto-configuring bsoncxx to use C++17 std library polyfills since C++17 is active and user didn't specify otherwise")
    else()
        # If enabled, use bsoncxx implementations instead of external libraries.
        set(BSONCXX_POLY_USE_IMPLS ON)
        message(STATUS "Auto-configuring bsoncxx to use its own polyfill implementations since C++17 is inactive")
    endif()
endif()

if(TARGET bson_shared OR TARGET bson_static)
    message(STATUS "Using bson targets imported via add_subdirectory() without version checks")

    if(BSONCXX_LINK_WITH_STATIC_MONGOC)
        set(bson_target bson::static)
    else()
        set(bson_target bson::shared)
    endif()
else()
    find_package(bson ${BSON_REQUIRED_VERSION} REQUIRED)

    message(STATUS "Found bson: ${bson_DIR} (found version \"${bson_VERSION}\")")

    if(BSONCXX_LINK_WITH_STATIC_MONGOC)
        set(bson_target bson::static)
    else()
        set(bson_target bson::shared)
    endif()
endif()

set(bsoncxx_sources "") # Required by bsoncxx_add_library().

add_subdirectory(include)
add_subdirectory(lib)

include(BsoncxxUtil)

set(bsoncxx_target_list "")

if(BSONCXX_BUILD_SHARED)
    bsoncxx_add_library(bsoncxx_shared "${BSONCXX_OUTPUT_BASENAME}" SHARED)
    list(APPEND bsoncxx_target_list bsoncxx_shared)

    if(WIN32)
        # Add resource-definition script for Windows shared library (.dll).
        configure_file(bsoncxx.rc.in bsoncxx.rc)
        target_sources(bsoncxx_shared PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/bsoncxx.rc)
    endif()
endif()

if(BSONCXX_BUILD_STATIC)
    bsoncxx_add_library(bsoncxx_static "${BSONCXX_OUTPUT_BASENAME}-static" STATIC)
    list(APPEND bsoncxx_target_list bsoncxx_static)
endif()

# Generate and install the export header.
if(1)
    function(bsoncxx_install_export_header TARGET)
        set(bsoncxx_export_header_custom_content "")
        string(APPEND bsoncxx_export_header_custom_content [[

#undef BSONCXX_DEPRECATED_EXPORT
#undef BSONCXX_DEPRECATED_NO_EXPORT

#if defined(_MSC_VER)
#define BSONCXX_ABI_CDECL __cdecl
#else
#define BSONCXX_ABI_CDECL
#endif

#define BSONCXX_ABI_EXPORT_CDECL(...) BSONCXX_ABI_EXPORT __VA_ARGS__ BSONCXX_ABI_CDECL

///
/// @file
/// Provides macros to control the set of symbols exported in the ABI.
///
/// @warning For internal use only!
///

///
/// @def BSONCXX_ABI_EXPORT
/// @hideinitializer
/// Exports the associated entity as part of the ABI.
///
/// @warning For internal use only!
///

///
/// @def BSONCXX_ABI_NO_EXPORT
/// @hideinitializer
/// Excludes the associated entity from being part of the ABI.
///
/// @warning For internal use only!
///

///
/// @def BSONCXX_ABI_CDECL
/// @hideinitializer
/// Expands to `__cdecl` when built with MSVC on Windows.
///
/// @warning For internal use only!
///

///
/// @def BSONCXX_ABI_EXPORT_CDECL
/// @hideinitializer
/// Equivalent to @ref BSONCXX_ABI_EXPORT with @ref BSONCXX_ABI_CDECL.
///
/// @warning For internal use only!
///

///
/// @def BSONCXX_DEPRECATED
/// @hideinitializer
/// Declares the associated entity as deprecated.
///
/// @warning For internal use only!
///
]]
        )
        generate_export_header(${TARGET}
            BASE_NAME BSONCXX_ABI
            EXPORT_MACRO_NAME BSONCXX_ABI_EXPORT
            DEPRECATED_MACRO_NAME BSONCXX_DEPRECATED
            EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/lib/bsoncxx/v1/config/export.hpp
            STATIC_DEFINE BSONCXX_STATIC
            CUSTOM_CONTENT_FROM_VARIABLE bsoncxx_export_header_custom_content
        )

        install(FILES
            ${PROJECT_BINARY_DIR}/lib/bsoncxx/v1/config/export.hpp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bsoncxx/v1/config
            COMPONENT dev
        )
    endfunction()

    # Ensure the export header is generated *at most* once.
    # Give priority to the shared library target which sets visibility properties.
    if(BSONCXX_BUILD_SHARED)
        bsoncxx_install_export_header(bsoncxx_shared)
    elseif(BSONCXX_BUILD_STATIC)
        bsoncxx_install_export_header(bsoncxx_static)
    endif()
endif()

add_subdirectory(cmake)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()

set_local_dist(src_bsoncxx_DIST_local
    CMakeLists.txt
    bsoncxx.rc.in
)

set(src_bsoncxx_DIST
    ${src_bsoncxx_DIST_local}
    ${src_bsoncxx_cmake_DIST}
    ${src_bsoncxx_include_DIST}
    ${src_bsoncxx_lib_DIST}
    ${src_bsoncxx_test_DIST}
    PARENT_SCOPE
)
