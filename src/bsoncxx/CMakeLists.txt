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
set(BSONCXX_VERSION_MINOR 4)
set(BSONCXX_VERSION_PATCH 0)
set(BSONCXX_VERSION_EXTRA "")
set(BSONCXX_ABI_VERSION _noabi)

option(BSONCXX_POLY_USE_MNMLSTC "Use MNMLSTC/core for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_STD_EXPERIMENTAL "Use std::experimental for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_SYSTEM_MNMLSTC "Obtain mnmlstc/core from system" OFF)
option(BSONCXX_POLY_USE_BOOST "Use boost for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_STD "Use C++17 std library for stdx polyfills" OFF)

# Count how many polyfill options are true-ish
set(BSONCXX_POLY_OPTIONS_SET 0)
foreach (BSONCXX_POLY_OPTION  ${BSONCXX_POLY_USE_MNMLSTC} ${BSONCXX_POLY_USE_STD_EXPERIMENTAL} ${BSONCXX_POLY_USE_BOOST} ${BSONCXX_POLY_USE_STD})
  if (${BSONCXX_POLY_OPTION})
    MATH(EXPR BSONCXX_POLY_OPTIONS_SET "${BSONCXX_POLY_OPTIONS_SET}+1")
  endif()
endforeach()

if (BSONCXX_POLY_OPTIONS_SET GREATER 1)
  # You can't ask for more than one polyfill
  message(FATAL_ERROR "Cannnot specify more than one bsoncxx polyfill choice")
elseif (BSONCXX_POLY_OPTIONS_SET EQUAL 0)
  # You can just not say, in which case we endeavor to pick a sane default:

  if (NOT CMAKE_CXX_STANDARD LESS 17)
    # If we are in C++17 mode, use the C++17 versions
    set(BSONCXX_POLY_USE_STD ON)
    message(STATUS "Auto-configuring bsoncxx to use C++17 std library polyfills since C++17 is active and user didn't specify otherwise")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
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
if (NOT BSONCXX_POLY_USE_MNMLSTC AND BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
  MESSAGE(FATAL_ERROR "Can't specify system mnmlstc when not using mnmlstc")
endif()

# Can only use STD_EXPERIMENTAL in C++14 mode
if (BSONCXX_POLY_USE_STD_EXPERIMENTAL AND CMAKE_CXX_STANDARD LESS 14)
  message(FATAL_ERROR "Can only use BSONCXX_POLY_USE_STD_EXPERIMENTAL if CMAKE_CXX_STANDARD is 14 or higher")
endif()

set(BSONCXX_VERSION_NO_EXTRA ${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}.${BSONCXX_VERSION_PATCH})
set(BSONCXX_VERSION ${BSONCXX_VERSION_NO_EXTRA}${BSONCXX_VERSION_EXTRA})
set(BSONCXX_INLINE_NAMESPACE "v${BSONCXX_ABI_VERSION}")
set(BSONCXX_HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/bsoncxx/${BSONCXX_INLINE_NAMESPACE}" CACHE INTERNAL "")

set(LIBBSON_REQUIRED_VERSION 1.13.0)
set(LIBBSON_REQUIRED_ABI_VERSION 1.0)

if (BUILD_SHARED_LIBS)
    find_package(libbson-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} REQUIRED)
    set(libbson_libraries ${BSON_LIBRARIES})
    set(libbson_include_directories ${BSON_INCLUDE_DIRS})
    set(libbson_definitions ${BSON_DEFINITIONS})
else()
    find_package(libbson-static-${LIBBSON_REQUIRED_ABI_VERSION} ${LIBBSON_REQUIRED_VERSION} REQUIRED)
    set(libbson_libraries ${BSON_STATIC_LIBRARIES})
    set(libbson_include_directories ${BSON_STATIC_INCLUDE_DIRS})
    set(libbson_definitions ${BSON_STATIC_DEFINITIONS})
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
    types/value.cpp
    validate.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

if (BSONCXX_POLY_USE_BOOST)
    find_package(Boost 1.56.0 REQUIRED)
endif()

# We define two libraries: the normal library and the testing-only library.  The testing-only
# library does not get installed, but the tests link against it instead of the normal library.  The
# only difference between the two libraries is that BSONCXX_TESTING is defined in the testing-only
# library (this define enables special testing-only behavior).
add_library(bsoncxx
    ${bsoncxx_sources}
)
add_library(bsoncxx_testing
    ${bsoncxx_sources}
)
target_compile_definitions(bsoncxx_testing PUBLIC BSONCXX_TESTING)

set_target_properties(bsoncxx_testing PROPERTIES
    OUTPUT_NAME bsoncxx-testing
)

if (BUILD_SHARED_LIBS)
    set_target_properties(bsoncxx PROPERTIES
        OUTPUT_NAME bsoncxx
    )

    foreach (TARGET bsoncxx bsoncxx_testing)
        set_target_properties(${TARGET} PROPERTIES
            VERSION ${BSONCXX_VERSION}
            DEFINE_SYMBOL BSONCXX_EXPORT
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            SOVERSION ${BSONCXX_ABI_VERSION}
        )
    endforeach(TARGET)
else()
    target_compile_definitions(bsoncxx PUBLIC BSONCXX_STATIC)
    target_compile_definitions(bsoncxx_testing PUBLIC BSONCXX_STATIC)

    set_target_properties(bsoncxx PROPERTIES
        OUTPUT_NAME bsoncxx-static
    )
endif()

generate_export_header(bsoncxx
    BASE_NAME BSONCXX
    EXPORT_MACRO_NAME BSONCXX_API
    NO_EXPORT_MACRO_NAME BSONCXX_PRIVATE
    EXPORT_FILE_NAME config/export.hpp
    STATIC_DEFINE BSONCXX_STATIC
)

generate_export_header(bsoncxx_testing
    BASE_NAME BSONCXX
    EXPORT_MACRO_NAME BSONCXX_API
    NO_EXPORT_MACRO_NAME BSONCXX_PRIVATE
    EXPORT_FILE_NAME config/export.hpp
    STATIC_DEFINE BSONCXX_STATIC
)

if (BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
    add_dependencies(bsoncxx EP_mnmlstc_core)
    add_dependencies(bsoncxx_testing EP_mnmlstc_core)
    ExternalProject_Get_Property(EP_mnmlstc_core source_dir)
    target_include_directories(bsoncxx PUBLIC ${source_dir}/include)
    target_include_directories(bsoncxx_testing PUBLIC ${source_dir}/include)
elseif (BSONCXX_POLY_USE_BOOST)
    target_include_directories(bsoncxx PUBLIC ${Boost_INCLUDE_DIRS})
    target_include_directories(bsoncxx_testing PUBLIC ${Boost_INCLUDE_DIRS})
endif()

target_include_directories(bsoncxx PRIVATE ${libbson_include_directories})
target_include_directories(bsoncxx_testing PRIVATE ${libbson_include_directories})

target_compile_definitions(bsoncxx PRIVATE ${libbson_definitions})
target_compile_definitions(bsoncxx_testing PRIVATE ${libbson_definitions})

if (BUILD_SHARED_LIBS)
    target_link_libraries(bsoncxx PRIVATE ${libbson_libraries})
    target_link_libraries(bsoncxx_testing PRIVATE ${libbson_libraries})
else()
    target_link_libraries(bsoncxx ${libbson_libraries})
    target_link_libraries(bsoncxx_testing ${libbson_libraries})
endif()

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

install(TARGETS
    bsoncxx
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
)

set(PACKAGE_INCLUDE_INSTALL_DIRS ${BSONCXX_HEADER_INSTALL_DIR})
set(PACKAGE_LIBRARY_INSTALL_DIRS ${CMAKE_INSTALL_LIBDIR})
set(PACKAGE_LIBRARIES bsoncxx)

include(CMakePackageConfigHelpers)

if (BUILD_SHARED_LIBS)
    set(PKG "libbsoncxx")
else()
    set(PKG "libbsoncxx-static")
endif()

configure_package_config_file(
  cmake/${PKG}-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${BSONCXX_VERSION}
  PATH_VARS PACKAGE_INCLUDE_INSTALL_DIRS PACKAGE_LIBRARY_INSTALL_DIRS
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
  VERSION ${BSONCXX_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${BSONCXX_VERSION}
)

add_subdirectory(test)
