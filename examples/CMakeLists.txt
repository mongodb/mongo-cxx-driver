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

# NOTE: The targets in this directory will only build if you have
# installed the bsoncxx and mongocxx libraries via the 'install'
# targets.

# TODO: Ideally, there would be dependencies here on the install targets
# for bsoncxx and mongocxx. However, there is currently no way to express
# that dependency in CMake (see https://cmake.org/Bug/view.php?id=8438)

# Build the examples with vectorcall as the default on Windows
# so that we shake out missing _CALL macros.
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Gv")
endif()

add_subdirectory(bsoncxx)
add_subdirectory(mongocxx)

add_custom_target(examples DEPENDS ${MONGOCXX_EXAMPLE_EXECUTABLES} ${BSONCXX_EXAMPLE_EXECUTABLES})

add_custom_target(run-examples DEPENDS examples)

# Run all mongocxx examples on `make run-examples`.
foreach(EXAMPLE ${MONGOCXX_EXAMPLE_EXECUTABLES})
    get_filename_component(EXAMPLE_EXECUTABLE "${CMAKE_BINARY_DIR}/examples/mongocxx/${EXAMPLE}" ABSOLUTE)
    add_custom_command(TARGET run-examples POST_BUILD COMMAND ${EXAMPLE_EXECUTABLE})
endforeach(EXAMPLE)

# Run all bsoncxx examples on `make run-examples`.
foreach(EXAMPLE ${BSONCXX_EXAMPLE_EXECUTABLES})
    get_filename_component(EXAMPLE_EXECUTABLE "${CMAKE_BINARY_DIR}/examples/bsoncxx/${EXAMPLE}" ABSOLUTE)
    add_custom_command(TARGET run-examples POST_BUILD COMMAND ${EXAMPLE_EXECUTABLE})
endforeach(EXAMPLE)
