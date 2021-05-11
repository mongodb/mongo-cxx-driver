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

ParseVersion (${BUILD_VERSION} BSONCXX)
# TODO: read from file
set(BSONCXX_ABI_VERSION _noabi)

option(BSONCXX_POLY_USE_MNMLSTC "Use MNMLSTC/core for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_STD_EXPERIMENTAL "Use std::experimental for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_SYSTEM_MNMLSTC "Obtain mnmlstc/core from system" OFF)
option(BSONCXX_POLY_USE_BOOST "Use boost for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_STD "Use C++17 std library for stdx polyfills" OFF)

set(BSONCXX_OUTPUT_BASENAME "bsoncxx" CACHE STRING "Output bsoncxx library base name")

# Count how many polyfill options are true-ish
set(BSONCXX_POLY_OPTIONS_SET 0)
foreach (BSONCXX_POLY_OPTION  ${BSONCXX_POLY_USE_MNMLSTC} ${BSONCXX_POLY_USE_STD_EXPERIMENTAL} ${BSONCXX_POLY_USE_BOOST} ${BSONCXX_POLY_USE_STD})
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
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Otherwise, since MSVC can't handle MNMLSTC, default to boost
    set(BSONCXX_POLY_USE_BOOST ON)
    message(STATUS "Auto-configuring bsoncxx to use boost std library polyfills since C++17 is inactive and compiler is MSVC")
  else()
    # Otherwise, we are on a platform that can handle MNMLSTC
    set(BSONCXX_POLY_USE_MNMLSTC ON)
    message(STATUS "Auto-configuring bsoncxx to use MNMLSTC for polyfills since C++17 is inactive")
  endif()

endif()

# It doesn't make sense to say we aren't using MNMLSTC but then
# request the system version of it.
if(NOT BSONCXX_POLY_USE_MNMLSTC AND BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
  MESSAGE(FATAL_ERROR "Can't specify system mnmlstc when not using mnmlstc")
endif()

# Can only use STD_EXPERIMENTAL in C++14 mode
if(BSONCXX_POLY_USE_STD_EXPERIMENTAL AND CMAKE_CXX_STANDARD LESS 14)
  message(FATAL_ERROR "Can only use BSONCXX_POLY_USE_STD_EXPERIMENTAL if CMAKE_CXX_STANDARD is 14 or higher")
endif()

set(BSONCXX_VERSION_NO_EXTRA ${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}.${BSONCXX_VERSION_PATCH})
set(BSONCXX_VERSION ${BSONCXX_VERSION_NO_EXTRA}${BSONCXX_VERSION_EXTRA})
message ("bsoncxx version: ${BSONCXX_VERSION}")
set(BSONCXX_INLINE_NAMESPACE "v${BSONCXX_ABI_VERSION}")
set(BSONCXX_HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/bsoncxx/${BSONCXX_INLINE_NAMESPACE}" CACHE INTERNAL "")

set(LIBBSON_REQUIRED_VERSION 1.13.0)
set(LIBBSON_REQUIRED_ABI_VERSION 1.0)

set(bsoncxx_pkg_dep "")

# Attempt to find libbson by new package name (without lib).
find_package(bson-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} QUIET)

if(bson-${LIBBSON_REQUIRED_ABI_VERSION}_FOUND)
  message ("found libbson version ${bson-${LIBBSON_REQUIRED_ABI_VERSION}_VERSION}")
  if(NOT BSONCXX_LINK_WITH_STATIC_MONGOC)
    set(libbson_target mongo::bson_shared)
  else()
    set(libbson_target mongo::bson_static)
  endif()

  if(BSONCXX_BUILD_STATIC)
    set(bsoncxx_pkg_dep "find_dependency(bson-1.0 REQUIRED)")
  endif()
else()
  # Require package of old libbson name (with lib).
  if(NOT BSONCXX_LINK_WITH_STATIC_MONGOC)
    find_package(libbson-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} REQUIRED)
    message ("found libbson version ${BSON_VERSION}")
    set(libbson_target ${BSON_LIBRARIES})
    set(libbson_include_directories ${BSON_INCLUDE_DIRS})
    set(libbson_definitions ${BSON_DEFINITIONS})
    if(BSONCXX_BUILD_STATIC)
      set(bsoncxx_pkg_dep "find_dependency(libbson-1.0 REQUIRED)")
    endif()
  else()
    find_package(libbson-static-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} REQUIRED)
    message ("found libbson version ${BSON_STATIC_VERSION}")
    set(libbson_target ${BSON_STATIC_LIBRARIES})
    set(libbson_include_directories ${BSON_STATIC_INCLUDE_DIRS})
    set(libbson_definitions ${BSON_STATIC_DEFINITIONS})
    if(BSONCXX_BUILD_STATIC)
      set(bsoncxx_pkg_dep "find_dependency(libbson-static-1.0 REQUIRED)")
    endif()
  endif()
endif()

add_subdirectory(third_party)
add_subdirectory(config)

set(bsoncxx_sources
    array/element.cpp
    array/value.cpp
    array/view.cpp
    builder/core.cpp
    decimal128.cpp
    document/element.cpp
    document/value.cpp
    document/view.cpp
    exception/error_code.cpp
    json.cpp
    oid.cpp
    private/itoa.cpp
    string/view_or_value.cpp
    types.cpp
    types/bson_value/value.cpp
    types/bson_value/view.cpp
    validate.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

if(BSONCXX_POLY_USE_BOOST)
    find_package(Boost 1.56.0 REQUIRED)
endif()

# We define both the normal libraries and the testing-only library.  The testing-only
# library does not get installed, but the tests link against it instead of the normal library.  The
# only difference between the libraries is that BSONCXX_TESTING is defined in the testing-only
# library (this define enables special testing-only behavior).

include(BsoncxxUtil)

if(BSONCXX_BUILD_SHARED)
    bsoncxx_add_library(bsoncxx_shared "${BSONCXX_OUTPUT_BASENAME}" SHARED)
endif()

if(BSONCXX_BUILD_STATIC)
    bsoncxx_add_library(bsoncxx_static "${BSONCXX_OUTPUT_BASENAME}-static" STATIC)
endif()

if(BSONCXX_BUILD_SHARED)
    set(BSONCXX_LIBRARY_FOR_EXAMPLES "bsoncxx_shared" CACHE INTERNAL "")
    bsoncxx_add_library(bsoncxx_testing "bsoncxx-testing" SHARED)
else()
    set(BSONCXX_LIBRARY_FOR_EXAMPLES "bsoncxx_static" CACHE INTERNAL "")
    bsoncxx_add_library(bsoncxx_testing "bsoncxx-testing" STATIC)
endif()
target_compile_definitions(bsoncxx_testing PUBLIC BSONCXX_TESTING)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${BSONCXX_HEADER_INSTALL_DIR}
    COMPONENT dev
    FILES_MATCHING
      PATTERN "*.hpp"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config/export.hpp
    DESTINATION ${BSONCXX_HEADER_INSTALL_DIR}/bsoncxx/config
    COMPONENT dev
)

set(PACKAGE_INCLUDE_INSTALL_DIRS ${BSONCXX_HEADER_INSTALL_DIR})
set(PACKAGE_LIBRARY_INSTALL_DIRS ${CMAKE_INSTALL_LIBDIR})
set(PACKAGE_LIBRARIES bsoncxx)

include(CMakePackageConfigHelpers)

set(bsoncxx_target_list "")
if(BSONCXX_BUILD_SHARED)
    bsoncxx_install_deprecated_cmake(bsoncxx)
    list(APPEND bsoncxx_target_list bsoncxx_shared)
endif()

if(BSONCXX_BUILD_STATIC)
    bsoncxx_install_deprecated_cmake(bsoncxx-static)
    list(APPEND bsoncxx_target_list bsoncxx_static)
    set(bsoncxx_pkg_dep "find_dependency(bson-1.0 REQUIRED)")
endif()
bsoncxx_install("${bsoncxx_target_list}" "${bsoncxx_pkg_dep}")

add_subdirectory(test)

set_local_dist (src_bsoncxx_DIST_local
   CMakeLists.txt
   array/element.cpp
   array/element.hpp
   array/value.cpp
   array/value.hpp
   array/view.cpp
   array/view.hpp
   array/view_or_value.hpp
   builder/basic/array.hpp
   builder/basic/document.hpp
   builder/basic/helpers.hpp
   builder/basic/impl.hpp
   builder/basic/kvp.hpp
   builder/basic/sub_array.hpp
   builder/basic/sub_document.hpp
   builder/concatenate.hpp
   builder/core.cpp
   builder/core.hpp
   builder/stream/array.hpp
   builder/stream/array_context.hpp
   builder/stream/closed_context.hpp
   builder/stream/document.hpp
   builder/stream/helpers.hpp
   builder/stream/key_context.hpp
   builder/stream/single_context.hpp
   builder/stream/value_context.hpp
   cmake/bsoncxx-config.cmake.in
   cmake/libbsoncxx-config.cmake.in
   cmake/libbsoncxx-static-config.cmake.in
   decimal128.cpp
   decimal128.hpp
   document/element.cpp
   document/element.hpp
   document/value.cpp
   document/value.hpp
   document/view.cpp
   document/view.hpp
   document/view_or_value.hpp
   enums/binary_sub_type.hpp
   enums/type.hpp
   exception/error_code.cpp
   exception/error_code.hpp
   exception/exception.hpp
   json.cpp
   json.hpp
   oid.cpp
   oid.hpp
   private/b64_ntop.hh
   private/helpers.hh
   private/itoa.cpp
   private/itoa.hh
   private/libbson.hh
   private/stack.hh
   private/suppress_deprecation_warnings.hh
   stdx/make_unique.hpp
   stdx/optional.hpp
   stdx/string_view.hpp
   string/to_string.hpp
   string/view_or_value.cpp
   string/view_or_value.hpp
   test_util/catch.hh
   test_util/export_for_testing.hh
   types.cpp
   types.hpp
   types/value.hpp
   types/private/convert.hh
   types/bson_value/make_value.hpp
   types/bson_value/private/value.hh
   types/bson_value/value.cpp
   types/bson_value/value.hpp
   types/bson_value/view.cpp
   types/bson_value/view.hpp
   types/bson_value/view_or_value.hpp
   util/functor.hpp
   validate.cpp
   validate.hpp
   view_or_value.hpp
)

set (src_bsoncxx_DIST
   ${src_bsoncxx_DIST_local}
   ${src_bsoncxx_config_DIST}
   ${src_bsoncxx_third_party_DIST}
   ${src_bsoncxx_test_DIST}
   PARENT_SCOPE
)
