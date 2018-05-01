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
    options/create_view.cpp
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
    options/modify_collection.cpp
    options/pool.cpp
    options/private/rewriter.cpp
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
    uri.cpp
    validation_criteria.cpp
    write_concern.cpp
)

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
)

add_executable(test_gridfs_specs
    ${CMAKE_SOURCE_DIR}/src/mongocxx/test_util/client_helpers.cpp
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    spec/gridfs.cpp
)

target_link_libraries(test_driver mongocxx_mocked ${libmongoc_libraries})
target_link_libraries(test_logging mongocxx_mocked ${libmongoc_libraries})
target_link_libraries(test_instance mongocxx_mocked ${libmongoc_libraries})
target_link_libraries(test_crud_specs mongocxx_mocked ${libmongoc_libraries})
target_link_libraries(test_gridfs_specs mongocxx_mocked ${libmongoc_libraries})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_driver PRIVATE /bigobj)
endif()

target_include_directories(test_driver PRIVATE ${libmongoc_include_directories})
target_include_directories(test_logging PRIVATE ${libmongoc_include_directories})
target_include_directories(test_instance PRIVATE ${libmongoc_include_directories})
target_include_directories(test_crud_specs PRIVATE ${libmongoc_include_directories})
target_include_directories(test_gridfs_specs PRIVATE ${libmongoc_include_directories})

target_compile_definitions(test_driver PRIVATE ${libmongoc_definitions})
target_compile_definitions(test_logging PRIVATE ${libmongoc_definitions})
target_compile_definitions(test_instance PRIVATE ${libmongoc_definitions})

add_test(driver test_driver)
add_test(logging test_logging)
add_test(instance test_instance)
add_test(crud_specs test_crud_specs)
add_test(gridfs_specs test_gridfs_specs)

set_tests_properties(crud_specs PROPERTIES
    ENVIRONMENT "CRUD_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/crud")

set_tests_properties(gridfs_specs PROPERTIES
    ENVIRONMENT "GRIDFS_TESTS_PATH=${CMAKE_SOURCE_DIR}/data/gridfs")

if (MONGOCXX_ENABLE_SLOW_TESTS)
  set_tests_properties(driver PROPERTIES
    ENVIRONMENT "MONGOCXX_ENABLE_SLOW_TESTS=1")
endif()
