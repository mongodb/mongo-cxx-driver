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
    ${CMAKE_SOURCE_DIR}/src/third_party/catch/include
)

add_executable(test_bson
    bson_b_date.cpp
    bson_builder.cpp
    bson_get_values.cpp
    bson_util_itoa.cpp
    bson_validate.cpp
    json.cpp
    new_tests.cpp
    view_or_value.cpp
)

target_link_libraries(test_bson bsoncxx_static)
add_test(bson test_bson)
