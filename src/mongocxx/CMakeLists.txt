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

ParseVersion (${BUILD_VERSION} MONGOCXX)
# TODO: read from file
set(MONGOCXX_ABI_VERSION _noabi)

option(MONGOCXX_ENABLE_SSL "Enable SSL - if the underlying C driver offers it" ON)
option(MONGOCXX_ENABLE_SLOW_TESTS "Run slow tests when invoking the the test target" OFF)

set(MONGOCXX_OUTPUT_BASENAME "mongocxx" CACHE STRING "Output mongocxx library base name")

set(MONGOCXX_VERSION_NO_EXTRA ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH})
set(MONGOCXX_VERSION ${MONGOCXX_VERSION_NO_EXTRA}${MONGOCXX_VERSION_EXTRA})
message ("mongocxx version: ${MONGOCXX_VERSION}")
set(MONGOCXX_INLINE_NAMESPACE "v${MONGOCXX_ABI_VERSION}")
set(MONGOCXX_HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/mongocxx/${MONGOCXX_INLINE_NAMESPACE}" CACHE INTERNAL "")

set(LIBMONGOC_REQUIRED_VERSION 1.17.0)
set(LIBMONGOC_REQUIRED_ABI_VERSION 1.0)

set(mongocxx_pkg_dep "")

# Attempt to find libmongoc by new package name (without lib).
find_package(mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} QUIET)

if(mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION}_FOUND)
  message ("found libmongoc version ${mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION}_VERSION}")
  if(NOT MONGOCXX_LINK_WITH_STATIC_MONGOC)
    set(libmongoc_target mongo::mongoc_shared)
  else()
    set(libmongoc_target mongo::mongoc_static)
  endif()

  if(MONGOCXX_BUILD_STATIC)
    set(mongocxx_pkg_dep "find_dependency(mongoc-1.0 REQUIRED)")
  endif()
else()
  # Require package of old libmongoc name (with lib).
  if(NOT MONGOCXX_LINK_WITH_STATIC_MONGOC)
    find_package(libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} REQUIRED)
    message ("found libmongoc version ${MONGOC_VERSION}")
    set(libmongoc_target ${MONGOC_LIBRARIES})
    set(libmongoc_include_directories ${MONGOC_INCLUDE_DIRS})
    set(libmongoc_definitions ${MONGOC_DEFINITIONS})
    if(MONGOCXX_BUILD_STATIC)
      set(mongocxx_pkg_dep "find_dependency(libmongoc-1.0 REQUIRED)")
    endif()
  else()
    find_package(libmongoc-static-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} REQUIRED)
    message ("found libmongoc version ${MONGOC_STATIC_VERSION}")
    set(libmongoc_target ${MONGOC_STATIC_LIBRARIES})
    set(libmongoc_include_directories ${MONGOC_STATIC_INCLUDE_DIRS})
    set(libmongoc_definitions ${MONGOC_STATIC_DEFINITIONS})
    if(MONGOCXX_BUILD_STATIC)
      set(mongocxx_pkg_dep "find_dependency(libmongoc-static-1.0 REQUIRED)")
    endif()
  endif()
endif()

add_subdirectory(config)

set(mongocxx_sources
    bulk_write.cpp
    client.cpp
    client_encryption.cpp
    client_session.cpp
    change_stream.cpp
    collection.cpp
    cursor.cpp
    database.cpp
    events/command_failed_event.cpp
    events/command_started_event.cpp
    events/command_succeeded_event.cpp
    events/heartbeat_failed_event.cpp
    events/heartbeat_started_event.cpp
    events/heartbeat_succeeded_event.cpp
    events/server_changed_event.cpp
    events/server_closed_event.cpp
    events/server_description.cpp
    events/server_opening_event.cpp
    events/topology_changed_event.cpp
    events/topology_closed_event.cpp
    events/topology_description.cpp
    events/topology_opening_event.cpp
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
    options/auto_encryption.cpp
    options/bulk_write.cpp
    options/change_stream.cpp
    options/client.cpp
    options/client_encryption.cpp
    options/client_session.cpp
    options/count.cpp
    options/estimated_document_count.cpp
    options/create_collection.cpp
    options/data_key.cpp
    options/delete.cpp
    options/distinct.cpp
    options/encrypt.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
    options/find.cpp
    options/gridfs/bucket.cpp
    options/gridfs/upload.cpp
    options/index.cpp
    options/index_view.cpp
    options/insert.cpp
    options/pool.cpp
    options/replace.cpp
    options/tls.cpp
    options/transaction.cpp
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

# We define both the normal libraries and the testing-only library.  The testing-only
# library does not get installed, but the tests link against it instead of the normal library.  The
# only difference between the libraries is that MONGOCXX_TESTING is defined in the testing-only
# library (this define enables special testing-only behavior).

include(MongocxxUtil)

if(MONGOCXX_BUILD_SHARED)
    mongocxx_add_library(mongocxx_shared "${MONGOCXX_OUTPUT_BASENAME}" SHARED)
    target_link_libraries(mongocxx_shared PUBLIC bsoncxx_shared)
endif()

if(MONGOCXX_BUILD_STATIC)
    mongocxx_add_library(mongocxx_static "${MONGOCXX_OUTPUT_BASENAME}-static" STATIC)
    target_link_libraries(mongocxx_static PUBLIC bsoncxx_static)
endif()

if(MONGOCXX_BUILD_SHARED)
    set(MONGOCXX_LIBRARY_FOR_EXAMPLES "mongocxx_shared" CACHE INTERNAL "")
    mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" SHARED)
else()
    set(MONGOCXX_LIBRARY_FOR_EXAMPLES "mongocxx_static" CACHE INTERNAL "")
    mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" STATIC)
endif()
target_link_libraries(mongocxx_mocked PUBLIC bsoncxx_testing)
target_compile_definitions(mongocxx_mocked PUBLIC MONGOCXX_TESTING)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mongocxx_mocked PRIVATE /bigobj)
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

set(PACKAGE_INCLUDE_INSTALL_DIRS ${MONGOCXX_HEADER_INSTALL_DIR})
set(PACKAGE_LIBRARY_INSTALL_DIRS ${CMAKE_INSTALL_LIBDIR})
set(PACKAGE_LIBRARIES mongocxx)

include(CMakePackageConfigHelpers)

set(mongocxx_target_list "")
if(MONGOCXX_BUILD_SHARED)
    mongocxx_install_deprecated_cmake(mongocxx)
    list(APPEND mongocxx_target_list mongocxx_shared)
endif()

if(MONGOCXX_BUILD_STATIC)
    mongocxx_install_deprecated_cmake(mongocxx-static)
    list(APPEND mongocxx_target_list mongocxx_static)
    set(mongocxx_pkg_dep "find_dependency(mongoc-1.0 REQUIRED)")
endif()
mongocxx_install("${mongocxx_target_list}" "${mongocxx_pkg_dep}")

add_subdirectory(test)

set_local_dist (src_mongocxx_DIST_local
   CMakeLists.txt
   bulk_write.cpp
   bulk_write.hpp
   change_stream.cpp
   change_stream.hpp
   client.cpp
   client.hpp
   client_encryption.cpp
   client_encryption.hpp
   client_session.cpp
   client_session.hpp
   cmake/mongocxx-config.cmake.in
   cmake/libmongocxx-config.cmake.in
   cmake/libmongocxx-static-config.cmake.in
   collection.cpp
   collection.hpp
   cursor.cpp
   cursor.hpp
   database.cpp
   database.hpp
   events/command_failed_event.cpp
   events/command_failed_event.hpp
   events/command_started_event.cpp
   events/command_started_event.hpp
   events/command_succeeded_event.cpp
   events/command_succeeded_event.hpp
   events/heartbeat_failed_event.cpp
   events/heartbeat_failed_event.hpp
   events/heartbeat_started_event.cpp
   events/heartbeat_started_event.hpp
   events/heartbeat_succeeded_event.cpp
   events/heartbeat_succeeded_event.hpp
   events/server_changed_event.cpp
   events/server_changed_event.hpp
   events/server_closed_event.cpp
   events/server_closed_event.hpp
   events/server_description.cpp
   events/server_description.hpp
   events/server_opening_event.cpp
   events/server_opening_event.hpp
   events/topology_changed_event.cpp
   events/topology_changed_event.hpp
   events/topology_closed_event.cpp
   events/topology_closed_event.hpp
   events/topology_description.cpp
   events/topology_description.hpp
   events/topology_opening_event.cpp
   events/topology_opening_event.hpp
   exception/authentication_exception.hpp
   exception/bulk_write_exception.hpp
   exception/error_code.cpp
   exception/error_code.hpp
   exception/exception.hpp
   exception/gridfs_exception.hpp
   exception/logic_error.hpp
   exception/operation_exception.cpp
   exception/operation_exception.hpp
   exception/private/mongoc_error.hh
   exception/query_exception.hpp
   exception/server_error_code.cpp
   exception/server_error_code.hpp
   exception/write_exception.hpp
   gridfs/bucket.cpp
   gridfs/bucket.hpp
   gridfs/downloader.cpp
   gridfs/downloader.hpp
   gridfs/private/bucket.hh
   gridfs/private/downloader.hh
   gridfs/private/uploader.hh
   gridfs/uploader.cpp
   gridfs/uploader.hpp
   hint.cpp
   hint.hpp
   index_model.cpp
   index_model.hpp
   index_view.cpp
   index_view.hpp
   instance.cpp
   instance.hpp
   logger.cpp
   logger.hpp
   model/delete_many.cpp
   model/delete_many.hpp
   model/delete_one.cpp
   model/delete_one.hpp
   model/insert_one.cpp
   model/insert_one.hpp
   model/replace_one.cpp
   model/replace_one.hpp
   model/update_many.cpp
   model/update_many.hpp
   model/update_one.cpp
   model/update_one.hpp
   model/write.cpp
   model/write.hpp
   options/aggregate.cpp
   options/aggregate.hpp
   options/apm.cpp
   options/apm.hpp
   options/auto_encryption.cpp
   options/auto_encryption.hpp
   options/bulk_write.cpp
   options/bulk_write.hpp
   options/change_stream.cpp
   options/change_stream.hpp
   options/client.cpp
   options/client.hpp
   options/client_encryption.cpp
   options/client_encryption.hpp
   options/client_session.cpp
   options/client_session.hpp
   options/count.cpp
   options/count.hpp
   options/create_collection.cpp
   options/create_collection.hpp
   options/data_key.cpp
   options/data_key.hpp
   options/delete.cpp
   options/delete.hpp
   options/distinct.cpp
   options/distinct.hpp
   options/encrypt.cpp
   options/encrypt.hpp
   options/estimated_document_count.cpp
   options/estimated_document_count.hpp
   options/find.cpp
   options/find.hpp
   options/find_one_and_delete.cpp
   options/find_one_and_delete.hpp
   options/find_one_and_replace.cpp
   options/find_one_and_replace.hpp
   options/find_one_and_update.cpp
   options/find_one_and_update.hpp
   options/find_one_common_options.hpp
   options/gridfs/bucket.cpp
   options/gridfs/bucket.hpp
   options/gridfs/upload.cpp
   options/gridfs/upload.hpp
   options/index.cpp
   options/index.hpp
   options/index_view.cpp
   options/index_view.hpp
   options/insert.cpp
   options/insert.hpp
   options/pool.cpp
   options/pool.hpp
   options/private/apm.hh
   options/private/ssl.hh
   options/private/transaction.hh
   options/replace.cpp
   options/replace.hpp
   options/ssl.hpp
   options/tls.cpp
   options/tls.hpp
   options/transaction.cpp
   options/transaction.hpp
   options/update.cpp
   options/update.hpp
   pipeline.cpp
   pipeline.hpp
   pool.cpp
   pool.hpp
   private/bulk_write.hh
   private/change_stream.hh
   private/client.hh
   private/client_encryption.hh
   private/client_session.hh
   private/collection.hh
   private/conversions.cpp
   private/conversions.hh
   private/cursor.hh
   private/database.hh
   private/index_view.hh
   private/libbson.cpp
   private/libbson.hh
   private/libmongoc.cpp
   private/libmongoc.hh
   private/libmongoc_symbols.hh
   private/pipeline.hh
   private/pool.hh
   private/read_concern.hh
   private/read_preference.hh
   private/uri.hh
   private/write_concern.hh
   read_concern.cpp
   read_concern.hpp
   read_preference.cpp
   read_preference.hpp
   result/bulk_write.cpp
   result/bulk_write.hpp
   result/delete.cpp
   result/delete.hpp
   result/gridfs/upload.cpp
   result/gridfs/upload.hpp
   result/insert_many.cpp
   result/insert_many.hpp
   result/insert_one.cpp
   result/insert_one.hpp
   result/replace_one.cpp
   result/replace_one.hpp
   result/update.cpp
   result/update.hpp
   stdx.hpp
   test_util/client_helpers.cpp
   test_util/client_helpers.hh
   test_util/export_for_testing.hh
   test_util/mock.hh
   uri.cpp
   uri.hpp
   validation_criteria.cpp
   validation_criteria.hpp
   write_concern.cpp
   write_concern.hpp
   write_type.hpp
)

set (src_mongocxx_DIST
   ${src_mongocxx_DIST_local}
   ${src_mongocxx_config_DIST}
   ${src_mongocxx_test_DIST}
   PARENT_SCOPE
)
