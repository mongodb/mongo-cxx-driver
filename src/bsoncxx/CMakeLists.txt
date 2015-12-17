project(BSONCXX)

# Update these as needed.
# TODO: read from file
set(BSONCXX_VERSION_MAJOR 0)
set(BSONCXX_VERSION_MINOR 3)
set(BSONCXX_VERSION_PATCH 0)
set(BSONCXX_VERSION_EXTRA "-pre")
set(BSONCXX_ABI_VERSION 0)
set(LIBBSON_REQUIRED_ABI_VERSION 1.0)

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

pkg_check_modules(LIBBSON REQUIRED libbson-${LIBBSON_REQUIRED_ABI_VERSION})

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
endif()

add_library(bsoncxx_static STATIC
    ${bsoncxx_sources}
)

set_target_properties(bsoncxx_static PROPERTIES
    OUTPUT_NAME bsoncxx
)

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
    EXPORT_FILE_NAME export.hpp
    STATIC_DEFINE BSONCXX_STATIC
)

if (BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
    add_dependencies(bsoncxx_static EP_mnmlstc_core)
    add_dependencies(bsoncxx EP_mnmlstc_core)
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
    ${CMAKE_CURRENT_BINARY_DIR}/export.hpp
    DESTINATION ${BSONCXX_HEADER_INSTALL_DIR}/bsoncxx
    COMPONENT dev
)

install(TARGETS
    bsoncxx
    LIBRARY DESTINATION lib COMPONENT runtime
)

install(TARGETS
    bsoncxx_static
    ARCHIVE DESTINATION lib COMPONENT runtime
)

add_subdirectory(test)
