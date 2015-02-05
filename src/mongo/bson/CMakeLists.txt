project(BSONCXX)

set(BSONCXX_VERSION_MAJOR 0)
set(BSONCXX_VERSION_MINOR 0)
set(BSONCXX_VERSION_PATCH 1)
set(BSONCXX_VERSION_EXTRA "")
set(BSONCXX_VERSION ${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}.${BSONCXX_VERSION_PATCH})

set(BSONCXX_ABI_VERSION 0)
set(BSONCXX_INLINE_NAMESPACE "v${BSONCXX_ABI_VERSION}")

set(BSONCXX_DIRECTORY_PREFIX "v${BSONCXX_VERSION_MAJOR}.${BSONCXX_VERSION_MINOR}")
set(BSONCXX_INSTALL_DIR "include/bsoncxx/${BSONCXX_DIRECTORY_PREFIX}/mongo")

set(LIBBSON_REQUIRED_ABI_VERSION 1.0)
pkg_check_modules(LIBBSON REQUIRED libbson-${LIBBSON_REQUIRED_ABI_VERSION})

link_directories(${LIBBSON_LIBRARY_DIRS})

include_directories(
    ${LIBBSON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

file(GLOB BSONCXX_HEADERS *.hpp)
file(GLOB BSONCXX_SOURCES *.cpp)

add_library(bsoncxx SHARED
    ${BSONCXX_SOURCES}
    $<TARGET_OBJECTS:bsoncxx-builder>
    $<TARGET_OBJECTS:bsoncxx-document>
    $<TARGET_OBJECTS:bsoncxx-stdx>
    $<TARGET_OBJECTS:bsoncxx-util>
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
)

# install generated files
install (FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/version.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/export.hpp"
    DESTINATION "${BSONCXX_INSTALL_DIR}/bson"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/libbsoncxx.pc"
  DESTINATION "${PKG_CONFIG_PATH}"
)

add_subdirectory(config)
add_subdirectory(builder)
add_subdirectory(document)
add_subdirectory(stdx)
add_subdirectory(util)
