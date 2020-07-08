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
    transactions.cpp
    uri.cpp
    validation_criteria.cpp
    write_concern.cpp
)

set(spec_test_common
    spec/operation.cpp
    spec/monitoring.cpp
    spec/util.cpp)

add_executable(test_driver
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
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
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/crud.cpp
    ${spec_test_common}
)

add_executable(test_change_stream_specs
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/change_stream.cpp
    ${spec_test_common}
)

add_executable(test_gridfs_specs
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/gridfs.cpp
    ${spec_test_common}
)

add_executable(test_client_side_encryption_specs
  ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
  ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
  spec/client_side_encryption.cpp
  ${spec_test_common}
)

add_executable(test_command_monitoring_specs
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/command_monitoring.cpp
    ${spec_test_common}
)

add_executable(test_transactions_specs
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/transactions.cpp
    ${spec_test_common}
)

add_executable(test_retryable_reads_specs
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/retryable-reads.cpp
    ${spec_test_common}
)

add_executable(test_read_write_concern_specs
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/read_write_concern.cpp
    ${spec_test_common}
)

target_link_libraries(test_driver mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_logging mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_instance mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_client_side_encryption_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_crud_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_gridfs_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_command_monitoring_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_change_stream_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_transactions_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_retryable_reads_specs mongocxx_mocked ${libmongoc_target})
target_link_libraries(test_read_write_concern_specs mongocxx_mocked ${libmongoc_target})

target_include_directories(test_driver PRIVATE ${libmongoc_include_directories})
target_include_directories(test_logging PRIVATE ${libmongoc_include_directories})
target_include_directories(test_instance PRIVATE ${libmongoc_include_directories})
target_include_directories(test_crud_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_gridfs_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_command_monitoring_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_change_stream_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_transactions_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_retryable_reads_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_read_write_concern_specs PRIVATE ${libmongoc_include_directories})

target_compile_definitions(test_driver PRIVATE ${libmongoc_definitions})
target_compile_definitions(test_logging PRIVATE ${libmongoc_definitions})
target_compile_definitions(test_instance PRIVATE ${libmongoc_definitions})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_driver PRIVATE /bigobj)
endif()

add_test(driver test_driver)
add_test(logging test_logging)
add_test(instance test_instance)
add_test(crud_specs test_crud_specs)
add_test(gridfs_specs test_gridfs_specs)
add_test(client_side_encryption_specs test_client_side_encryption_specs)
add_test(command_monitoring_specs test_command_monitoring_specs)
add_test(change_stream_specs test_change_stream_specs)
add_test(transactions_specs test_transactions_specs)
add_test(retryable_reads_spec test_retryable_reads_specs)
add_test(read_write_concern_specs test_read_write_concern_specs)

set_tests_properties(crud_specs PROPERTIES
    ENVIRONMENT "CRUD_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/crud")

set_tests_properties(gridfs_specs PROPERTIES
    ENVIRONMENT "GRIDFS_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/gridfs")

set_tests_properties(client_side_encryption_specs PROPERTIES
    ENVIRONMENT "ENCRYPTION_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/client_side_encryption")

set_tests_properties(driver PROPERTIES
    ENVIRONMENT "ENCRYPTION_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/client_side_encryption")
  
set_tests_properties(command_monitoring_specs PROPERTIES
    ENVIRONMENT "COMMAND_MONITORING_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/command-monitoring")

set_tests_properties(change_stream_specs PROPERTIES
    ENVIRONMENT "CHANGE_STREAM_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/change_stream")

set_tests_properties(transactions_specs PROPERTIES
  ENVIRONMENT "TRANSACTIONS_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/transactions")
set_property(TEST transactions_specs
  APPEND PROPERTY ENVIRONMENT "WITH_TRANSACTION_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/with_transaction")

set_tests_properties(retryable_reads_spec PROPERTIES
   ENVIRONMENT "RETRYABLE_READS_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/retryable-reads")

set_tests_properties(read_write_concern_specs PROPERTIES
   ENVIRONMENT "READ_WRITE_CONCERN_OPERATION_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/read-write-concern/operation")

if (MONGOCXX_ENABLE_SLOW_TESTS)
  set_tests_properties(driver PROPERTIES
    ENVIRONMENT "MONGOCXX_ENABLE_SLOW_TESTS=1")
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
   spec/change_stream.cpp
   spec/client_side_encryption.cpp
   spec/command_monitoring.cpp
   spec/crud.cpp
   spec/gridfs.cpp
   spec/monitoring.cpp
   spec/monitoring.hh
   spec/operation.cpp
   spec/operation.hh
   spec/read_write_concern.cpp
   spec/retryable-reads.cpp
   spec/transactions.cpp
   spec/util.cpp
   spec/util.hh
   transactions.cpp
   uri.cpp
   validation_criteria.cpp
   write_concern.cpp
)
