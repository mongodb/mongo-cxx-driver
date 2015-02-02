project(MONGOCXX)

set(MONGOCXX_VERSION_MAJOR 0)
set(MONGOCXX_VERSION_MINOR 0)
set(MONGOCXX_VERSION_PATCH 1)
set(MONGOCXX_VERSION_EXTRA "")
set(MONGOCXX_VERSION ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH})

set(MONGOCXX_ABI_MAJOR 0)
set(MONGOCXX_ABI_MINOR 0)
set(MONGOCXX_ABI ${MONGOCXX_ABI_MAJOR}.${MONGOCXX_ABI_MINOR})

set(LIBMONGOC_REQUIRED_ABI_VERSION 1.0)
pkg_check_modules(LIBMONGOC REQUIRED libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION})

link_directories(${LIBMONGOC_LIBRARY_DIRS})

include_directories(
    ${LIBMONGOC_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

file(WRITE ${PROJECT_BINARY_DIR}/mongocxx.cpp "")
set(MONGOCXX_SOURCES ${PROJECT_BINARY_DIR}/mongocxx.cpp)

add_library(mongocxx SHARED
    ${MONGOCXX_SOURCES}
    $<TARGET_OBJECTS:mongocxx-driver-base>
    $<TARGET_OBJECTS:mongocxx-driver-model>
    $<TARGET_OBJECTS:mongocxx-driver-options>
    $<TARGET_OBJECTS:mongocxx-driver-private>
    $<TARGET_OBJECTS:mongocxx-driver-result>
)

set_target_properties (mongocxx PROPERTIES
    OUTPUT_NAME mongocxx
    CXX_STANDARD 11
    VERSION ${MONGOCXX_VERSION}
    SOVERSION ${MONGOCXX_ABI}
)

target_link_libraries(mongocxx bsoncxx ${LIBMONGOC_LIBRARIES})

install(TARGETS
    mongocxx
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (DIRECTORY
    DESTINATION "include/mongo/driver"
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
generate_export_header(mongocxx
    BASE_NAME LIBMONGOCXX
    EXPORT_MACRO_NAME LIBMONGOCXX_API
    EXPORT_FILE_NAME export.hpp
    STATIC_DEFINE LIBMONGOCXX_STATIC
)

add_subdirectory(base)
add_subdirectory(model)
add_subdirectory(options)
add_subdirectory(private)
add_subdirectory(result)
