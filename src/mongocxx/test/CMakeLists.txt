# Copyright 2009-present MongoDB, Inc.
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

include(FetchCatch2)

if(NOT BUILD_TESTING)
    set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()

if(MONGOCXX_BUILD_SHARED)
    mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" SHARED)
else()
    mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" STATIC)
endif()

target_link_libraries(mongocxx_mocked PUBLIC bsoncxx_testing)
target_compile_definitions(mongocxx_mocked PUBLIC MONGOCXX_TESTING)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mongocxx_mocked PRIVATE /bigobj)
endif()

add_library(client_helpers OBJECT client_helpers.cpp)

add_library(spec_test_common OBJECT
    spec/monitoring.cpp
    spec/operation.cpp
    spec/unified_tests/assert.cpp
    spec/unified_tests/entity.cpp
    spec/util.cpp
)

add_executable(test_driver
    bulk_write.cpp
    catch_helpers.cpp
    change_streams.cpp
    client_session.cpp
    client_side_encryption.cpp
    client.cpp
    collection_mocked.cpp
    collection.cpp
    conversions.cpp
    database.cpp
    gridfs/bucket.cpp
    gridfs/downloader.cpp
    gridfs/uploader.cpp
    hint.cpp
    index_view.cpp
    model/delete_many.cpp
    model/delete_one.cpp
    model/insert_one.cpp
    model/replace_one.cpp
    model/update_many.cpp
    model/update_one.cpp
    options/aggregate.cpp
    options/bulk_write.cpp
    options/client_session.cpp
    options/count.cpp
    options/delete.cpp
    options/distinct.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
    options/find.cpp
    options/gridfs/bucket.cpp
    options/gridfs/upload.cpp
    options/index.cpp
    options/insert.cpp
    options/pool.cpp
    options/replace.cpp
    options/update.cpp
    pool.cpp
    private/numeric_casting.cpp
    private/scoped_bson_t.cpp
    private/write_concern.cpp
    read_concern.cpp
    read_preference.cpp
    result/bulk_write.cpp
    result/delete.cpp
    result/gridfs/upload.cpp
    result/insert_one.cpp
    result/replace_one.cpp
    result/update.cpp
    sdam-monitoring.cpp
    search_index_view.cpp
    spec/initial_dns_seedlist_discovery.cpp
    spec/monitoring.cpp
    spec/monitoring.hh
    spec/unified_tests/assert.cpp
    spec/unified_tests/entity.cpp
    spec/uri_options.cpp
    transactions.cpp
    uri.cpp
    validation_criteria.cpp
    version.cpp
    write_concern.cpp
)
target_link_libraries(test_driver PRIVATE client_helpers)

add_executable(test_logging logging.cpp)

add_executable(test_instance instance.cpp)

add_executable(test_crud_specs spec/crud.cpp)
target_link_libraries(test_crud_specs PRIVATE spec_test_common client_helpers)

add_executable(test_gridfs_specs spec/gridfs.cpp)
target_link_libraries(test_gridfs_specs PRIVATE spec_test_common client_helpers)

add_executable(test_client_side_encryption_specs spec/client_side_encryption.cpp)
target_link_libraries(test_client_side_encryption_specs PRIVATE spec_test_common client_helpers)

add_executable(test_command_monitoring_specs spec/command_monitoring.cpp)
target_link_libraries(test_command_monitoring_specs PRIVATE spec_test_common client_helpers)

add_executable(test_transactions_specs spec/transactions.cpp)
target_link_libraries(test_transactions_specs PRIVATE spec_test_common client_helpers)

add_executable(test_retryable_reads_specs spec/retryable-reads.cpp)
target_link_libraries(test_retryable_reads_specs PRIVATE spec_test_common client_helpers)

add_executable(test_read_write_concern_specs spec/read_write_concern.cpp)
target_link_libraries(test_read_write_concern_specs PRIVATE spec_test_common client_helpers)

add_executable(test_mongohouse_specs spec/mongohouse.cpp)
target_link_libraries(test_mongohouse_specs PRIVATE spec_test_common client_helpers)

add_executable(test_unified_format_specs spec/unified_tests/operations.cpp spec/unified_tests/runner.cpp)
target_link_libraries(test_unified_format_specs PRIVATE spec_test_common client_helpers)

add_executable(test_versioned_api versioned_api.cpp)
target_link_libraries(test_versioned_api PRIVATE spec_test_common client_helpers)

# Common target properties for test executables.
add_library(mongocxx_test_properties INTERFACE)

target_link_libraries(mongocxx_test_properties INTERFACE Catch2::Catch2)
target_compile_features(mongocxx_test_properties INTERFACE cxx_std_14)

target_link_libraries(mongocxx_test_properties INTERFACE mongocxx_mocked ${mongoc_target})
target_include_directories(mongocxx_test_properties INTERFACE
    # Allow `#include <mongocxx/test/...>`.
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
# Use `__vectorcall` by default with MSVC to catch missing `__cdecl`.
target_compile_options(mongocxx_test_properties INTERFACE "$<$<CXX_COMPILER_ID:MSVC>:/Gv>")
add_library(mongocxx::test_properties ALIAS mongocxx_test_properties)

add_library(mongocxx_test_properties_with_main INTERFACE)
target_link_libraries(mongocxx_test_properties_with_main INTERFACE mongocxx::test_properties)
target_sources(mongocxx_test_properties_with_main INTERFACE catch.cpp)
add_library(mongocxx::test_properties_with_main ALIAS mongocxx_test_properties_with_main)

set_property(
    TARGET
        client_helpers
        spec_test_common
    APPEND PROPERTY LINK_LIBRARIES
        mongocxx::test_properties
)

set_property(
    TARGET
        test_client_side_encryption_specs
        test_command_monitoring_specs
        test_crud_specs
        test_driver
        test_gridfs_specs
        test_instance
        test_logging
        test_mongohouse_specs
        test_read_write_concern_specs
        test_retryable_reads_specs
        test_transactions_specs
        test_unified_format_specs
        test_versioned_api
    APPEND PROPERTY LINK_LIBRARIES
        mongocxx::test_properties_with_main
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_driver PRIVATE /bigobj)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_driver PRIVATE -Wno-maybe-uninitialized)
    target_compile_options(test_retryable_reads_specs PRIVATE -Wno-maybe-uninitialized)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(test_driver PRIVATE Threads::Threads)

foreach(test_name
    client_side_encryption_specs
    command_monitoring_specs
    crud_specs
    driver
    gridfs_specs
    instance
    logging
    read_write_concern_specs
    retryable_reads_specs
    transactions_specs
    unified_format_specs
    versioned_api
)
    add_test(NAME ${test_name} COMMAND test_${test_name} --reporter compact --allow-running-no-tests)
endforeach()

# Adding this as a test will run it as part of the RUN_TESTS command in MSVC.
# Do not add, since we only test mongohouse on Linux.
if(0)
    add_test(NAME mongohouse_specs COMMAND test_mongohouse_specs ${test_args})
endif()

set_property(TEST crud_specs APPEND PROPERTY ENVIRONMENT
    "CRUD_LEGACY_TESTS_PATH=${DATA_SOURCE_DIR}/crud/legacy"
)

set_property(TEST gridfs_specs APPEND PROPERTY ENVIRONMENT
    "GRIDFS_TESTS_PATH=${DATA_SOURCE_DIR}/gridfs"
)

set_property(TEST client_side_encryption_specs APPEND PROPERTY ENVIRONMENT
    "CLIENT_SIDE_ENCRYPTION_LEGACY_TESTS_PATH=${DATA_SOURCE_DIR}/client_side_encryption/legacy"
)

set_property(TEST driver APPEND PROPERTY ENVIRONMENT
    "CLIENT_SIDE_ENCRYPTION_TESTS_PATH=${DATA_SOURCE_DIR}/client_side_encryption"
    "URI_OPTIONS_TESTS_PATH=${DATA_SOURCE_DIR}/uri-options"
)

set_property(TEST command_monitoring_specs APPEND PROPERTY ENVIRONMENT
    "COMMAND_MONITORING_TESTS_PATH=${DATA_SOURCE_DIR}/command-monitoring"
)

set_property(TEST transactions_specs APPEND PROPERTY ENVIRONMENT
    "TRANSACTIONS_LEGACY_TESTS_PATH=${DATA_SOURCE_DIR}/transactions/legacy"
    "WITH_TRANSACTION_TESTS_PATH=${DATA_SOURCE_DIR}/with_transaction"
)

set_property(TEST retryable_reads_specs APPEND PROPERTY ENVIRONMENT
    "RETRYABLE_READS_LEGACY_TESTS_PATH=${DATA_SOURCE_DIR}/retryable-reads/legacy"
)

set_property(TEST read_write_concern_specs APPEND PROPERTY ENVIRONMENT
    "READ_WRITE_CONCERN_OPERATION_TESTS_PATH=${DATA_SOURCE_DIR}/read-write-concern/operation"
)

set_property(TEST unified_format_specs APPEND PROPERTY ENVIRONMENT
    "CHANGE_STREAMS_UNIFIED_TESTS_PATH=${DATA_SOURCE_DIR}/change-streams/unified"
    "CLIENT_SIDE_ENCRYPTION_UNIFIED_TESTS_PATH=${DATA_SOURCE_DIR}/client_side_encryption/unified"
    "COLLECTION_MANAGEMENT_TESTS_PATH=${DATA_SOURCE_DIR}/collection-management"
    "CRUD_UNIFIED_TESTS_PATH=${DATA_SOURCE_DIR}/crud/unified"
    "INDEX_MANAGEMENT_TESTS_PATH=${DATA_SOURCE_DIR}/index-management"
    "RETRYABLE_READS_UNIFIED_TESTS_PATH=${DATA_SOURCE_DIR}/retryable-reads/unified/"
    "RETRYABLE_WRITES_UNIFIED_TESTS_PATH=${DATA_SOURCE_DIR}/retryable-writes/unified/"
    "SESSION_UNIFIED_TESTS_PATH=${DATA_SOURCE_DIR}/sessions/unified/"
    "TRANSACTIONS_UNIFIED_TESTS_PATH=${DATA_SOURCE_DIR}/transactions/unified"
    "UNIFIED_FORMAT_TESTS_PATH=${DATA_SOURCE_DIR}/unified-format"
    "VERSIONED_API_TESTS_PATH=${DATA_SOURCE_DIR}/versioned-api"
)

if(MONGOCXX_ENABLE_SLOW_TESTS)
    set_property(TEST driver APPEND PROPERTY ENVIRONMENT "MONGOCXX_ENABLE_SLOW_TESTS=1")
endif()

# Generate test to ensure macro guards behave properly.
if(ENABLE_MACRO_GUARD_TESTS)
    include(MacroGuardTest)

    add_macro_guard_test(
        PROJECT_NAME mongocxx
        PROJECT_TEST_PROPERTIES_TARGET mongocxx::test_properties
        GUARDED_MACROS
            # mongocxx/v1/config/export.hpp (generated by CMake)
            MONGOCXX_ABI_CDECL
            MONGOCXX_ABI_EXPORT
            MONGOCXX_ABI_EXPORT_CDECL
            MONGOCXX_ABI_NO_EXPORT
            MONGOCXX_DEPRECATED

            # mongocxx/v1/config/version.hpp (generated by CMake)
            MONGOCXX_VERSION_STRING
            MONGOCXX_VERSION_EXTRA
            MONGOCXX_VERSION_MAJOR
            MONGOCXX_VERSION_MINOR
            MONGOCXX_VERSION_PATCH

            # mongocxx/v1/detail/macros.hpp
            MONGOCXX_PRIVATE_UNREACHABLE

            # mongocxx/v1/detail/prelude.hpp
            MONGOCXX_PRIVATE_V1_INSIDE_MACRO_GUARD_SCOPE

            # config.hh (generated by CMake)
            MONGOCXX_ENABLE_SSL
            MONGOCXX_COMPILER_ID
            MONGOCXX_COMPILER_VERSION
        INCLUDE_PATTERNS
            "include/*.hpp" # Public headers.
        EXCLUDE_REGEXES
            "include/mongocxx/docs/.*\.hpp" # Doc header.
            "include/.*/(prelude|postlude)\.hpp" # Macro guard headers.
            "include/mongocxx/v1/detail/macros\.hpp" # v_noabi include-via-prelude headers.
            "include/mongocxx/v_noabi/mongocxx/config/.*\.hpp" # v_noabi config headers.
    )
endif()

set_dist_list(src_mongocxx_test_DIST
    CMakeLists.txt
    bulk_write.cpp
    catch_helpers.cpp
    catch_helpers.hh
    catch.cpp
    change_streams.cpp
    client_helpers.cpp
    client_helpers.hh
    client_session.cpp
    client_side_encryption.cpp
    client.cpp
    collection_mocked.cpp
    collection.cpp
    conversions.cpp
    database.cpp
    gridfs/bucket.cpp
    gridfs/downloader.cpp
    gridfs/uploader.cpp
    hint.cpp
    index_view.cpp
    instance.cpp
    logging.cpp
    model/delete_many.cpp
    model/delete_one.cpp
    model/insert_one.cpp
    model/replace_one.cpp
    model/update_many.cpp
    model/update_one.cpp
    options/aggregate.cpp
    options/bulk_write.cpp
    options/client_session.cpp
    options/count.cpp
    options/delete.cpp
    options/distinct.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
    options/find.cpp
    options/gridfs/bucket.cpp
    options/gridfs/upload.cpp
    options/index.cpp
    options/insert.cpp
    options/pool.cpp
    options/replace.cpp
    options/update.cpp
    pool.cpp
    private/numeric_casting.cpp
    private/scoped_bson_t.cpp
    private/write_concern.cpp
    read_concern.cpp
    read_preference.cpp
    result/bulk_write.cpp
    result/delete.cpp
    result/gridfs/upload.cpp
    result/insert_one.cpp
    result/replace_one.cpp
    result/update.cpp
    sdam-monitoring.cpp
    search_index_view.cpp
    spec/client_side_encryption.cpp
    spec/command_monitoring.cpp
    spec/crud.cpp
    spec/gridfs.cpp
    spec/initial_dns_seedlist_discovery.cpp
    spec/mongohouse.cpp
    spec/monitoring.cpp
    spec/monitoring.hh
    spec/operation.cpp
    spec/operation.hh
    spec/read_write_concern.cpp
    spec/retryable-reads.cpp
    spec/transactions.cpp
    spec/unified_tests/assert.cpp
    spec/unified_tests/assert.hh
    spec/unified_tests/entity.cpp
    spec/unified_tests/entity.hh
    spec/unified_tests/operations.cpp
    spec/unified_tests/operations.hh
    spec/unified_tests/runner.cpp
    spec/uri_options.cpp
    spec/util.cpp
    spec/util.hh
    test_macro_guards.cpp.in
    transactions.cpp
    uri.cpp
    validation_criteria.cpp
    version.cpp
    versioned_api.cpp
    write_concern.cpp
)
