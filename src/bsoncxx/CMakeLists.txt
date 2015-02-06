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

file(GLOB bsoncxx-core_headers ${PROJECT_SOURCE_DIR}/*.hpp)
file(GLOB bsoncxx-core_sources ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB bsoncxx-builder_headers ${PROJECT_SOURCE_DIR}/builder/*.hpp)
file(GLOB bsoncxx-builder_sources ${PROJECT_SOURCE_DIR}/builder/*.cpp)
file(GLOB bsoncxx-document_headers ${PROJECT_SOURCE_DIR}/document/*.hpp)
file(GLOB bsoncxx-document_sources ${PROJECT_SOURCE_DIR}/document/*.cpp)
file(GLOB bsoncxx-stdx_headers ${PROJECT_SOURCE_DIR}/stdx/*.hpp)
file(GLOB bsoncxx-stdx_sources ${PROJECT_SOURCE_DIR}/stdx/*.cpp)
file(GLOB bsoncxx-util_headers ${PROJECT_SOURCE_DIR}/util/*.hpp)
file(GLOB bsoncxx-util_sources ${PROJECT_SOURCE_DIR}/util/*.cpp)

add_library(bsoncxx SHARED
    ${bsoncxx-core_sources}
    ${bsoncxx-builder_sources}
    ${bsoncxx-document_sources}
    ${bsoncxx-stdx_sources}
    ${bsoncxx-util_sources}
)

set_target_properties (bsoncxx PROPERTIES
    OUTPUT_NAME bsoncxx
    CXX_STANDARD 11
    VERSION ${BSONCXX_VERSION}
    # uncomment this line when we start promising a stable ABI
    # SOVERSION ${BSONCXX_ABI_VERSION}
)

target_link_libraries(bsoncxx ${LIBBSON_LIBRARIES})

install(TARGETS
    bsoncxx
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${BSONCXX_INSTALL_DIR}
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*private*" EXCLUDE
)

configure_file (
    ${PROJECT_SOURCE_DIR}/version.hpp.in
    ${PROJECT_BINARY_DIR}/version.hpp
)

generate_export_header(bsoncxx
    BASE_NAME LIBBSONCXX
    EXPORT_MACRO_NAME LIBBSONCXX_API
    EXPORT_FILE_NAME export.hpp
    STATIC_DEFINE LIBBSONCXX_STATIC
)

configure_file (
    ${PROJECT_SOURCE_DIR}/libbsoncxx.pc.in
    ${PROJECT_BINARY_DIR}/libbsoncxx.pc
    @ONLY
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

if(BUILD_UNIT_TESTS)
  add_subdirectory(test)
endif()
