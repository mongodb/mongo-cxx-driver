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

option(MONGOCXX_ENABLE_SSL "Enable SSL - if the underlying C driver offers it" ON)
option(MONGOCXX_ENABLE_SLOW_TESTS "Run slow tests when invoking the the test target" OFF)

set(MONGOCXX_OUTPUT_BASENAME "mongocxx" CACHE STRING "Output mongocxx library base name")

set(MONGOCXX_VERSION_NO_EXTRA ${MONGOCXX_VERSION_MAJOR}.${MONGOCXX_VERSION_MINOR}.${MONGOCXX_VERSION_PATCH})
set(MONGOCXX_VERSION ${MONGOCXX_VERSION_NO_EXTRA}${MONGOCXX_VERSION_EXTRA})
message ("mongocxx version: ${MONGOCXX_VERSION}")

set(mongocxx_pkg_dep "")

if(TARGET mongoc_shared OR TARGET mongoc_static)
  # If these targets exist, then libmongoc has already been included as a project
  # sub-directory
  message ("found libmongoc targets declared in current build scope; version not checked")

  if(NOT MONGOCXX_LINK_WITH_STATIC_MONGOC)
    set(libmongoc_target mongoc_shared)
  else()
    set(libmongoc_target mongoc_static)
  endif()

  set(mongocxx_pkg_dep "find_dependency(mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} REQUIRED)")
else()
  # Attempt to find libmongoc by new package name (without lib).
  find_package(mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} QUIET)

  if(mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION}_FOUND)
    message ("found libmongoc version ${mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION}_VERSION}")
    if(NOT MONGOCXX_LINK_WITH_STATIC_MONGOC)
      set(libmongoc_target mongo::mongoc_shared)
    else()
      set(libmongoc_target mongo::mongoc_static)
    endif()
    set(mongocxx_pkg_dep "find_dependency(mongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} REQUIRED)")
  else()
    # Require package of old libmongoc name (with lib).
    if(NOT MONGOCXX_LINK_WITH_STATIC_MONGOC)
      find_package(libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} REQUIRED)
      message ("found libmongoc version ${MONGOC_VERSION}")
      set(libmongoc_target ${MONGOC_LIBRARIES})
      set(libmongoc_definitions ${MONGOC_DEFINITIONS})
      set(mongocxx_pkg_dep "find_dependency(libmongoc-${LIBMONGOC_REQUIRED_ABI_VERSION} REQUIRED)")
    else()
      find_package(libmongoc-static-${LIBMONGOC_REQUIRED_ABI_VERSION} ${LIBMONGOC_REQUIRED_VERSION} REQUIRED)
      message ("found libmongoc version ${MONGOC_STATIC_VERSION}")
      set(libmongoc_target ${MONGOC_STATIC_LIBRARIES})
      set(libmongoc_definitions ${MONGOC_STATIC_DEFINITIONS})
      set(mongocxx_pkg_dep "find_dependency(libmongoc-static-${LIBMONGOC_REQUIRED_ABI_VERSION} REQUIRED)")
    endif()
  endif()
endif()

add_subdirectory(lib/mongocxx/v_noabi/mongocxx/config)

set(mongocxx_sources
    lib/mongocxx/v_noabi/mongocxx/bulk_write.cpp
    lib/mongocxx/v_noabi/mongocxx/client.cpp
    lib/mongocxx/v_noabi/mongocxx/client_encryption.cpp
    lib/mongocxx/v_noabi/mongocxx/client_session.cpp
    lib/mongocxx/v_noabi/mongocxx/change_stream.cpp
    lib/mongocxx/v_noabi/mongocxx/collection.cpp
    lib/mongocxx/v_noabi/mongocxx/cursor.cpp
    lib/mongocxx/v_noabi/mongocxx/database.cpp
    lib/mongocxx/v_noabi/mongocxx/events/command_failed_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/command_started_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/command_succeeded_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/heartbeat_failed_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/heartbeat_started_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/heartbeat_succeeded_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/server_changed_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/server_closed_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/server_description.cpp
    lib/mongocxx/v_noabi/mongocxx/events/server_opening_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/topology_changed_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/topology_closed_event.cpp
    lib/mongocxx/v_noabi/mongocxx/events/topology_description.cpp
    lib/mongocxx/v_noabi/mongocxx/events/topology_opening_event.cpp
    lib/mongocxx/v_noabi/mongocxx/exception/error_code.cpp
    lib/mongocxx/v_noabi/mongocxx/exception/operation_exception.cpp
    lib/mongocxx/v_noabi/mongocxx/exception/server_error_code.cpp
    lib/mongocxx/v_noabi/mongocxx/gridfs/bucket.cpp
    lib/mongocxx/v_noabi/mongocxx/gridfs/downloader.cpp
    lib/mongocxx/v_noabi/mongocxx/gridfs/uploader.cpp
    lib/mongocxx/v_noabi/mongocxx/hint.cpp
    lib/mongocxx/v_noabi/mongocxx/index_model.cpp
    lib/mongocxx/v_noabi/mongocxx/index_view.cpp
    lib/mongocxx/v_noabi/mongocxx/instance.cpp
    lib/mongocxx/v_noabi/mongocxx/logger.cpp
    lib/mongocxx/v_noabi/mongocxx/model/delete_many.cpp
    lib/mongocxx/v_noabi/mongocxx/model/delete_one.cpp
    lib/mongocxx/v_noabi/mongocxx/model/insert_one.cpp
    lib/mongocxx/v_noabi/mongocxx/model/replace_one.cpp
    lib/mongocxx/v_noabi/mongocxx/model/update_many.cpp
    lib/mongocxx/v_noabi/mongocxx/model/update_one.cpp
    lib/mongocxx/v_noabi/mongocxx/model/write.cpp
    lib/mongocxx/v_noabi/mongocxx/options/aggregate.cpp
    lib/mongocxx/v_noabi/mongocxx/options/apm.cpp
    lib/mongocxx/v_noabi/mongocxx/options/auto_encryption.cpp
    lib/mongocxx/v_noabi/mongocxx/options/bulk_write.cpp
    lib/mongocxx/v_noabi/mongocxx/options/change_stream.cpp
    lib/mongocxx/v_noabi/mongocxx/options/client.cpp
    lib/mongocxx/v_noabi/mongocxx/options/client_encryption.cpp
    lib/mongocxx/v_noabi/mongocxx/options/client_session.cpp
    lib/mongocxx/v_noabi/mongocxx/options/count.cpp
    lib/mongocxx/v_noabi/mongocxx/options/estimated_document_count.cpp
    lib/mongocxx/v_noabi/mongocxx/options/create_collection.cpp
    lib/mongocxx/v_noabi/mongocxx/options/data_key.cpp
    lib/mongocxx/v_noabi/mongocxx/options/delete.cpp
    lib/mongocxx/v_noabi/mongocxx/options/distinct.cpp
    lib/mongocxx/v_noabi/mongocxx/options/encrypt.cpp
    lib/mongocxx/v_noabi/mongocxx/options/find_one_and_delete.cpp
    lib/mongocxx/v_noabi/mongocxx/options/find_one_and_replace.cpp
    lib/mongocxx/v_noabi/mongocxx/options/find_one_and_update.cpp
    lib/mongocxx/v_noabi/mongocxx/options/find.cpp
    lib/mongocxx/v_noabi/mongocxx/options/gridfs/bucket.cpp
    lib/mongocxx/v_noabi/mongocxx/options/gridfs/upload.cpp
    lib/mongocxx/v_noabi/mongocxx/options/index.cpp
    lib/mongocxx/v_noabi/mongocxx/options/index_view.cpp
    lib/mongocxx/v_noabi/mongocxx/options/insert.cpp
    lib/mongocxx/v_noabi/mongocxx/options/pool.cpp
    lib/mongocxx/v_noabi/mongocxx/options/range.cpp
    lib/mongocxx/v_noabi/mongocxx/options/replace.cpp
    lib/mongocxx/v_noabi/mongocxx/options/rewrap_many_datakey.cpp
    lib/mongocxx/v_noabi/mongocxx/options/server_api.cpp
    lib/mongocxx/v_noabi/mongocxx/options/tls.cpp
    lib/mongocxx/v_noabi/mongocxx/options/transaction.cpp
    lib/mongocxx/v_noabi/mongocxx/options/update.cpp
    lib/mongocxx/v_noabi/mongocxx/pipeline.cpp
    lib/mongocxx/v_noabi/mongocxx/pool.cpp
    lib/mongocxx/v_noabi/mongocxx/private/conversions.cpp
    lib/mongocxx/v_noabi/mongocxx/private/libbson.cpp
    lib/mongocxx/v_noabi/mongocxx/private/libmongoc.cpp
    lib/mongocxx/v_noabi/mongocxx/private/numeric_casting.cpp
    lib/mongocxx/v_noabi/mongocxx/read_concern.cpp
    lib/mongocxx/v_noabi/mongocxx/read_preference.cpp
    lib/mongocxx/v_noabi/mongocxx/result/bulk_write.cpp
    lib/mongocxx/v_noabi/mongocxx/result/delete.cpp
    lib/mongocxx/v_noabi/mongocxx/result/gridfs/upload.cpp
    lib/mongocxx/v_noabi/mongocxx/result/insert_many.cpp
    lib/mongocxx/v_noabi/mongocxx/result/insert_one.cpp
    lib/mongocxx/v_noabi/mongocxx/result/replace_one.cpp
    lib/mongocxx/v_noabi/mongocxx/result/rewrap_many_datakey.cpp
    lib/mongocxx/v_noabi/mongocxx/result/update.cpp
    lib/mongocxx/v_noabi/mongocxx/search_index_model.cpp
    lib/mongocxx/v_noabi/mongocxx/search_index_view.cpp
    lib/mongocxx/v_noabi/mongocxx/uri.cpp
    lib/mongocxx/v_noabi/mongocxx/validation_criteria.cpp
    lib/mongocxx/v_noabi/mongocxx/write_concern.cpp
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
    if(ENABLE_TESTS)
      mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" SHARED)
    endif()
else()
    set(MONGOCXX_LIBRARY_FOR_EXAMPLES "mongocxx_static" CACHE INTERNAL "")
    if(ENABLE_TESTS)
      mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" STATIC)
    endif()
endif()
if(ENABLE_TESTS)
  target_link_libraries(mongocxx_mocked PUBLIC bsoncxx_testing)
  target_compile_definitions(mongocxx_mocked PUBLIC MONGOCXX_TESTING)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mongocxx_mocked PRIVATE /bigobj)
  endif()
endif()


install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
)

install(FILES
    ${PROJECT_BINARY_DIR}/lib/mongocxx/v_noabi/mongocxx/config/export.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mongocxx/v_noabi/mongocxx/config
    COMPONENT dev
)

set(PACKAGE_INCLUDE_INSTALL_DIRS ${CMAKE_INSTALL_INCLUDEDIR}/mongocxx/v_noabi)
set(PACKAGE_LIBRARY_INSTALL_DIRS ${CMAKE_INSTALL_LIBDIR})
set(PACKAGE_LIBRARIES mongocxx)

include(CMakePackageConfigHelpers)

set (libdir "\${prefix}/${CMAKE_INSTALL_LIBDIR}")

if(MONGOCXX_BUILD_SHARED)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/libmongocxx.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/libmongocxx.pc
        @ONLY
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/libmongocxx.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT dev
    )
endif()

if(MONGOCXX_BUILD_STATIC)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/libmongocxx-static.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/libmongocxx-static.pc
        @ONLY
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/libmongocxx-static.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT dev
    )
endif()

set(mongocxx_target_list "")
if(MONGOCXX_BUILD_SHARED)
    mongocxx_install_deprecated_cmake(mongocxx)
    list(APPEND mongocxx_target_list mongocxx_shared)
endif()

if(MONGOCXX_BUILD_STATIC)
    mongocxx_install_deprecated_cmake(mongocxx-static)
    list(APPEND mongocxx_target_list mongocxx_static)
endif()
mongocxx_install("${mongocxx_target_list}" "${mongocxx_pkg_dep}")

if(ENABLE_TESTS)
  add_subdirectory(test)
endif()

set_local_dist (src_mongocxx_lib_v_noabi_DIST_local
   CMakeLists.txt
   lib/mongocxx/v_noabi/mongocxx/bulk_write.cpp
   lib/mongocxx/v_noabi/mongocxx/change_stream.cpp
   lib/mongocxx/v_noabi/mongocxx/client.cpp
   lib/mongocxx/v_noabi/mongocxx/client_encryption.cpp
   lib/mongocxx/v_noabi/mongocxx/client_session.cpp
   cmake/mongocxx-config.cmake.in
   cmake/libmongocxx-config.cmake.in
   cmake/libmongocxx-static-config.cmake.in
   cmake/libmongocxx-static.pc.in
   cmake/libmongocxx.pc.in
   lib/mongocxx/v_noabi/mongocxx/collection.cpp
   lib/mongocxx/v_noabi/mongocxx/cursor.cpp
   lib/mongocxx/v_noabi/mongocxx/database.cpp
   lib/mongocxx/v_noabi/mongocxx/events/command_failed_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/command_started_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/command_succeeded_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/heartbeat_failed_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/heartbeat_started_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/heartbeat_succeeded_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/server_changed_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/server_closed_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/server_description.cpp
   lib/mongocxx/v_noabi/mongocxx/events/server_opening_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/topology_changed_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/topology_closed_event.cpp
   lib/mongocxx/v_noabi/mongocxx/events/topology_description.cpp
   lib/mongocxx/v_noabi/mongocxx/events/topology_opening_event.cpp
   lib/mongocxx/v_noabi/mongocxx/exception/error_code.cpp
   lib/mongocxx/v_noabi/mongocxx/exception/operation_exception.cpp
   lib/mongocxx/v_noabi/mongocxx/exception/private/mongoc_error.hh
   lib/mongocxx/v_noabi/mongocxx/exception/server_error_code.cpp
   lib/mongocxx/v_noabi/mongocxx/gridfs/bucket.cpp
   lib/mongocxx/v_noabi/mongocxx/gridfs/downloader.cpp
   lib/mongocxx/v_noabi/mongocxx/gridfs/private/bucket.hh
   lib/mongocxx/v_noabi/mongocxx/gridfs/private/downloader.hh
   lib/mongocxx/v_noabi/mongocxx/gridfs/private/uploader.hh
   lib/mongocxx/v_noabi/mongocxx/gridfs/uploader.cpp
   lib/mongocxx/v_noabi/mongocxx/hint.cpp
   lib/mongocxx/v_noabi/mongocxx/index_model.cpp
   lib/mongocxx/v_noabi/mongocxx/index_view.cpp
   lib/mongocxx/v_noabi/mongocxx/instance.cpp
   lib/mongocxx/v_noabi/mongocxx/logger.cpp
   lib/mongocxx/v_noabi/mongocxx/model/delete_many.cpp
   lib/mongocxx/v_noabi/mongocxx/model/delete_one.cpp
   lib/mongocxx/v_noabi/mongocxx/model/insert_one.cpp
   lib/mongocxx/v_noabi/mongocxx/model/replace_one.cpp
   lib/mongocxx/v_noabi/mongocxx/model/update_many.cpp
   lib/mongocxx/v_noabi/mongocxx/model/update_one.cpp
   lib/mongocxx/v_noabi/mongocxx/model/write.cpp
   lib/mongocxx/v_noabi/mongocxx/options/aggregate.cpp
   lib/mongocxx/v_noabi/mongocxx/options/apm.cpp
   lib/mongocxx/v_noabi/mongocxx/options/auto_encryption.cpp
   lib/mongocxx/v_noabi/mongocxx/options/bulk_write.cpp
   lib/mongocxx/v_noabi/mongocxx/options/change_stream.cpp
   lib/mongocxx/v_noabi/mongocxx/options/client.cpp
   lib/mongocxx/v_noabi/mongocxx/options/client_encryption.cpp
   lib/mongocxx/v_noabi/mongocxx/options/client_session.cpp
   lib/mongocxx/v_noabi/mongocxx/options/count.cpp
   lib/mongocxx/v_noabi/mongocxx/options/create_collection.cpp
   lib/mongocxx/v_noabi/mongocxx/options/data_key.cpp
   lib/mongocxx/v_noabi/mongocxx/options/delete.cpp
   lib/mongocxx/v_noabi/mongocxx/options/distinct.cpp
   lib/mongocxx/v_noabi/mongocxx/options/encrypt.cpp
   lib/mongocxx/v_noabi/mongocxx/options/estimated_document_count.cpp
   lib/mongocxx/v_noabi/mongocxx/options/find.cpp
   lib/mongocxx/v_noabi/mongocxx/options/find_one_and_delete.cpp
   lib/mongocxx/v_noabi/mongocxx/options/find_one_and_replace.cpp
   lib/mongocxx/v_noabi/mongocxx/options/find_one_and_update.cpp
   lib/mongocxx/v_noabi/mongocxx/options/gridfs/bucket.cpp
   lib/mongocxx/v_noabi/mongocxx/options/gridfs/upload.cpp
   lib/mongocxx/v_noabi/mongocxx/options/index.cpp
   lib/mongocxx/v_noabi/mongocxx/options/index_view.cpp
   lib/mongocxx/v_noabi/mongocxx/options/insert.cpp
   lib/mongocxx/v_noabi/mongocxx/options/pool.cpp
   lib/mongocxx/v_noabi/mongocxx/options/private/apm.hh
   lib/mongocxx/v_noabi/mongocxx/options/private/server_api.hh
   lib/mongocxx/v_noabi/mongocxx/options/private/ssl.hh
   lib/mongocxx/v_noabi/mongocxx/options/private/transaction.hh
   lib/mongocxx/v_noabi/mongocxx/options/range.cpp
   lib/mongocxx/v_noabi/mongocxx/options/replace.cpp
   lib/mongocxx/v_noabi/mongocxx/options/rewrap_many_datakey.cpp
   lib/mongocxx/v_noabi/mongocxx/options/server_api.cpp
   lib/mongocxx/v_noabi/mongocxx/options/tls.cpp
   lib/mongocxx/v_noabi/mongocxx/options/transaction.cpp
   lib/mongocxx/v_noabi/mongocxx/options/update.cpp
   lib/mongocxx/v_noabi/mongocxx/pipeline.cpp
   lib/mongocxx/v_noabi/mongocxx/pool.cpp
   lib/mongocxx/v_noabi/mongocxx/private/append_aggregate_options.hh
   lib/mongocxx/v_noabi/mongocxx/private/bulk_write.hh
   lib/mongocxx/v_noabi/mongocxx/private/change_stream.hh
   lib/mongocxx/v_noabi/mongocxx/private/client.hh
   lib/mongocxx/v_noabi/mongocxx/private/client_encryption.hh
   lib/mongocxx/v_noabi/mongocxx/private/client_session.hh
   lib/mongocxx/v_noabi/mongocxx/private/collection.hh
   lib/mongocxx/v_noabi/mongocxx/private/conversions.cpp
   lib/mongocxx/v_noabi/mongocxx/private/conversions.hh
   lib/mongocxx/v_noabi/mongocxx/private/cursor.hh
   lib/mongocxx/v_noabi/mongocxx/private/database.hh
   lib/mongocxx/v_noabi/mongocxx/private/index_view.hh
   lib/mongocxx/v_noabi/mongocxx/private/libbson.cpp
   lib/mongocxx/v_noabi/mongocxx/private/libbson.hh
   lib/mongocxx/v_noabi/mongocxx/private/libmongoc.cpp
   lib/mongocxx/v_noabi/mongocxx/private/libmongoc.hh
   lib/mongocxx/v_noabi/mongocxx/private/libmongoc_symbols.hh
   lib/mongocxx/v_noabi/mongocxx/private/numeric_casting.cpp
   lib/mongocxx/v_noabi/mongocxx/private/numeric_casting.hh
   lib/mongocxx/v_noabi/mongocxx/private/pipeline.hh
   lib/mongocxx/v_noabi/mongocxx/private/pool.hh
   lib/mongocxx/v_noabi/mongocxx/private/read_concern.hh
   lib/mongocxx/v_noabi/mongocxx/private/read_preference.hh
   lib/mongocxx/v_noabi/mongocxx/private/search_index_model.hh
   lib/mongocxx/v_noabi/mongocxx/private/search_index_view.hh
   lib/mongocxx/v_noabi/mongocxx/private/uri.hh
   lib/mongocxx/v_noabi/mongocxx/private/write_concern.hh
   lib/mongocxx/v_noabi/mongocxx/read_concern.cpp
   lib/mongocxx/v_noabi/mongocxx/read_preference.cpp
   lib/mongocxx/v_noabi/mongocxx/result/bulk_write.cpp
   lib/mongocxx/v_noabi/mongocxx/result/delete.cpp
   lib/mongocxx/v_noabi/mongocxx/result/gridfs/upload.cpp
   lib/mongocxx/v_noabi/mongocxx/result/insert_many.cpp
   lib/mongocxx/v_noabi/mongocxx/result/insert_one.cpp
   lib/mongocxx/v_noabi/mongocxx/result/replace_one.cpp
   lib/mongocxx/v_noabi/mongocxx/result/rewrap_many_datakey.cpp
   lib/mongocxx/v_noabi/mongocxx/result/update.cpp
   lib/mongocxx/v_noabi/mongocxx/search_index_model.cpp
   lib/mongocxx/v_noabi/mongocxx/search_index_view.cpp
   lib/mongocxx/v_noabi/mongocxx/test_util/client_helpers.cpp
   lib/mongocxx/v_noabi/mongocxx/test_util/client_helpers.hh
   lib/mongocxx/v_noabi/mongocxx/test_util/export_for_testing.hh
   lib/mongocxx/v_noabi/mongocxx/test_util/mock.hh
   lib/mongocxx/v_noabi/mongocxx/uri.cpp
   lib/mongocxx/v_noabi/mongocxx/validation_criteria.cpp
   lib/mongocxx/v_noabi/mongocxx/write_concern.cpp
)

set_local_dist (src_mongocxx_include_v_noabi_DIST_local
    include/mongocxx/v_noabi/mongocxx/bulk_write.hpp
    include/mongocxx/v_noabi/mongocxx/change_stream.hpp
    include/mongocxx/v_noabi/mongocxx/client_encryption.hpp
    include/mongocxx/v_noabi/mongocxx/client_session.hpp
    include/mongocxx/v_noabi/mongocxx/client.hpp
    include/mongocxx/v_noabi/mongocxx/collection.hpp
    include/mongocxx/v_noabi/mongocxx/config/compiler.hpp
    include/mongocxx/v_noabi/mongocxx/config/postlude.hpp
    include/mongocxx/v_noabi/mongocxx/config/prelude.hpp
    include/mongocxx/v_noabi/mongocxx/cursor.hpp
    include/mongocxx/v_noabi/mongocxx/database.hpp
    include/mongocxx/v_noabi/mongocxx/events/command_failed_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/command_started_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/command_succeeded_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/heartbeat_failed_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/heartbeat_started_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/heartbeat_succeeded_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/server_changed_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/server_closed_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/server_description.hpp
    include/mongocxx/v_noabi/mongocxx/events/server_opening_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/topology_changed_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/topology_closed_event.hpp
    include/mongocxx/v_noabi/mongocxx/events/topology_description.hpp
    include/mongocxx/v_noabi/mongocxx/events/topology_opening_event.hpp
    include/mongocxx/v_noabi/mongocxx/exception/authentication_exception.hpp
    include/mongocxx/v_noabi/mongocxx/exception/bulk_write_exception.hpp
    include/mongocxx/v_noabi/mongocxx/exception/error_code.hpp
    include/mongocxx/v_noabi/mongocxx/exception/exception.hpp
    include/mongocxx/v_noabi/mongocxx/exception/gridfs_exception.hpp
    include/mongocxx/v_noabi/mongocxx/exception/logic_error.hpp
    include/mongocxx/v_noabi/mongocxx/exception/operation_exception.hpp
    include/mongocxx/v_noabi/mongocxx/exception/query_exception.hpp
    include/mongocxx/v_noabi/mongocxx/exception/server_error_code.hpp
    include/mongocxx/v_noabi/mongocxx/exception/write_exception.hpp
    include/mongocxx/v_noabi/mongocxx/gridfs/bucket.hpp
    include/mongocxx/v_noabi/mongocxx/gridfs/downloader.hpp
    include/mongocxx/v_noabi/mongocxx/gridfs/uploader.hpp
    include/mongocxx/v_noabi/mongocxx/hint.hpp
    include/mongocxx/v_noabi/mongocxx/index_model.hpp
    include/mongocxx/v_noabi/mongocxx/index_view.hpp
    include/mongocxx/v_noabi/mongocxx/instance.hpp
    include/mongocxx/v_noabi/mongocxx/logger.hpp
    include/mongocxx/v_noabi/mongocxx/model/delete_many.hpp
    include/mongocxx/v_noabi/mongocxx/model/delete_one.hpp
    include/mongocxx/v_noabi/mongocxx/model/insert_one.hpp
    include/mongocxx/v_noabi/mongocxx/model/replace_one.hpp
    include/mongocxx/v_noabi/mongocxx/model/update_many.hpp
    include/mongocxx/v_noabi/mongocxx/model/update_one.hpp
    include/mongocxx/v_noabi/mongocxx/model/write.hpp
    include/mongocxx/v_noabi/mongocxx/options/aggregate.hpp
    include/mongocxx/v_noabi/mongocxx/options/apm.hpp
    include/mongocxx/v_noabi/mongocxx/options/auto_encryption.hpp
    include/mongocxx/v_noabi/mongocxx/options/bulk_write.hpp
    include/mongocxx/v_noabi/mongocxx/options/change_stream.hpp
    include/mongocxx/v_noabi/mongocxx/options/client_encryption.hpp
    include/mongocxx/v_noabi/mongocxx/options/client_session.hpp
    include/mongocxx/v_noabi/mongocxx/options/client.hpp
    include/mongocxx/v_noabi/mongocxx/options/count.hpp
    include/mongocxx/v_noabi/mongocxx/options/create_collection.hpp
    include/mongocxx/v_noabi/mongocxx/options/data_key.hpp
    include/mongocxx/v_noabi/mongocxx/options/delete.hpp
    include/mongocxx/v_noabi/mongocxx/options/distinct.hpp
    include/mongocxx/v_noabi/mongocxx/options/encrypt.hpp
    include/mongocxx/v_noabi/mongocxx/options/estimated_document_count.hpp
    include/mongocxx/v_noabi/mongocxx/options/find_one_and_delete.hpp
    include/mongocxx/v_noabi/mongocxx/options/find_one_and_replace.hpp
    include/mongocxx/v_noabi/mongocxx/options/find_one_and_update.hpp
    include/mongocxx/v_noabi/mongocxx/options/find_one_common_options.hpp
    include/mongocxx/v_noabi/mongocxx/options/find.hpp
    include/mongocxx/v_noabi/mongocxx/options/gridfs/bucket.hpp
    include/mongocxx/v_noabi/mongocxx/options/gridfs/upload.hpp
    include/mongocxx/v_noabi/mongocxx/options/index_view.hpp
    include/mongocxx/v_noabi/mongocxx/options/index.hpp
    include/mongocxx/v_noabi/mongocxx/options/insert.hpp
    include/mongocxx/v_noabi/mongocxx/options/pool.hpp
    include/mongocxx/v_noabi/mongocxx/options/range.hpp
    include/mongocxx/v_noabi/mongocxx/options/replace.hpp
    include/mongocxx/v_noabi/mongocxx/options/rewrap_many_datakey.hpp
    include/mongocxx/v_noabi/mongocxx/options/server_api.hpp
    include/mongocxx/v_noabi/mongocxx/options/ssl.hpp
    include/mongocxx/v_noabi/mongocxx/options/tls.hpp
    include/mongocxx/v_noabi/mongocxx/options/transaction.hpp
    include/mongocxx/v_noabi/mongocxx/options/update.hpp
    include/mongocxx/v_noabi/mongocxx/pipeline.hpp
    include/mongocxx/v_noabi/mongocxx/pool.hpp
    include/mongocxx/v_noabi/mongocxx/read_concern.hpp
    include/mongocxx/v_noabi/mongocxx/read_preference.hpp
    include/mongocxx/v_noabi/mongocxx/result/bulk_write.hpp
    include/mongocxx/v_noabi/mongocxx/result/delete.hpp
    include/mongocxx/v_noabi/mongocxx/result/gridfs/upload.hpp
    include/mongocxx/v_noabi/mongocxx/result/insert_many.hpp
    include/mongocxx/v_noabi/mongocxx/result/insert_one.hpp
    include/mongocxx/v_noabi/mongocxx/result/replace_one.hpp
    include/mongocxx/v_noabi/mongocxx/result/rewrap_many_datakey.hpp
    include/mongocxx/v_noabi/mongocxx/result/update.hpp
    include/mongocxx/v_noabi/mongocxx/search_index_model.hpp
    include/mongocxx/v_noabi/mongocxx/search_index_view.hpp
    include/mongocxx/v_noabi/mongocxx/stdx.hpp
    include/mongocxx/v_noabi/mongocxx/uri.hpp
    include/mongocxx/v_noabi/mongocxx/validation_criteria.hpp
    include/mongocxx/v_noabi/mongocxx/write_concern.hpp
    include/mongocxx/v_noabi/mongocxx/write_type.hpp
)

set (src_mongocxx_DIST
   ${src_mongocxx_lib_v_noabi_DIST_local}
   ${src_mongocxx_include_v_noabi_DIST_local}
   ${src_mongocxx_config_DIST}
   ${src_mongocxx_test_DIST}
   PARENT_SCOPE
)
