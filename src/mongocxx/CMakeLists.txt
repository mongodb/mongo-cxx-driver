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

project(MONGOCXX)

ParseVersion(${BUILD_VERSION} MONGOCXX)

set(MONGOCXX_VERSION_NO_EXTRA ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH} CACHE INTERNAL "")
set(MONGOCXX_VERSION ${MONGOCXX_VERSION_NO_EXTRA}${MONGOCXX_VERSION_EXTRA} CACHE INTERNAL "")
message(STATUS "mongocxx version: $CACHE{MONGOCXX_VERSION}")

option(MONGOCXX_ENABLE_SSL "Enable SSL - if the underlying C driver offers it" ON)
option(MONGOCXX_ENABLE_SLOW_TESTS "Run slow tests when invoking the the test target" OFF)

option(MONGOCXX_API_OVERRIDE_DEFAULT_ABI "The default ABI namespace to use for root namespace redeclarations" OFF)
mark_as_advanced(MONGOCXX_API_OVERRIDE_DEFAULT_ABI)
if(MONGOCXX_API_OVERRIDE_DEFAULT_ABI)
    message(WARNING "MONGOCXX_API_OVERRIDE_DEFAULT_ABI is an experimental feature")
endif ()

set(MONGOCXX_OUTPUT_BASENAME "mongocxx" CACHE STRING "Output mongocxx library base name")

if(TARGET mongoc_shared OR TARGET mongoc_static)
    message(STATUS "Using mongoc targets imported via add_subdirectory() without version checks")

    if(MONGOCXX_LINK_WITH_STATIC_MONGOC)
        set(mongoc_target mongoc::static)
    else()
        set(mongoc_target mongoc::shared)
    endif()
else()
    find_package(mongoc ${MONGOC_REQUIRED_VERSION} REQUIRED)

    message(STATUS "Found mongoc: ${mongoc_DIR} (found version \"${mongoc_VERSION}\")")

    if(MONGOCXX_LINK_WITH_STATIC_MONGOC)
        set(mongoc_target mongoc::static)
    else()
        set(mongoc_target mongoc::shared)
    endif()
endif()

set(mongocxx_sources "") # Required by mongocxx_add_library().

add_subdirectory(include)
add_subdirectory(lib)

include(MongocxxUtil)

set(mongocxx_target_list "")

if(MONGOCXX_BUILD_SHARED)
    mongocxx_add_library(mongocxx_shared "${MONGOCXX_OUTPUT_BASENAME}" SHARED)
    target_link_libraries(mongocxx_shared PUBLIC bsoncxx_shared)
    list(APPEND mongocxx_target_list mongocxx_shared)

    if(WIN32)
        # Add resource-definition script for Windows shared library (.dll).
        configure_file(mongocxx.rc.in mongocxx.rc)
        target_sources(mongocxx_shared PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/mongocxx.rc)
    endif()
endif()

if(MONGOCXX_BUILD_STATIC)
    mongocxx_add_library(mongocxx_static "${MONGOCXX_OUTPUT_BASENAME}-static" STATIC)
    target_link_libraries(mongocxx_static PUBLIC bsoncxx_static)
    list(APPEND mongocxx_target_list mongocxx_static)
endif()

# Generate and install the export header.
if(1)
    function(mongocxx_install_export_header TARGET)
        set(mongocxx_export_header_custom_content "")
        string(APPEND mongocxx_export_header_custom_content [[

#undef MONGOCXX_DEPRECATED_EXPORT
#undef MONGOCXX_DEPRECATED_NO_EXPORT

#if defined(_MSC_VER)
#define MONGOCXX_ABI_CDECL __cdecl
#else
#define MONGOCXX_ABI_CDECL
#endif

#define MONGOCXX_ABI_EXPORT_CDECL(...) MONGOCXX_ABI_EXPORT __VA_ARGS__ MONGOCXX_ABI_CDECL

///
/// @file
/// Provides macros to control the set of symbols exported in the ABI.
///
/// @warning For internal use only!
///

///
/// @def MONGOCXX_ABI_EXPORT
/// @hideinitializer
/// Exports the associated entity as part of the ABI.
///
/// @warning For internal use only!
///

///
/// @def MONGOCXX_ABI_NO_EXPORT
/// @hideinitializer
/// Excludes the associated entity from being part of the ABI.
///
/// @warning For internal use only!
///

///
/// @def MONGOCXX_ABI_CDECL
/// @hideinitializer
/// Expands to `__cdecl` when built with MSVC on Windows.
///
/// @warning For internal use only!
///

///
/// @def MONGOCXX_ABI_EXPORT_CDECL
/// @hideinitializer
/// Equivalent to @ref MONGOCXX_ABI_EXPORT with @ref MONGOCXX_ABI_CDECL.
///
/// @warning For internal use only!
///

///
/// @def MONGOCXX_DEPRECATED
/// @hideinitializer
/// Declares the associated entity as deprecated.
///
/// @warning For internal use only!
///
        ]]
        )
        generate_export_header(${TARGET}
            BASE_NAME MONGOCXX_ABI
            EXPORT_MACRO_NAME MONGOCXX_ABI_EXPORT
            DEPRECATED_MACRO_NAME MONGOCXX_DEPRECATED
            EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/lib/mongocxx/v1/config/export.hpp
            STATIC_DEFINE MONGOCXX_STATIC
            CUSTOM_CONTENT_FROM_VARIABLE mongocxx_export_header_custom_content
        )

        install(FILES
            ${PROJECT_BINARY_DIR}/lib/mongocxx/v1/config/export.hpp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mongocxx/v1/config
            COMPONENT dev
        )
    endfunction()

    # Ensure the export header is generated *at most* once.
    # Give priority to the shared library target which sets visibility properties.
    if(MONGOCXX_BUILD_SHARED)
        mongocxx_install_export_header(mongocxx_shared)
    elseif(MONGOCXX_BUILD_STATIC)
        mongocxx_install_export_header(mongocxx_static)
    endif()
endif()

add_subdirectory(cmake)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()

set_local_dist(src_mongocxx_DIST_local
    CMakeLists.txt
    mongocxx.rc.in
)

set(src_mongocxx_DIST
    ${src_mongocxx_DIST_local}
    ${src_mongocxx_cmake_DIST}
    ${src_mongocxx_include_DIST}
    ${src_mongocxx_lib_DIST}
    ${src_mongocxx_test_DIST}
    PARENT_SCOPE
)
