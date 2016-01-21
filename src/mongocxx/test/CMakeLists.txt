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

link_directories(${LIBMONGOC_LIBRARY_DIRS} ${LIBBSON_LIBRARY_DIRS})

set(mongocxx_test_sources
    CMakeLists.txt
    bulk_write.cpp
    client.cpp
    collection.cpp
    collection_mocked.cpp
    database.cpp
    hint.cpp
    model/update_many.cpp
    options/aggregate.cpp
    options/create_collection.cpp
    options/find.cpp
    options/find_one_and_delete.cpp
    options/find_one_and_replace.cpp
    options/find_one_and_update.cpp
    options/index.cpp
    options/insert.cpp
    options/modify_collection.cpp
    options/update.cpp
    pool.cpp
    private/scoped_bson_t.cpp
    private/write_concern.cpp
    read_concern.cpp
    read_preference.cpp
    result/delete.cpp
    result/insert_one.cpp
    result/replace_one.cpp
    result/update.cpp
    validation_criteria.cpp
    write_concern.cpp
)

add_executable(test_driver
    ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
    ${mongocxx_test_sources}
)

target_link_libraries(test_driver mongocxx_mocked)

add_executable(test_instance
  ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp
  instance.cpp
)

target_link_libraries(test_instance mongocxx_mocked)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(test_driver PRIVATE /bigobj)
endif()

add_test(driver test_driver)
add_test(instance test_instance)
