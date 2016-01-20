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

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if(POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif()

project(MONGO_CXX_DRIVER LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8.2")
    message(FATAL_ERROR "Insufficient GCC version - GCC 4.8.2+ required")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.0.23506")
    message(FATAL_ERROR "Insufficient Microsoft Visual C++ version - MSVC 2015 Update 1+ required")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.1")
    message(FATAL_ERROR "Insufficient Apple clang version - XCode 5.1+ required")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.5")
    message(FATAL_ERROR "Insufficient clang version - clang 3.5+ required")
  endif()
else()
  message(WARNING "Unknown compiler... recklessly proceeding without a version check")
endif()

set (CMAKE_SKIP_BUILD_RPATH false)
set (CMAKE_BUILD_WITH_INSTALL_RPATH false)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
# Ensure that RPATH is used on OSX
set(CMAKE_MACOSX_RPATH 1)

# Add in our modules, we write FindX modules for libbson and libmongoc
# since they don't currently install them.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Enforce the C++ standard, and disable extensions
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

# Include the required modules
include(GenerateExportHeader)
include(InstallRequiredSystemLibraries)

# If the user did not customize the install prefix,
# set it to live under build so we don't inadverently pollute /usr/local
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default is Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(docs
    doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    VERBATIM
)

add_custom_target(format
    python ${CMAKE_SOURCE_DIR}/etc/clang_format.py format
    VERBATIM
)

add_custom_target(format-lint
    python ${CMAKE_SOURCE_DIR}/etc/clang_format.py lint
    VERBATIM
)

set(THIRD_PARTY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party)

enable_testing()

add_subdirectory(src)

add_subdirectory(examples EXCLUDE_FROM_ALL)
