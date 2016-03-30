# Copyright 2016 MongoDB Inc.
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

# Update these as needed.
# TODO: read from file
set(BSONCXX_VERSION_MAJOR 3)
set(BSONCXX_VERSION_MINOR 0)
set(BSONCXX_VERSION_PATCH 2)
set(BSONCXX_VERSION_EXTRA "-rc0-pre")
set(BSONCXX_ABI_VERSION _noabi)

set(BSONCXX_POLY_USE_MNMLSTC_DEFAULT ON)
set(BSONCXX_POLY_USE_BOOST_DEFAULT OFF)

# MSVC can't handle mnmlstc yet, default to boost on that platform
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(BSONCXX_POLY_USE_MNMLSTC_DEFAULT OFF)
  set(BSONCXX_POLY_USE_BOOST_DEFAULT ON)
endif()

option(BSONCXX_POLY_USE_MNMLSTC "Use MNMLSTC/core for stdx polyfills" ${BSONCXX_POLY_USE_MNMLSTC_DEFAULT})
option(BSONCXX_POLY_USE_STD_EXPERIMENTAL "Use std::experimental for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_SYSTEM_MNMLSTC "Obtain mnmlstc/core from system" OFF)
option(BSONCXX_POLY_USE_BOOST "Use boost for stdx polyfills" ${BSONCXX_POLY_USE_BOOST_DEFAULT})

# If the user explicitly selected boost or std::experimental
# turn off mnmlstc
if (BSONCXX_POLY_USE_BOOST OR BSONCXX_POLY_USE_STD_EXPERIMENTAL)
  set(BSONCXX_POLY_USE_MNMLSTC OFF)
endif()

# It doesn't make sense to say we aren't using MNMLSTC but then
# request the system version of it.
if (NOT BSONCXX_POLY_USE_MNMLSTC AND BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
  MESSAGE(FATAL_ERROR "Can't specify system mnmlstc when using boost or std::experimental for bsoncxx polyfills")
endif()

# It doesn't make sense to request both other sources of polyfills
if (BSONCXX_POLY_USE_BOOST AND BSONCXX_POLY_USE_STD_EXPERIMENTAL)
  message(FATAL_ERROR "Can't choose both boost and std::experimental for bsoncx polyfills")
endif()

# Can only use STD_EXPERIMENTAL in C++14 mode
if (BSONCXX_POLY_USE_STD_EXPERIMENTAL AND CMAKE_CXX_STANDARD LESS 14)
  message(FATAL_ERROR "Can only use BSONCXX_POLY_USE_STD_EXPERIMENTAL if CMAKE_CXX_STANDARD is 14 or higher")
endif()

set(BSONCXX_VERSION ${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}.${BSONCXX_VERSION_PATCH}${BSONCXX_VERSION_EXTRA})
set(BSONCXX_INLINE_NAMESPACE "v${BSONCXX_ABI_VERSION}")
set(BSONCXX_HEADER_INSTALL_DIR "include/bsoncxx/${BSONCXX_INLINE_NAMESPACE}" CACHE INTERNAL "")

set(LIBBSON_REQUIRED_VERSION 1.3.4)
set(LIBBSON_REQUIRED_ABI_VERSION 1.0)
find_package(LibBSON ${LIBBSON_REQUIRED_VERSION} REQUIRED)

add_subdirectory(third_party)
add_subdirectory(config)

set(bsoncxx_sources
    array/element.cpp
    array/value.cpp
    array/view.cpp
    builder/core.cpp
    document/element.cpp
    document/value.cpp
    document/view.cpp
    exception/error_code.cpp
    json.cpp
    oid.cpp
    private/itoa.cpp
    string/view_or_value.cpp
    types.cpp
    types/value.cpp
    validate.cpp
)

include_directories(
    ${LIBBSON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

link_directories(${LIBBSON_LIBRARY_DIRS})

if (BSONCXX_POLY_USE_BOOST)
    find_package(Boost 1.56.0 REQUIRED)
endif()

add_library(bsoncxx_static STATIC
    ${bsoncxx_sources}
)

target_compile_definitions(bsoncxx_static PUBLIC BSONCXX_STATIC)

set_target_properties(bsoncxx_static PROPERTIES
    OUTPUT_NAME bsoncxx
)

# Follow the boost convention to disambiguate the dll and static
# library names
if (WIN32)
    set_target_properties(bsoncxx_static PROPERTIES
        PREFIX lib
    )
endif()

add_library(bsoncxx SHARED
    ${bsoncxx_sources}
)

set(bsoncxx_libs ${LIBBSON_LIBRARIES})

set_target_properties(bsoncxx PROPERTIES
    OUTPUT_NAME bsoncxx
    VERSION ${BSONCXX_VERSION}
    DEFINE_SYMBOL BSONCXX_EXPORT
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    SOVERSION ${BSONCXX_ABI_VERSION}
)

generate_export_header(bsoncxx
    BASE_NAME BSONCXX
    EXPORT_MACRO_NAME BSONCXX_API
    NO_EXPORT_MACRO_NAME BSONCXX_PRIVATE
    EXPORT_FILE_NAME config/export.hpp
    STATIC_DEFINE BSONCXX_STATIC
)

if (BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
    add_dependencies(bsoncxx_static EP_mnmlstc_core)
    add_dependencies(bsoncxx EP_mnmlstc_core)
    ExternalProject_Get_Property(EP_mnmlstc_core source_dir)
    target_include_directories(bsoncxx_static PUBLIC ${source_dir}/include)
    target_include_directories(bsoncxx PUBLIC ${source_dir}/include)
elseif (BSONCXX_POLY_USE_BOOST)
    target_include_directories(bsoncxx_static PUBLIC ${Boost_INCLUDE_DIRS})
    target_include_directories(bsoncxx PUBLIC ${Boost_INCLUDE_DIRS})
endif()

target_link_libraries(bsoncxx_static ${bsoncxx_libs})
target_link_libraries(bsoncxx PRIVATE ${bsoncxx_libs})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${BSONCXX_HEADER_INSTALL_DIR}
    COMPONENT dev
    FILES_MATCHING
      PATTERN "*.h"
      PATTERN "*.hpp"
      REGEX ".+/private" EXCLUDE
      REGEX ".+/test" EXCLUDE
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config/export.hpp
    DESTINATION ${BSONCXX_HEADER_INSTALL_DIR}/bsoncxx/config
    COMPONENT dev
)

install(TARGETS
    bsoncxx
    RUNTIME DESTINATION bin COMPONENT runtime
    LIBRARY DESTINATION lib COMPONENT runtime
    ARCHIVE DESTINATION lib COMPONENT dev
)

install(TARGETS
    bsoncxx_static
    ARCHIVE DESTINATION lib COMPONENT dev
)

set(PACKAGE_INCLUDE_INSTALL_DIRS ${BSONCXX_HEADER_INSTALL_DIR})
set(PACKAGE_LIBRARY_INSTALL_DIRS lib)
set(PACKAGE_LIBRARIES bsoncxx)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/libbsoncxx-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx-config.cmake
  INSTALL_DESTINATION lib/cmake/libbsoncxx-${BSONCXX_VERSION}
  PATH_VARS PACKAGE_INCLUDE_INSTALL_DIRS PACKAGE_LIBRARY_INSTALL_DIRS
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx-config-version.cmake
  VERSION ${BSONCXX_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx-config-version.cmake
  DESTINATION lib/cmake/libbsoncxx-${BSONCXX_VERSION}
)

add_subdirectory(test)
