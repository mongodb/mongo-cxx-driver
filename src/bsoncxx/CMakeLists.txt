project(BSONCXX)

set(LIBBSON_REQUIRED_ABI_VERSION 1.0)
find_package(LibBSON 1.3.0 REQUIRED)

# Update these as needed.
# TODO: read from file
set(BSONCXX_VERSION_MAJOR 0)
set(BSONCXX_VERSION_MINOR 3)
set(BSONCXX_VERSION_PATCH 0)
set(BSONCXX_VERSION_EXTRA "-pre")
set(BSONCXX_ABI_VERSION 0)

set(BSONCXX_VERSION ${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}.${BSONCXX_VERSION_PATCH})
set(BSONCXX_INLINE_NAMESPACE "v${BSONCXX_ABI_VERSION}")
set(BSONCXX_DIRECTORY_PREFIX "v${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}")
set(BSONCXX_HEADER_INSTALL_DIR "include/bsoncxx/${BSONCXX_DIRECTORY_PREFIX}" CACHE INTERNAL "")

# TODO: Not all combinations of options are valid, and USE_STD is only valid
# for C++14 compiles. Add logic to validate options and configure checks to validate
# STD_EX vs C++14.
option(BSONCXX_POLY_USE_STD_EXPERIMENTAL "Use <experimental/ for stdx polyfills" OFF)
option(BSONCXX_POLY_USE_MNMLSTC "Use mnmlstc/core for stdx polyfills" ON)
option(BSONCXX_POLY_USE_SYSTEM_MNMLSTC "Obtain mnmlstc/core from system" OFF)
option(BSONCXX_POLY_USE_BOOST "Use boost for stdx polyfills" OFF)

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
    json.cpp
    oid.cpp
    private/error_category.cpp
    private/error_code.cpp
    private/itoa.cpp
    private/libbson_error.cpp
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

if (BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
    ExternalProject_Get_Property(EP_mnmlstc_core source_dir)
    include_directories(${source_dir}/include)
elseif (BSONCXX_POLY_USE_BOOST)
    find_package(Boost REQUIRED)
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

# Add all generated targets as dependencies here. This is a proxy
# to allow the examples build to be sequenced after the mongocxx build
# since you can't currently depend on an install target in CMake.
add_custom_target(bsoncxx_built
  DEPENDS
  bsoncxx
  bsoncxx_static
  ${CMAKE_CURRENT_BINARY_DIR}/config/export.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/config/version.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/config/config.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/config/private/config.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx-config-version.cmake
)

add_subdirectory(test)
