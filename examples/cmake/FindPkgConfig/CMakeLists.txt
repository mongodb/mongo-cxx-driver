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

# Demonstrates how to use the CMake 'FindPkgConfig' mechanism to locate
# and build against libmongocxx and libbsoncxx.

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(MONGO_CXX_DRIVER LANGUAGES CXX)

# Enforce the C++ standard, and disable extensions
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig QUIET)

# NOTE: For this to work, the PKG_CONFIG_PATH variable (man pkg-config) must be set to point to the
# 'lib/pkgconfig' subdirectory of the directory used as the argument to CMAKE_INSTALL_PREFIX when
# building libmongocxx and libbsoncxx.
pkg_search_module(BSONCXX REQUIRED libbsoncxx)
pkg_search_module(MONGOCXX REQUIRED libmongocxx)

link_directories(
  ${MONGOCXX_LIBRARY_DIRS}
  ${BSONCXX_LIBRARY_DIRS}
)

add_executable(hello_mongocxx hello_mongocxx.cpp)

target_include_directories(hello_mongocxx
  PRIVATE ${MONGOCXX_INCLUDE_DIRS}
  PRIVATE ${BSONCXX_INCLUDE_DIRS}
)
target_link_libraries(hello_mongocxx
  PRIVATE ${MONGOCXX_LIBRARIES} ${BSONCXX_LIBRARIES}
)
