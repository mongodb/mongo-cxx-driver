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

if(NOT BUILD_TESTING)
    set_property(DIRECTORY PROPERTY EXCLUDE_FROM_ALL TRUE)
endif()

if(BSONCXX_BUILD_SHARED)
    bsoncxx_add_library(bsoncxx_testing "bsoncxx-testing" SHARED)
else()
    bsoncxx_add_library(bsoncxx_testing "bsoncxx-testing" STATIC)
endif()

target_compile_definitions(bsoncxx_testing PUBLIC BSONCXX_TESTING)

# Allow `#include <third_party/catch/include/...>`
target_include_directories(bsoncxx_testing PUBLIC ${THIRD_PARTY_SOURCE_DIR}/..)

add_executable(test_bson
    array.cpp
    bson_b_date.cpp
    bson_builder.cpp
    bson_get_values.cpp
    bson_serialization.cpp
    bson_types.cpp
    bson_util_itoa.cpp
    bson_validate.cpp
    bson_value.cpp
    json.cpp
    oid.cpp
    view_or_value.cpp
)

# Common target properties for test executables.
add_library(bsoncxx_test_properties INTERFACE)
target_sources(bsoncxx_test_properties INTERFACE ${THIRD_PARTY_SOURCE_DIR}/catch/main.cpp)
target_link_libraries(bsoncxx_test_properties INTERFACE bsoncxx_testing ${libbson_target})
target_include_directories(bsoncxx_test_properties INTERFACE ${libbson_include_directories})
target_compile_definitions(bsoncxx_test_properties INTERFACE ${libbson_definitions})

target_link_libraries(test_bson PRIVATE bsoncxx_test_properties)

add_test(NAME bson COMMAND test_bson)

set_dist_list(src_bsoncxx_test_DIST
    CMakeLists.txt
    array.cpp
    bson_b_date.cpp
    bson_builder.cpp
    bson_get_values.cpp
    bson_serialization.cpp
    bson_types.cpp
    bson_util_itoa.cpp
    bson_validate.cpp
    bson_value.cpp
    json.cpp
    oid.cpp
    view_or_value.cpp
)
