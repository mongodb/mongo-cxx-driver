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

project(MONGOCXX)

set(LIBBSON_REQUIRED_VERSION 1.3.4)
set(LIBBSON_REQUIRED_ABI_VERSION 1.0)
find_package(LibBSON ${LIBBSON_REQUIRED_VERSION} REQUIRED)

set(LIBMONGOC_REQUIRED_VERSION 1.3.4)
set(LIBMONGOC_REQUIRED_ABI_VERSION 1.0)
find_package(LibMongoC ${LIBMONGOC_REQUIRED_VERSION} REQUIRED)

# Update these as needed.
# TODO: read from file
set(MONGOCXX_VERSION_MAJOR 3)
set(MONGOCXX_VERSION_MINOR 0)
set(MONGOCXX_VERSION_PATCH 2)
set(MONGOCXX_VERSION_EXTRA "-rc0-pre")
set(MONGOCXX_ABI_VERSION _noabi)

set(MONGOCXX_VERSION ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH}${MONGOCXX_VERSION_EXTRA})
set(MONGOCXX_INLINE_NAMESPACE "v${MONGOCXX_ABI_VERSION}")
set(MONGOCXX_HEADER_INSTALL_DIR "include/mongocxx/${MONGOCXX_INLINE_NAMESPACE}" CACHE INTERNAL "")

add_subdirectory(config)

set(mongocxx_sources
    bulk_write.cpp
    client.cpp
    collection.cpp
    cursor.cpp
    database.cpp
    exception/error_code.cpp
    exception/server_error_code.cpp
    exception/operation_exception.cpp
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
    ${LIBBSON_INCLUDE_DIRS}
    ${LIBMONGOC_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

link_directories(
  ${LIBBSON_LIBRARY_DIRS}
  ${LIBMONGOC_LIBRARY_DIRS}
)

add_library(mongocxx_static STATIC
    ${mongocxx_sources}
)

target_compile_definitions(mongocxx_static PUBLIC MONGOCXX_STATIC)

set_target_properties(mongocxx_static PROPERTIES
    OUTPUT_NAME mongocxx
)

# Follow the boost convention to disambiguate the dll and static
# library names
if (WIN32)
    set_target_properties(mongocxx_static PROPERTIES
        PREFIX lib
    )
endif()

add_library(mongocxx_mocked STATIC
    ${mongocxx_sources}
)

target_compile_definitions(mongocxx_mocked PUBLIC MONGOCXX_STATIC MONGOCXX_TESTING)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mongocxx_mocked PRIVATE /bigobj)
endif()

set_target_properties(mongocxx_mocked PROPERTIES
    OUTPUT_NAME mongocxx_mocked
    VERSION ${MONGOCXX_VERSION}
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
    EXPORT_FILE_NAME config/export.hpp
    STATIC_DEFINE MONGOCXX_STATIC
)

set(mongocxx_libs ${LIBMONGOC_LIBRARIES} ${LIBBSON_LIBRARIES})

target_link_libraries(mongocxx_static bsoncxx_static ${mongocxx_libs})
target_link_libraries(mongocxx_mocked bsoncxx_static ${mongocxx_libs})
target_link_libraries(mongocxx PUBLIC bsoncxx PRIVATE ${mongocxx_libs})

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
    ${CMAKE_CURRENT_BINARY_DIR}/config/export.hpp
    DESTINATION ${MONGOCXX_HEADER_INSTALL_DIR}/mongocxx/config
    COMPONENT dev
)

install(TARGETS
    mongocxx
    RUNTIME DESTINATION bin COMPONENT runtime
    LIBRARY DESTINATION lib COMPONENT runtime
    ARCHIVE DESTINATION lib COMPONENT dev
)

install(TARGETS
    mongocxx_static
    ARCHIVE DESTINATION lib COMPONENT dev
)

set(PACKAGE_INCLUDE_INSTALL_DIRS ${MONGOCXX_HEADER_INSTALL_DIR})
set(PACKAGE_LIBRARY_INSTALL_DIRS lib)
set(PACKAGE_LIBRARIES mongocxx)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/libmongocxx-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libmongocxx-config.cmake
  INSTALL_DESTINATION lib/cmake/libmongocxx-${MONGOCXX_VERSION}
  PATH_VARS PACKAGE_INCLUDE_INSTALL_DIRS PACKAGE_LIBRARY_INSTALL_DIRS
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/libmongocxx-config-version.cmake
  VERSION ${MONGOCXX_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/libmongocxx-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/libmongocxx-config-version.cmake
  DESTINATION lib/cmake/libmongocxx-${MONGOCXX_VERSION}
)

add_subdirectory(test)
