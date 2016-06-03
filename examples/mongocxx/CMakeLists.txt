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
    ${CMAKE_INSTALL_PREFIX}/${BSONCXX_HEADER_INSTALL_DIR}
    ${CMAKE_INSTALL_PREFIX}/${MONGOCXX_HEADER_INSTALL_DIR}
)

link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib
)

set(MONGOCXX_EXAMPLES
    aggregate.cpp
    bulk_write.cpp
    create.cpp
    connect.cpp
    document_validation.cpp
    exception.cpp
    index.cpp
    inserted_id.cpp
    query.cpp
    remove.cpp
    update.cpp
    view_or_value_variant.cpp
)

foreach(EXAMPLE_SRC ${MONGOCXX_EXAMPLES})
    get_filename_component(EXAMPLE_TARGET ${EXAMPLE_SRC} NAME_WE)
    add_executable(${EXAMPLE_TARGET} ${EXAMPLE_SRC})
    target_link_libraries(${EXAMPLE_TARGET} mongocxx)
    set(MONGOCXX_EXAMPLE_EXECUTABLES ${MONGOCXX_EXAMPLE_EXECUTABLES} ${EXAMPLE_TARGET})
endforeach(EXAMPLE_SRC)

set(MONGOCXX_EXAMPLE_EXECUTABLES ${MONGOCXX_EXAMPLE_EXECUTABLES} PARENT_SCOPE)
