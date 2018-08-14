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

# Update these as needed.
# TODO: read from file
set(MONGOCXX_VERSION_MAJOR 3)
set(MONGOCXX_VERSION_MINOR 4)
set(MONGOCXX_VERSION_PATCH 0)
set(MONGOCXX_VERSION_EXTRA "-rc0-pre")
set(MONGOCXX_ABI_VERSION _noabi)

option(MONGOCXX_ENABLE_SSL "Enable SSL - if the underlying C driver offers it" ON)
option(MONGOCXX_ENABLE_SLOW_TESTS "Run slow tests when invoking the the test target" OFF)

set(MONGOCXX_VERSION_NO_EXTRA ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH})
set(MONGOCXX_VERSION ${MONGOCXX_VERSION_NO_EXTRA}${MONGOCXX_VERSION_EXTRA})
set(MONGOCXX_INLINE_NAMESPACE "v${MONGOCXX_ABI_VERSION}")
set(MONGOCXX_HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/mongocxx/${MONGOCXX_INLINE_NAMESPACE}" CACHE INTERNAL "")

set(LIBMONGOC_REQUIRED_VERSION 1.10.0)
set(LIBMONGOC_REQUIRED_ABI_VERSION 1.0)

if (BUILD_SHARED_LIBS)
    find_package(libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} REQUIRED)
    set(libmongoc_libraries ${MONGOC_LIBRARIES})
    set(libmongoc_include_directories ${MONGOC_INCLUDE_DIRS})
    set(libmongoc_definitions ${MONGOC_DEFINITIONS})
else()
    find_package(libmongoc-static-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} REQUIRED)
    set(libmongoc_libraries ${MONGOC_STATIC_LIBRARIES})
    set(libmongoc_include_directories ${MONGOC_STATIC_INCLUDE_DIRS})
    set(libmongoc_definitions ${MONGOC_STATIC_DEFINITIONS})
endif()

add_subdirectory(config)

set(mongocxx_sources
    bulk_write.cpp
    client.cpp
    client_session.cpp
    change_stream.cpp
    collection.cpp
    cursor.cpp
    database.cpp
    events/command_failed_event.cpp
    events/command_started_event.cpp
    events/command_succeeded_event.cpp
    exception/error_code.cpp
    exception/operation_exception.cpp
    exception/server_error_code.cpp
    gridfs/bucket.cpp
    gridfs/downloader.cpp
    gridfs/uploader.cpp
    hint.cpp
    index_model.cpp
    index_view.cpp
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
    options/apm.cpp
    options/bulk_write.cpp
    options/change_stream.cpp
    options/client.cpp
    options/client_session.cpp
    options/count.cpp
    options/create_collection.cpp
    options/create_view.cpp
    options/delete.cpp
    options/distinct.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
    options/find.cpp
    options/gridfs/bucket.cpp
    options/gridfs/upload.cpp
    options/index.cpp
    options/index_view.cpp
    options/insert.cpp
    options/modify_collection.cpp
    options/pool.cpp
    options/private/rewriter.cpp
    options/ssl.cpp
    options/update.cpp
    pipeline.cpp
    pool.cpp
    private/conversions.cpp
    private/libbson.cpp
    private/libmongoc.cpp
    read_concern.cpp
    read_preference.cpp
    result/bulk_write.cpp
    result/delete.cpp
    result/gridfs/upload.cpp
    result/insert_many.cpp
    result/insert_one.cpp
    result/replace_one.cpp
    result/update.cpp
    uri.cpp
    validation_criteria.cpp
    write_concern.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

# We define two libraries: the normal library and the testing-only library.  The testing-only
# library does not get installed, but the tests link against it instead of the normal library.  The
# only difference between the two libraries is that MONGOCXX_TESTING is defined in the testing-only
# library (this define enables special testing-only behavior).
add_library(mongocxx
    ${mongocxx_sources}
)
add_library(mongocxx_mocked
    ${mongocxx_sources}
)
target_compile_definitions(mongocxx_mocked PUBLIC MONGOCXX_TESTING)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mongocxx_mocked PRIVATE /bigobj)
endif()

set_target_properties(mongocxx_mocked PROPERTIES
    OUTPUT_NAME mongocxx-mocked
)

if (BUILD_SHARED_LIBS)
    set_target_properties(mongocxx PROPERTIES
        OUTPUT_NAME mongocxx
    )

    foreach (TARGET mongocxx mongocxx_mocked)
        set_target_properties (${TARGET} PROPERTIES
            VERSION ${MONGOCXX_VERSION}
            DEFINE_SYMBOL MONGOCXX_EXPORTS
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            SOVERSION ${MONGOCXX_ABI_VERSION}
        )
    endforeach(TARGET)
else()
    target_compile_definitions(mongocxx PUBLIC MONGOCXX_STATIC)
    target_compile_definitions(mongocxx_mocked PUBLIC MONGOCXX_STATIC)

    set_target_properties(mongocxx PROPERTIES
        OUTPUT_NAME mongocxx-static
    )
endif()

generate_export_header(mongocxx
    BASE_NAME MONGOCXX
    EXPORT_MACRO_NAME MONGOCXX_API
    NO_EXPORT_MACRO_NAME MONGOCXX_PRIVATE
    EXPORT_FILE_NAME config/export.hpp
    STATIC_DEFINE MONGOCXX_STATIC
)
generate_export_header(mongocxx_mocked
    BASE_NAME MONGOCXX
    EXPORT_MACRO_NAME MONGOCXX_API
    NO_EXPORT_MACRO_NAME MONGOCXX_PRIVATE
    EXPORT_FILE_NAME config/export.hpp
    STATIC_DEFINE MONGOCXX_STATIC
)

target_include_directories(mongocxx PRIVATE ${libmongoc_include_directories})
target_include_directories(mongocxx_mocked PRIVATE ${libmongoc_include_directories})

target_compile_definitions(mongocxx PRIVATE ${libmongoc_definitions})
target_compile_definitions(mongocxx_mocked PRIVATE ${libmongoc_definitions})

if (BUILD_SHARED_LIBS)
    target_link_libraries(mongocxx PUBLIC bsoncxx PRIVATE ${libmongoc_libraries})
    target_link_libraries(mongocxx_mocked PUBLIC bsoncxx PRIVATE ${libmongoc_libraries})
else()
    target_link_libraries(mongocxx bsoncxx ${libmongoc_libraries})
    target_link_libraries(mongocxx_mocked bsoncxx ${libmongoc_libraries})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION ${MONGOCXX_HEADER_INSTALL_DIR}
    COMPONENT dev
    FILES_MATCHING
        PATTERN "*.hpp"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config/export.hpp
    DESTINATION ${MONGOCXX_HEADER_INSTALL_DIR}/mongocxx/config
    COMPONENT dev
)

install(TARGETS
    mongocxx
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT dev
)

set(PACKAGE_INCLUDE_INSTALL_DIRS ${MONGOCXX_HEADER_INSTALL_DIR})
set(PACKAGE_LIBRARY_INSTALL_DIRS ${CMAKE_INSTALL_LIBDIR})
set(PACKAGE_LIBRARIES mongocxx)

include(CMakePackageConfigHelpers)

if (BUILD_SHARED_LIBS)
    set(PKG "libmongocxx")
else()
    set(PKG "libmongocxx-static")
endif()

configure_package_config_file(
  cmake/${PKG}-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${MONGOCXX_VERSION}
  PATH_VARS PACKAGE_INCLUDE_INSTALL_DIRS PACKAGE_LIBRARY_INSTALL_DIRS
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
  VERSION ${MONGOCXX_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PKG}-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PKG}-${MONGOCXX_VERSION}
)

add_subdirectory(test)
