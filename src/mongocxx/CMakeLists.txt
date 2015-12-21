project(MONGOCXX)

# Update these as needed.
# TODO: read from file
set(MONGOCXX_VERSION_MAJOR 0)
set(MONGOCXX_VERSION_MINOR 3)
set(MONGOCXX_VERSION_PATCH 0)
set(MONGOCXX_VERSION_EXTRA "-pre")
set(MONGOCXX_ABI_VERSION 0)
set(LIBMONGOC_REQUIRED_ABI_VERSION 1.0)

set(MONGOCXX_VERSION ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH})
set(MONGOCXX_INLINE_NAMESPACE "v${MONGOCXX_ABI_VERSION}")
set(MONGOCXX_DIRECTORY_PREFIX "v${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}")
set(MONGOCXX_HEADER_INSTALL_DIR "include/mongocxx/${MONGOCXX_DIRECTORY_PREFIX}" CACHE INTERNAL "")

pkg_check_modules(LIBMONGOC REQUIRED libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION})

add_subdirectory(config)

set(mongocxx_sources
    bulk_write.cpp
    client.cpp
    collection.cpp
    cursor.cpp
    database.cpp
    exception/operation_exception.cpp
    exception/private/error_category.cpp
    exception/private/error_code.cpp
    exception/private/mongoc_error.cpp
    hint.cpp
    insert_many_builder.cpp
    instance.cpp
    logger.cpp
    model/delete_many.cpp
    model/delete_one.cpp
    model/insert_one.cpp
    model/replace_one.cpp
    model/update_many.cpp
    model/update_one.cpp
    model/write.cpp
    options/aggregate.cpp
    options/bulk_write.cpp
    options/client.cpp
    options/count.cpp
    options/create_collection.cpp
    options/delete.cpp
    options/distinct.cpp
    options/find.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
    options/index.cpp
    options/insert.cpp
    options/modify_collection.cpp
    options/ssl.cpp
    options/update.cpp
    pipeline.cpp
    pool.cpp
    private/libbson.cpp
    private/libmongoc.cpp
    read_concern.cpp
    read_preference.cpp
    result/bulk_write.cpp
    result/delete.cpp
    result/insert_many.cpp
    result/insert_one.cpp
    result/replace_one.cpp
    result/update.cpp
    uri.cpp
    validation_criteria.cpp
    write_concern.cpp
)

include_directories(
    ${LIBMONGOC_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

link_directories(${LIBMONGOC_LIBRARY_DIRS})

if (BSONCXX_POLY_USE_MNMLSTC AND NOT BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
    ExternalProject_Get_Property(EP_mnmlstc_core source_dir)
    include_directories(${source_dir}/include)
endif()

add_library(mongocxx_static STATIC
    ${mongocxx_sources}
)

set_target_properties(mongocxx_static PROPERTIES
    OUTPUT_NAME mongocxx
)

add_library(mongocxx_mocked STATIC
    ${mongocxx_sources}
)

set_target_properties(mongocxx_mocked PROPERTIES
    OUTPUT_NAME mongocxx_mocked
    VERSION ${MONGOCXX_VERSION}
    COMPILE_FLAGS "-DMONGOCXX_TESTING"
)

add_library(mongocxx SHARED
    ${mongocxx_sources}
)

set_target_properties (mongocxx PROPERTIES
    OUTPUT_NAME mongocxx
    VERSION ${MONGOCXX_VERSION}
    DEFINE_SYMBOL MONGOCXX_EXPORTS
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    SOVERSION ${MONGOCXX_ABI_VERSION}
)

generate_export_header (mongocxx
    BASE_NAME MONGOCXX
    EXPORT_MACRO_NAME MONGOCXX_API
    NO_EXPORT_MACRO_NAME MONGOCXX_PRIVATE
    EXPORT_FILE_NAME export.hpp
    STATIC_DEFINE MONGOCXX_STATIC
)

set(mongocxx_libs bsoncxx)

target_link_libraries(mongocxx_static bsoncxx ${LIBMONGOC_LIBRARIES})
target_link_libraries(mongocxx_mocked bsoncxx ${LIBMONGOC_LIBRARIES})
target_link_libraries(mongocxx PUBLIC bsoncxx PRIVATE ${LIBMONGOC_LIBRARIES})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${MONGOCXX_HEADER_INSTALL_DIR}
    COMPONENT dev
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        REGEX ".+/mock" EXCLUDE
        REGEX ".+/private" EXCLUDE
        REGEX ".+/test" EXCLUDE
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/export.hpp
    DESTINATION ${MONGOCXX_HEADER_INSTALL_DIR}/mongocxx
    COMPONENT dev
)

install(TARGETS
    mongocxx
    LIBRARY DESTINATION lib COMPONENT runtime
)

install(TARGETS
    mongocxx_static
    ARCHIVE DESTINATION lib COMPONENT runtime
)

add_subdirectory(test)
