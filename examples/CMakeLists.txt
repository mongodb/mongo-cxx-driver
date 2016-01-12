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

add_custom_target(install_headers
    COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=dev -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
    DEPENDS bsoncxx_built mongocxx_built
)

add_custom_target(install_libs
    COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=runtime -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
    DEPENDS bsoncxx_built mongocxx_built
)

add_subdirectory(bsoncxx)
add_subdirectory(mongocxx)

add_custom_target(run-examples DEPENDS ${MONGOCXX_EXAMPLE_EXECUTABLES} ${BSONCXX_EXAMPLE_EXECUTABLES})

# Run all mongocxx examples on `make examples`.
foreach(EXAMPLE ${MONGOCXX_EXAMPLE_EXECUTABLES})
    get_filename_component(EXAMPLE_EXECUTABLE "${CMAKE_BINARY_DIR}/examples/mongocxx/${EXAMPLE}" ABSOLUTE)
    add_custom_command(TARGET run-examples POST_BUILD COMMAND ${EXAMPLE_EXECUTABLE})
endforeach(EXAMPLE)

# Run all bsoncxx examples on `make examples`.
foreach(EXAMPLE ${BSONCXX_EXAMPLE_EXECUTABLES})
    get_filename_component(EXAMPLE_EXECUTABLE "${CMAKE_BINARY_DIR}/examples/bsoncxx/${EXAMPLE}" ABSOLUTE)
    add_custom_command(TARGET run-examples POST_BUILD COMMAND ${EXAMPLE_EXECUTABLE})
endforeach(EXAMPLE)
