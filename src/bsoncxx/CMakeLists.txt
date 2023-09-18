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

if (BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
    # For deprecated CMake package config files.
    set(BSONCXX_POLY_MNMLSTC_DEPRECATED_INCLUDE_DIRS
        [[list(PREPEND LIBBSONCXX_INCLUDE_DIRS "${PACKAGE_PREFIX_DIR}/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc")]]
    )
    set(BSONCXX_POLY_MNMLSTC_DEPRECATED_STATIC_INCLUDE_DIRS
        [[list(PREPEND LIBBSONCXX_STATIC_INCLUDE_DIRS "${PACKAGE_PREFIX_DIR}/include/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc")]]
    )

    # For pkg-config files. Trailing space to separate cflags.
    set(BSONCXX_POLY_MNMLSTC_PKGCONFIG_INCLUDE_DIRS
        [[-I${includedir}/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc ]]
    )
    set(BSONCXX_POLY_MNMLSTC_PKGCONFIG_STATIC_INCLUDE_DIRS
        [[-I${includedir}/bsoncxx/v_noabi/bsoncxx/third_party/mnmlstc ]]
    )
endif ()

set(LIBBSON_REQUIRED_VERSION 1.24.0)
set(LIBBSON_REQUIRED_ABI_VERSION 1.0)

set(bsoncxx_pkg_dep "")
set(bsoncxx_boost_pkg_dep "")

if(TARGET bson_shared OR TARGET bson_static)
  # If these targets exist, then libbson has already been included as a project
  # sub-directory
  message ("found libbson targets declared in current build scope; version not checked")

  if(NOT BSONCXX_LINK_WITH_STATIC_MONGOC)
    set(libbson_target bson_shared)
  else()
    set(libbson_target bson_static)
  endif()

  set(bsoncxx_pkg_dep "find_dependency(bson-${LIBBSON_REQUIRED_ABI_VERSION} REQUIRED)")
else()
  # Attempt to find libbson by new package name (without lib).
  find_package(bson-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} QUIET)

  if(bson-${LIBBSON_REQUIRED_ABI_VERSION}_FOUND)
    message ("found libbson version ${bson-${LIBBSON_REQUIRED_ABI_VERSION}_VERSION}")
    if(NOT BSONCXX_LINK_WITH_STATIC_MONGOC)
      set(libbson_target mongo::bson_shared)
    else()
      set(libbson_target mongo::bson_static)
    endif()
    set(bsoncxx_pkg_dep "find_dependency(bson-${LIBBSON_REQUIRED_ABI_VERSION} REQUIRED)")
  else()
    # Require package of old libbson name (with lib).
    if(NOT BSONCXX_LINK_WITH_STATIC_MONGOC)
      find_package(libbson-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} REQUIRED)
      message ("found libbson version ${BSON_VERSION}")
      set(libbson_target ${BSON_LIBRARIES})
      set(libbson_include_directories ${BSON_INCLUDE_DIRS})
      set(libbson_definitions ${BSON_DEFINITIONS})
      set(bsoncxx_pkg_dep "find_dependency(libbson-${LIBBSON_REQUIRED_ABI_VERSION} REQUIRED)")
    else()
      find_package(libbson-static-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} REQUIRED)
      message ("found libbson version ${BSON_STATIC_VERSION}")
      set(libbson_target ${BSON_STATIC_LIBRARIES})
      set(libbson_include_directories ${BSON_STATIC_INCLUDE_DIRS})
      set(libbson_definitions ${BSON_STATIC_DEFINITIONS})
      set(bsoncxx_pkg_dep "find_dependency(libbson-static-${LIBBSON_REQUIRED_ABI_VERSION} REQUIRED)")
    endif()
  endif()
endif()

add_subdirectory(third_party)
add_subdirectory(lib/bsoncxx/v_noabi/bsoncxx/config)

set(bsoncxx_sources
    lib/bsoncxx/v_noabi/bsoncxx/array/element.cpp
    lib/bsoncxx/v_noabi/bsoncxx/array/value.cpp
    lib/bsoncxx/v_noabi/bsoncxx/array/view.cpp
    lib/bsoncxx/v_noabi/bsoncxx/builder/core.cpp
    lib/bsoncxx/v_noabi/bsoncxx/decimal128.cpp
    lib/bsoncxx/v_noabi/bsoncxx/document/element.cpp
    lib/bsoncxx/v_noabi/bsoncxx/document/value.cpp
    lib/bsoncxx/v_noabi/bsoncxx/document/view.cpp
    lib/bsoncxx/v_noabi/bsoncxx/exception/error_code.cpp
    lib/bsoncxx/v_noabi/bsoncxx/json.cpp
    lib/bsoncxx/v_noabi/bsoncxx/oid.cpp
    lib/bsoncxx/v_noabi/bsoncxx/private/itoa.cpp
    lib/bsoncxx/v_noabi/bsoncxx/string/view_or_value.cpp
    lib/bsoncxx/v_noabi/bsoncxx/types.cpp
    lib/bsoncxx/v_noabi/bsoncxx/types/bson_value/value.cpp
    lib/bsoncxx/v_noabi/bsoncxx/types/bson_value/view.cpp
    lib/bsoncxx/v_noabi/bsoncxx/validate.cpp
)

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
    if(ENABLE_TESTS)
      bsoncxx_add_library(bsoncxx_testing "bsoncxx-testing" SHARED)
    endif()
else()
    set(BSONCXX_LIBRARY_FOR_EXAMPLES "bsoncxx_static" CACHE INTERNAL "")
    if(ENABLE_TESTS)
      bsoncxx_add_library(bsoncxx_testing "bsoncxx-testing" STATIC)
    endif()
endif()
if(ENABLE_TESTS)
  target_compile_definitions(bsoncxx_testing PUBLIC BSONCXX_TESTING)
endif()

set (libdir "\${prefix}/${CMAKE_INSTALL_LIBDIR}")

if(BSONCXX_BUILD_SHARED)
    configure_file(
        cmake/libbsoncxx.pc.in
        cmake/libbsoncxx.pc
        @ONLY
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/libbsoncxx.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT dev
    )
endif()

if(BSONCXX_BUILD_STATIC)
    configure_file(
        cmake/libbsoncxx-static.pc.in
        cmake/libbsoncxx-static.pc
        @ONLY
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/libbsoncxx-static.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT dev
    )
endif()

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING
      PATTERN "*.hpp"
)

install(FILES
    ${PROJECT_BINARY_DIR}/lib/bsoncxx/v_noabi/bsoncxx/config/export.hpp
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
endif()
if(BSONCXX_POLY_USE_BOOST)
    set(bsoncxx_boost_pkg_dep "find_dependency(Boost 1.56.0 REQUIRED)")
endif()
bsoncxx_install("${bsoncxx_target_list}" "${bsoncxx_pkg_dep}" "${bsoncxx_boost_pkg_dep}")

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()

set_local_dist (src_bsoncxx_lib_v_noabi_DIST_local
   CMakeLists.txt
   lib/bsoncxx/v_noabi/bsoncxx/array/element.cpp
   lib/bsoncxx/v_noabi/bsoncxx/array/value.cpp
   lib/bsoncxx/v_noabi/bsoncxx/array/view.cpp
   lib/bsoncxx/v_noabi/bsoncxx/builder/core.cpp
   cmake/bsoncxx-config.cmake.in
   cmake/libbsoncxx-config.cmake.in
   cmake/libbsoncxx-static-config.cmake.in
   cmake/libbsoncxx-static.pc.in
   cmake/libbsoncxx.pc.in
   lib/bsoncxx/v_noabi/bsoncxx/decimal128.cpp
   lib/bsoncxx/v_noabi/bsoncxx/document/element.cpp
   lib/bsoncxx/v_noabi/bsoncxx/document/value.cpp
   lib/bsoncxx/v_noabi/bsoncxx/document/view.cpp
   lib/bsoncxx/v_noabi/bsoncxx/exception/error_code.cpp
   lib/bsoncxx/v_noabi/bsoncxx/json.cpp
   lib/bsoncxx/v_noabi/bsoncxx/oid.cpp
   lib/bsoncxx/v_noabi/bsoncxx/private/b64_ntop.hh
   lib/bsoncxx/v_noabi/bsoncxx/private/helpers.hh
   lib/bsoncxx/v_noabi/bsoncxx/private/itoa.cpp
   lib/bsoncxx/v_noabi/bsoncxx/private/itoa.hh
   lib/bsoncxx/v_noabi/bsoncxx/private/libbson.hh
   lib/bsoncxx/v_noabi/bsoncxx/private/stack.hh
   lib/bsoncxx/v_noabi/bsoncxx/private/suppress_deprecation_warnings.hh
   lib/bsoncxx/v_noabi/bsoncxx/string/view_or_value.cpp
   lib/bsoncxx/v_noabi/bsoncxx/test_util/catch.hh
   lib/bsoncxx/v_noabi/bsoncxx/test_util/export_for_testing.hh
   lib/bsoncxx/v_noabi/bsoncxx/test_util/to_string.hh
   lib/bsoncxx/v_noabi/bsoncxx/types.cpp
   lib/bsoncxx/v_noabi/bsoncxx/types/private/convert.hh
   lib/bsoncxx/v_noabi/bsoncxx/types/bson_value/private/value.hh
   lib/bsoncxx/v_noabi/bsoncxx/types/bson_value/value.cpp
   lib/bsoncxx/v_noabi/bsoncxx/types/bson_value/view.cpp
   lib/bsoncxx/v_noabi/bsoncxx/validate.cpp
)

set_local_dist (src_bsoncxx_include_v_noabi_DIST_local
    include/bsoncxx/v_noabi/bsoncxx/array/element.hpp
    include/bsoncxx/v_noabi/bsoncxx/array/value.hpp
    include/bsoncxx/v_noabi/bsoncxx/array/view_or_value.hpp
    include/bsoncxx/v_noabi/bsoncxx/array/view.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/basic/array.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/basic/document.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/basic/helpers.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/basic/impl.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/basic/kvp.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/basic/sub_array.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/basic/sub_document.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/concatenate.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/core.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/list.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/array_context.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/array.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/closed_context.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/document.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/helpers.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/key_context.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/single_context.hpp
    include/bsoncxx/v_noabi/bsoncxx/builder/stream/value_context.hpp
    include/bsoncxx/v_noabi/bsoncxx/config/compiler.hpp
    include/bsoncxx/v_noabi/bsoncxx/config/postlude.hpp
    include/bsoncxx/v_noabi/bsoncxx/config/prelude.hpp
    include/bsoncxx/v_noabi/bsoncxx/decimal128.hpp
    include/bsoncxx/v_noabi/bsoncxx/document/element.hpp
    include/bsoncxx/v_noabi/bsoncxx/document/value.hpp
    include/bsoncxx/v_noabi/bsoncxx/document/view_or_value.hpp
    include/bsoncxx/v_noabi/bsoncxx/document/view.hpp
    include/bsoncxx/v_noabi/bsoncxx/enums/binary_sub_type.hpp
    include/bsoncxx/v_noabi/bsoncxx/enums/type.hpp
    include/bsoncxx/v_noabi/bsoncxx/exception/error_code.hpp
    include/bsoncxx/v_noabi/bsoncxx/exception/exception.hpp
    include/bsoncxx/v_noabi/bsoncxx/json.hpp
    include/bsoncxx/v_noabi/bsoncxx/oid.hpp
    include/bsoncxx/v_noabi/bsoncxx/stdx/make_unique.hpp
    include/bsoncxx/v_noabi/bsoncxx/stdx/optional.hpp
    include/bsoncxx/v_noabi/bsoncxx/stdx/string_view.hpp
    include/bsoncxx/v_noabi/bsoncxx/string/to_string.hpp
    include/bsoncxx/v_noabi/bsoncxx/string/view_or_value.hpp
    include/bsoncxx/v_noabi/bsoncxx/types.hpp
    include/bsoncxx/v_noabi/bsoncxx/types/bson_value/make_value.hpp
    include/bsoncxx/v_noabi/bsoncxx/types/bson_value/value.hpp
    include/bsoncxx/v_noabi/bsoncxx/types/bson_value/view_or_value.hpp
    include/bsoncxx/v_noabi/bsoncxx/types/bson_value/view.hpp
    include/bsoncxx/v_noabi/bsoncxx/types/value.hpp
    include/bsoncxx/v_noabi/bsoncxx/util/functor.hpp
    include/bsoncxx/v_noabi/bsoncxx/validate.hpp
    include/bsoncxx/v_noabi/bsoncxx/view_or_value.hpp
)

set (src_bsoncxx_DIST
   ${src_bsoncxx_lib_v_noabi_DIST_local}
   ${src_bsoncxx_include_v_noabi_DIST_local}
   ${src_bsoncxx_config_DIST}
   ${src_bsoncxx_third_party_DIST}
   ${src_bsoncxx_test_DIST}
   PARENT_SCOPE
)
