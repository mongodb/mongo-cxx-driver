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

# Avoid x86 (32-bit) compatibility preventing compilation of large test TUs:
#  MSVC: "C1128: number of sections exceeded object file format".
#  GCC: "too many sections" and "file too big".
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Compiling test executables with '/bigobj'")
    add_compile_options("/bigobj")
elseif(MINGW)
    message(STATUS "Compiling test executables with '-Wa,-mbig-obj'")
    add_compile_options("-Wa,-mbig-obj")
endif()

if(MONGOCXX_BUILD_SHARED)
    mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" SHARED)
else()
    mongocxx_add_library(mongocxx_mocked "mongocxx-mocked" STATIC)
endif()

target_link_libraries(mongocxx_mocked PUBLIC bsoncxx_testing)
target_compile_definitions(mongocxx_mocked PUBLIC MONGOCXX_TESTING)

set(mongocxx_test_bsoncxx
    ${PROJECT_SOURCE_DIR}/../bsoncxx/test/v1/array/view_string_maker.cpp
    ${PROJECT_SOURCE_DIR}/../bsoncxx/test/v1/document/view_string_maker.cpp
    ${PROJECT_SOURCE_DIR}/../bsoncxx/test/v1/element/view_string_maker.cpp
)

set(mongocxx_test_sources_private
    private/numeric_casting.cpp
    private/scoped_bson_value.cpp
    private/scoped_bson.cpp
    private/mongoc_version.cpp
)

set(mongocxx_test_sources_v_noabi
    v_noabi/bulk_write.cpp
    v_noabi/change_streams.cpp
    v_noabi/client_session.cpp
    v_noabi/client_side_encryption.cpp
    v_noabi/client.cpp
    v_noabi/collection_mocked.cpp
    v_noabi/collection.cpp
    v_noabi/conversions.cpp
    v_noabi/database.cpp
    v_noabi/gridfs/bucket.cpp
    v_noabi/gridfs/downloader.cpp
    v_noabi/gridfs/uploader.cpp
    v_noabi/hint.cpp
    v_noabi/index_view.cpp
    v_noabi/model/delete_many.cpp
    v_noabi/model/delete_one.cpp
    v_noabi/model/insert_one.cpp
    v_noabi/model/replace_one.cpp
    v_noabi/model/update_many.cpp
    v_noabi/model/update_one.cpp
    v_noabi/options/aggregate.cpp
    v_noabi/options/bulk_write.cpp
    v_noabi/options/client_session.cpp
    v_noabi/options/count.cpp
    v_noabi/options/delete.cpp
    v_noabi/options/distinct.cpp
    v_noabi/options/find_one_and_delete.cpp
    v_noabi/options/find_one_and_replace.cpp
    v_noabi/options/find_one_and_update.cpp
    v_noabi/options/find.cpp
    v_noabi/options/gridfs/bucket.cpp
    v_noabi/options/gridfs/upload.cpp
    v_noabi/options/index.cpp
    v_noabi/options/insert.cpp
    v_noabi/options/pool.cpp
    v_noabi/options/replace.cpp
    v_noabi/options/update.cpp
    v_noabi/pool.cpp
    v_noabi/read_concern.cpp
    v_noabi/read_preference.cpp
    v_noabi/result/bulk_write.cpp
    v_noabi/result/delete.cpp
    v_noabi/result/gridfs/upload.cpp
    v_noabi/result/insert_one.cpp
    v_noabi/result/replace_one.cpp
    v_noabi/result/update.cpp
    v_noabi/sdam-monitoring.cpp
    v_noabi/search_index_view.cpp
    v_noabi/transactions.cpp
    v_noabi/uri.cpp
    v_noabi/validation_criteria.cpp
    v_noabi/write_concern.cpp
)

set(mongocxx_test_sources_v1
    v1/aggregate_options.cpp
    v1/apm.cpp
    v1/auto_encryption_options.cpp
    v1/bsoncxx.cpp
    v1/bulk_write.cpp
    v1/change_stream.cpp
    v1/count_options.cpp
    v1/cursor.cpp
    v1/data_key_options.cpp
    v1/delete_many_options.cpp
    v1/delete_one_options.cpp
    v1/distinct_options.cpp
    v1/estimated_document_count_options.cpp
    v1/events.cpp
    v1/exception.cpp
    v1/find_one_and_delete_options.cpp
    v1/find_one_and_replace_options.cpp
    v1/find_one_and_update_options.cpp
    v1/find_options.cpp
    v1/hint.cpp
    v1/insert_many_options.cpp
    v1/insert_one_options.cpp
    v1/insert_one_result.cpp
    v1/logger.cpp
    v1/pipeline.cpp
    v1/range_options.cpp
    v1/read_concern.cpp
    v1/read_preference.cpp
    v1/replace_one_options.cpp
    v1/rewrap_many_datakey_options.cpp
    v1/rewrap_many_datakey_result.cpp
    v1/server_api.cpp
    v1/server_error.cpp
    v1/tls.cpp
    v1/transaction_options.cpp
    v1/update_many_options.cpp
    v1/update_one_options.cpp
    v1/uri.cpp
    v1/write_concern.cpp
)

set(mongocxx_test_sources_spec
    spec/monitoring.cpp
    spec/operation.cpp
    spec/unified_tests/assert.cpp
    spec/unified_tests/entity.cpp
    spec/util.cpp
)

set(mongocxx_test_sources_extra
    catch.cpp
    spec/client_side_encryption.cpp
    spec/command_monitoring.cpp
    spec/crud.cpp
    spec/gridfs.cpp
    spec/read_write_concern.cpp
    spec/retryable-reads.cpp
    spec/transactions.cpp
    spec/unified_tests/operations.cpp
    spec/unified_tests/runner.cpp
    subprocess.cpp
    v_noabi/catch_helpers.cpp
    v_noabi/client_helpers.cpp
    v_noabi/instance.cpp
    v_noabi/logging.cpp
    v1/instance.cpp
)

file(GLOB_RECURSE mongocxx_test_headers
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    CONFIGURE_DEPENDS
    "*.hh"
)

add_library(client_helpers OBJECT v_noabi/client_helpers.cpp)

add_library(spec_test_common OBJECT ${mongocxx_test_sources_spec})

add_executable(test_driver
    ${mongocxx_test_bsoncxx}
    ${mongocxx_test_sources_private}
    ${mongocxx_test_sources_v_noabi}
    ${mongocxx_test_sources_v1}
    v_noabi/catch_helpers.cpp
)
target_link_libraries(test_driver PRIVATE spec_test_common client_helpers)

add_executable(test_instance
    subprocess.cpp
    v_noabi/instance.cpp
    v_noabi/logging.cpp
    v1/instance.cpp
)
target_compile_definitions(test_instance PRIVATE MONGOCXX_TEST_DISABLE_MAIN_INSTANCE)

add_executable(test_crud_specs
    ${mongocxx_test_bsoncxx}
    spec/crud.cpp
)
target_link_libraries(test_crud_specs PRIVATE spec_test_common client_helpers)

add_executable(test_gridfs_specs
    ${mongocxx_test_bsoncxx}
    spec/gridfs.cpp
)
target_link_libraries(test_gridfs_specs PRIVATE spec_test_common client_helpers)

add_executable(test_client_side_encryption_specs
    ${mongocxx_test_bsoncxx}
    spec/client_side_encryption.cpp
)
target_link_libraries(test_client_side_encryption_specs PRIVATE spec_test_common client_helpers)

add_executable(test_command_monitoring_specs
    ${mongocxx_test_bsoncxx}
    spec/command_monitoring.cpp
)
target_link_libraries(test_command_monitoring_specs PRIVATE spec_test_common client_helpers)

add_executable(test_transactions_specs
    ${mongocxx_test_bsoncxx}
    spec/transactions.cpp
)
target_link_libraries(test_transactions_specs PRIVATE spec_test_common client_helpers)

add_executable(test_retryable_reads_specs
    ${mongocxx_test_bsoncxx}
    spec/retryable-reads.cpp
)
target_link_libraries(test_retryable_reads_specs PRIVATE spec_test_common client_helpers)

add_executable(test_read_write_concern_specs
    ${mongocxx_test_bsoncxx}
    spec/read_write_concern.cpp
)
target_link_libraries(test_read_write_concern_specs PRIVATE spec_test_common client_helpers)

add_executable(test_unified_format_specs
    ${mongocxx_test_bsoncxx}
    spec/unified_tests/operations.cpp spec/unified_tests/runner.cpp
)
target_link_libraries(test_unified_format_specs PRIVATE spec_test_common client_helpers)

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
        test_read_write_concern_specs
        test_retryable_reads_specs
        test_transactions_specs
        test_unified_format_specs
    APPEND PROPERTY LINK_LIBRARIES
        mongocxx::test_properties_with_main
)

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
    read_write_concern_specs
    retryable_reads_specs
    transactions_specs
    unified_format_specs
)
    add_test(NAME ${test_name} COMMAND test_${test_name} --reporter compact --allow-running-no-tests)
endforeach()

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
    ${mongocxx_test_sources_private}
    ${mongocxx_test_sources_v_noabi}
    ${mongocxx_test_sources_v1}
    ${mongocxx_test_sources_spec}
    ${mongocxx_test_sources_extra}
    ${mongocxx_test_headers}
)
