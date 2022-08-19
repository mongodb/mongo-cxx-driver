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

include_directories(
    ${THIRD_PARTY_SOURCE_DIR}/catch/include
)

set(test_driver_sources
    CMakeLists.txt
    bulk_write.cpp
    change_streams.cpp
    client.cpp
    client_session.cpp
    client_side_encryption.cpp
    collection.cpp
    collection_mocked.cpp
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
    options/create_collection.cpp
    options/delete.cpp
    options/distinct.cpp
    options/find.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
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
    spec/monitoring.cpp
    spec/monitoring.hh
    spec/unified_tests/assert.cpp
    spec/unified_tests/entity.cpp
    transactions.cpp
    uri.cpp
    validation_criteria.cpp
    write_concern.cpp
)

set(spec_test_common
    spec/operation.cpp
    spec/unified_tests/entity.cpp
    spec/unified_tests/assert.cpp
    spec/monitoring.cpp
    spec/util.cpp
)

add_executable(test_driver
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    ${test_driver_sources}
)

add_executable(test_logging
  ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
  logging.cpp
)

add_executable(test_instance
  ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
  instance.cpp
)

add_executable(test_crud_specs
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/crud.cpp
    ${spec_test_common}
)

add_executable(test_gridfs_specs
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/gridfs.cpp
    ${spec_test_common}
)

add_executable(test_client_side_encryption_specs
  ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
  ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
  spec/client_side_encryption.cpp
  ${spec_test_common}
)

add_executable(test_command_monitoring_specs
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/command_monitoring.cpp
    ${spec_test_common}
)

add_executable(test_transactions_specs
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/transactions.cpp
    ${spec_test_common}
)

add_executable(test_retryable_reads_specs
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/retryable-reads.cpp
    ${spec_test_common}
)

add_executable(test_read_write_concern_specs
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/read_write_concern.cpp
    ${spec_test_common}
)

add_executable(test_mongohouse_specs
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/mongohouse.cpp
    ${spec_test_common}
)

add_executable(test_unified_format_spec
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/unified_tests/operations.cpp
    spec/unified_tests/runner.cpp
    ${spec_test_common}
)

add_executable(test_versioned_api
    ${PROJECT_SOURCE_DIR}/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    versioned_api.cpp
    ${spec_test_common}
    )

target_link_libraries(test_driver mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_logging mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_instance mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_client_side_encryption_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_crud_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_gridfs_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_command_monitoring_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_transactions_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_retryable_reads_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_read_write_concern_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_mongohouse_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_unified_format_spec mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_versioned_api mongocxx_mocked ${libmongoc_target})

target_include_directories(test_driver PRIVATE ${libmongoc_include_directories})
target_include_directories(test_logging PRIVATE ${libmongoc_include_directories})
target_include_directories(test_instance PRIVATE ${libmongoc_include_directories})
target_include_directories(test_client_side_encryption_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_crud_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_gridfs_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_command_monitoring_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_transactions_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_retryable_reads_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_read_write_concern_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_mongohouse_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_unified_format_spec PRIVATE ${libmongoc_include_directories})
target_include_directories(test_versioned_api PRIVATE ${libmongoc_include_directories})

target_compile_definitions(test_driver PRIVATE ${libmongoc_definitions})
target_compile_definitions(test_logging PRIVATE ${libmongoc_definitions})
target_compile_definitions(test_instance PRIVATE ${libmongoc_definitions})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_driver PRIVATE /bigobj)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_driver PRIVATE -Wno-maybe-uninitialized)
    target_compile_options(test_retryable_reads_specs PRIVATE -Wno-maybe-uninitialized)
endif()

add_test(driver test_driver)
add_test(logging test_logging)
add_test(instance test_instance)
add_test(crud_specs test_crud_specs)
add_test(gridfs_specs test_gridfs_specs)
add_test(client_side_encryption_specs test_client_side_encryption_specs)
add_test(command_monitoring_specs test_command_monitoring_specs)
add_test(transactions_specs test_transactions_specs)
add_test(retryable_reads_spec test_retryable_reads_specs)
add_test(read_write_concern_specs test_read_write_concern_specs)
add_test(unified_format_spec test_unified_format_spec)
add_test(versioned_api test_versioned_api)

# Adding this as a test will run it as part of the RUN_TESTS command in MSVC.
# Do not add, since we only test mongohouse on Linux.
#add_test(mongohouse_specs test_mongohouse_specs)

set_tests_properties(crud_specs PROPERTIES
    ENVIRONMENT "CRUD_LEGACY_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/crud/legacy")

set_tests_properties(gridfs_specs PROPERTIES
    ENVIRONMENT "GRIDFS_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/gridfs")

set_tests_properties(client_side_encryption_specs PROPERTIES
    ENVIRONMENT "ENCRYPTION_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/client_side_encryption")

set_tests_properties(driver PROPERTIES
    ENVIRONMENT "ENCRYPTION_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/client_side_encryption")

set_tests_properties(command_monitoring_specs PROPERTIES
    ENVIRONMENT "COMMAND_MONITORING_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/command-monitoring")

set_tests_properties(transactions_specs PROPERTIES
  ENVIRONMENT "TRANSACTIONS_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/transactions")
set_property(TEST transactions_specs
  APPEND PROPERTY ENVIRONMENT "WITH_TRANSACTION_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/with_transaction")

set_tests_properties(retryable_reads_spec PROPERTIES
   ENVIRONMENT "RETRYABLE_READS_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/retryable-reads")

set_tests_properties(read_write_concern_specs PROPERTIES
  ENVIRONMENT "READ_WRITE_CONCERN_OPERATION_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/read-write-concern/operation")

set_property(TEST unified_format_spec APPEND PROPERTY ENVIRONMENT
  "CHANGE_STREAMS_UNIFIED_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/change-streams/unified"
  "COLLECTION_MANAGEMENT_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/collection-management"
  "CRUD_UNIFIED_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/crud/unified"
  "SESSION_UNIFIED_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/sessions/unified/"
  "UNIFIED_FORMAT_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/unified-format"
  "VERSIONED_API_TESTS_PATH=${PROJECT_SOURCE_DIR}/../../data/versioned-api"
)

if (MONGOCXX_ENABLE_SLOW_TESTS)
  set_property(TEST driver
    APPEND PROPERTY ENVIRONMENT "MONGOCXX_ENABLE_SLOW_TESTS=1")
endif()

set_dist_list (src_mongocxx_test_DIST
   CMakeLists.txt
   bulk_write.cpp
   change_streams.cpp
   client.cpp
   client_session.cpp
   client_side_encryption.cpp
   collection.cpp
   collection_mocked.cpp
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
   options/create_collection.cpp
   options/delete.cpp
   options/distinct.cpp
   options/find.cpp
   options/find_one_and_delete.cpp
   options/find_one_and_replace.cpp
   options/find_one_and_update.cpp
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
   spec/client_side_encryption.cpp
   spec/command_monitoring.cpp
   spec/crud.cpp
   spec/gridfs.cpp
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
   spec/util.cpp
   spec/util.hh
   transactions.cpp
   uri.cpp
   validation_criteria.cpp
   versioned_api.cpp
   write_concern.cpp
)
