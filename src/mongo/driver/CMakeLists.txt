project(MONGOCXX)

set(MONGOCXX_VERSION_MAJOR 0)
set(MONGOCXX_VERSION_MINOR 0)
set(MONGOCXX_VERSION_PATCH 1)
set(MONGOCXX_VERSION_EXTRA "")
set(MONGOCXX_VERSION ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH})

set(MONGOCXX_REQUIRED_BSONCXX_VERSION "0.0.0")

set(MONGOCXX_ABI_VERSION 0)
set(MONGOCXX_INLINE_NAMESPACE "v${MONGOCXX_ABI_VERSION}")

set(MONGOCXX_DIRECTORY_PREFIX "v${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}")
set(MONGOCXX_INSTALL_DIR "include/mongocxx/${MONGOCXX_DIRECTORY_PREFIX}/mongo")

set(LIBMONGOC_REQUIRED_ABI_VERSION 1.0)
pkg_check_modules(LIBMONGOC REQUIRED libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION})

link_directories(${LIBMONGOC_LIBRARY_DIRS})

include_directories(
    ${LIBMONGOC_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

# driver files in top level driver directory
file (GLOB mongocxx-driver-core_headers *.hpp)
file (GLOB mongocxx-driver-core_sources *.cpp)

add_library(mongocxx SHARED
    ${mongocxx-driver-core_sources}
    $<TARGET_OBJECTS:mongocxx-driver-model>
    $<TARGET_OBJECTS:mongocxx-driver-options>
    $<TARGET_OBJECTS:mongocxx-driver-private>
    $<TARGET_OBJECTS:mongocxx-driver-result>
)

set_target_properties (mongocxx PROPERTIES
    OUTPUT_NAME mongocxx
    CXX_STANDARD 11
    VERSION ${MONGOCXX_VERSION}
    # uncomment below line when we start promising a stable ABI
    # SOVERSION ${MONGOCXX_ABI_VERSION}
)

target_link_libraries(mongocxx bsoncxx ${LIBMONGOC_LIBRARIES})

if(BUILD_UNIT_TESTS)
    add_library(mongocxx-mocked SHARED
        ${mongocxx-driver-core_sources}
        $<TARGET_OBJECTS:mongocxx-driver-model>
        $<TARGET_OBJECTS:mongocxx-driver-options>
        $<TARGET_OBJECTS:mongocxx-driver-private-mocked>
        $<TARGET_OBJECTS:mongocxx-driver-result>
    )

    set_target_properties (mongocxx-mocked PROPERTIES
        OUTPUT_NAME mongocxx-mocked
        CXX_STANDARD 11
        VERSION ${MONGOCXX_VERSION}
        SOVERSION ${MONGOCXX_ABI_VERSION}
        COMPILE_FLAGS "-DMONGOCXX_TESTING"
    )

    target_link_libraries(mongocxx-mocked bsoncxx ${LIBMONGOC_LIBRARIES})
endif()

install(TARGETS
    mongocxx
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${MONGOCXX_INSTALL_DIR}
    FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*private*" EXCLUDE
)

configure_file (
    ${PROJECT_SOURCE_DIR}/version.hpp.in
    ${PROJECT_BINARY_DIR}/version.hpp
)

# TODO: MONGOCXX_SHARED_NO_EXPORT -> LIBMONGOCXX_PRIVATE
# TODO: LIBMONGOCXX_EXPORTS -> LIBMONGOCXX_BUILDING
# TODO: Need deprecated defines? or do the same?
generate_export_header (mongocxx
    BASE_NAME LIBMONGOCXX
    EXPORT_MACRO_NAME LIBMONGOCXX_API
    EXPORT_FILE_NAME export.hpp
    STATIC_DEFINE LIBMONGOCXX_STATIC
)

configure_file (
    ${PROJECT_SOURCE_DIR}/libmongocxx.pc.in
    ${PROJECT_BINARY_DIR}/libmongocxx.pc
)

# install generated files
install (FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/version.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/export.hpp"
    DESTINATION "${MONGOCXX_INSTALL_DIR}/driver"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/libmongocxx.pc"
  DESTINATION "${PKG_CONFIG_PATH}"
)

add_subdirectory(config)
add_subdirectory(model)
add_subdirectory(options)
add_subdirectory(private)
add_subdirectory(result)
