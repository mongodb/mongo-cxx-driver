project(BSONCXX)

set(BSONCXX_VERSION_MAJOR 0)
set(BSONCXX_VERSION_MINOR 0)
set(BSONCXX_VERSION_PATCH 1)
set(BSONCXX_VERSION_EXTRA "")
set(BSONCXX_VERSION ${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}.${BSONCXX_VERSION_PATCH})

set(BSONCXX_ABI_VERSION 0)
set(BSONCXX_INLINE_NAMESPACE "v${BSONCXX_ABI_VERSION}")

set(BSONCXX_DIRECTORY_PREFIX "v${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}")
set(BSONCXX_INSTALL_DIR "include/bsoncxx/${BSONCXX_DIRECTORY_PREFIX}")

set(LIBBSON_REQUIRED_ABI_VERSION 1.0)
pkg_check_modules(LIBBSON REQUIRED libbson-${LIBBSON_REQUIRED_ABI_VERSION})

link_directories(${LIBBSON_LIBRARY_DIRS})

include_directories(
    ${LIBBSON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

configure_file (
    ${PROJECT_SOURCE_DIR}/version.hpp.in
    ${PROJECT_BINARY_DIR}/version.hpp
)

configure_file (
    ${PROJECT_SOURCE_DIR}/libbsoncxx.pc.in
    ${PROJECT_BINARY_DIR}/libbsoncxx.pc
    @ONLY
)

file(GLOB bsoncxx-array_headers array/*.hpp)
file(GLOB bsoncxx-array_sources array/*.cpp)
file(GLOB bsoncxx-core_headers *.hpp types/*.hpp)
file(GLOB bsoncxx-core_sources *.cpp types/*.cpp)
file(GLOB bsoncxx-builder_headers builder/*.hpp)
file(GLOB bsoncxx-builder_sources builder/*.cpp)
file(GLOB bsoncxx-builder-stream_headers builder/stream/*.hpp)
file(GLOB bsoncxx-builder-stream_sources builder/stream/*.cpp)
file(GLOB bsoncxx-document_headers document/*.hpp)
file(GLOB bsoncxx-document_sources document/*.cpp)
file(GLOB bsoncxx-private_headers private/*.hpp)
file(GLOB bsoncxx-private_sources private/*.cpp)
file(GLOB bsoncxx-stdx_headers stdx/*.hpp)
file(GLOB bsoncxx-stdx_sources stdx/*.cpp)
# note that bson_ntop is a c-style header
file(GLOB bsoncxx-util_headers util/*.hpp util/*.h)
file(GLOB bsoncxx-util_sources util/*.cpp)

add_library(bsoncxx SHARED
    ${bsoncxx-array_sources}
    ${bsoncxx-core_sources}
    ${bsoncxx-builder_sources}
    ${bsoncxx-builder-stream_sources}
    ${bsoncxx-document_sources}
    ${bsoncxx-private_sources}
    ${bsoncxx-stdx_sources}
    ${bsoncxx-util_sources}
)

set_target_properties (bsoncxx PROPERTIES
    OUTPUT_NAME bsoncxx
    VERSION ${BSONCXX_VERSION}
    DEFINE_SYMBOL BSONCXX_EXPORT
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    # uncomment this line when we start promising a stable ABI
    # SOVERSION ${BSONCXX_ABI_VERSION}
)

target_link_libraries(bsoncxx ${LIBBSON_LIBRARIES})

install(TARGETS
    bsoncxx
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# TODO: instead of doing this, use the lists of headers from above
# so we can include the generated headers and avoid having a separate
# install step for them
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${BSONCXX_INSTALL_DIR}
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
        PATTERN "*private*" EXCLUDE
)

generate_export_header(bsoncxx
    BASE_NAME BSONCXX
    EXPORT_MACRO_NAME BSONCXX_API
    NO_EXPORT_MACRO_NAME BSONCXX_PRIVATE
    EXPORT_FILE_NAME export.hpp
    STATIC_DEFINE BSONCXX_STATIC
)

# install generated files
install (FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/version.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/export.hpp"
    DESTINATION "${BSONCXX_INSTALL_DIR}/bsoncxx"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx.pc"
  DESTINATION "${PKG_CONFIG_PATH}"
)

add_subdirectory(config)

if(BUILD_UNIT_TESTS)
  add_subdirectory(test)
endif()
