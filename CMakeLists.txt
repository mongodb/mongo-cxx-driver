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

# All of our target compilers support the deprecated
# attribute. Normally, we would just let the GenerateExportHeader
# subsystem do this via configure check, but there appears to be a
# CMake bug where if -Werror is set on the command line, it breaks the
# configure check, and we end up not configuring the macro. That means
# that we end up not being able to turn deprecation warnings into
# errors. Instead, since we know all our platforms offer the feature,
# just hard enable it here.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(COMPILER_HAS_DEPRECATED true)
else()
  set(COMPILER_HAS_DEPRECATED_ATTR true)
endif()

set (CMAKE_SKIP_BUILD_RPATH false)
set (CMAKE_BUILD_WITH_INSTALL_RPATH false)
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
# Ensure that RPATH is used on OSX
set(CMAKE_MACOSX_RPATH 1)

# Enforce the C++ standard, and disable extensions
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

# Include the required modules
include(GenerateExportHeader)
include(InstallRequiredSystemLibraries)

# Allow the user to decide whether to build the shared libaries or the static libraries.
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
set(BUILD_VERSION "0.0.0" CACHE STRING "Library version (for both libbsoncxx and libmongocxx)")

# Allow the user to decide whether to also build static libraries
option(BUILD_SHARED_AND_STATIC_LIBS "Build static libraries" OFF)

# Allow the user to decide whether to use shared libraries or static libraries
# for the mongo-c-driver
option(BUILD_SHARED_LIBS_WITH_STATIC_MONGOC
       "Use static mongo-c-driver libraries with shared mongo-cxx-driver libraries"
        OFF
)

option (ENABLE_UNINSTALL "Enable creation of uninstall script and associated uninstall build target." ON)

# Disambiguate our options into clear flags
if(BUILD_SHARED_LIBS)
    set(BSONCXX_BUILD_SHARED ON CACHE INTERNAL "")
    set(MONGOCXX_BUILD_SHARED ON CACHE INTERNAL "")

    if(BUILD_SHARED_AND_STATIC_LIBS)
        set(BSONCXX_BUILD_STATIC ON CACHE INTERNAL "")
        set(MONGOCXX_BUILD_STATIC ON CACHE INTERNAL "")
    else()
        set(BSONCXX_BUILD_STATIC OFF CACHE INTERNAL "")
        set(MONGOCXX_BUILD_STATIC OFF CACHE INTERNAL "")
    endif()

    if(BUILD_SHARED_LIBS_WITH_STATIC_MONGOC)
        set(BSONCXX_LINK_WITH_STATIC_MONGOC ON CACHE INTERNAL "")
        set(MONGOCXX_LINK_WITH_STATIC_MONGOC ON CACHE INTERNAL "")
    else()
        set(BSONCXX_LINK_WITH_STATIC_MONGOC OFF CACHE INTERNAL "")
        set(MONGOCXX_LINK_WITH_STATIC_MONGOC OFF CACHE INTERNAL "")
    endif()
else()
    # Give a fatal error if we have a non-sensical combination
    if(BUILD_SHARED_AND_STATIC_LIBS)
        message(FATAL_ERROR
                "BUILD_SHARED_LIBS is OFF but BUILD_SHARED_AND_STATIC_LIBS is ON. \
To build static libraries only, set both BUILD_SHARED_LIBS and BUILD_SHARED_AND_STATIC_LIBS to OFF"
        )
    endif()

    set(BSONCXX_BUILD_SHARED OFF CACHE INTERNAL "")
    set(MONGOCXX_BUILD_SHARED OFF CACHE INTERNAL "")

    set(BSONCXX_BUILD_STATIC ON CACHE INTERNAL "")
    set(MONGOCXX_BUILD_STATIC ON CACHE INTERNAL "")

    set(BSONCXX_LINK_WITH_STATIC_MONGOC ON CACHE INTERNAL "")
    set(MONGOCXX_LINK_WITH_STATIC_MONGOC ON CACHE INTERNAL "")
endif()


# If the user did not customize the install prefix,
# set it to live under build so we don't inadverently pollute /usr/local
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

set(CMAKE_MODULE_PATH
   ${CMAKE_MODULE_PATH}
   ${PROJECT_SOURCE_DIR}/cmake
   ${PROJECT_SOURCE_DIR}/cmake/make_dist
)

include(GNUInstallDirs)
include (ParseVersion)

if(BUILD_VERSION STREQUAL "0.0.0")
    if(EXISTS ${CMAKE_BINARY_DIR}/VERSION_CURRENT)
        file(STRINGS ${CMAKE_BINARY_DIR}/VERSION_CURRENT BUILD_VERSION)
    elseif(EXISTS ${PROJECT_SOURCE_DIR}/build/VERSION_CURRENT)
        file(STRINGS ${PROJECT_SOURCE_DIR}/build/VERSION_CURRENT BUILD_VERSION)
    else()
        find_package(PythonInterp)
        if(PYTHONINTERP_FOUND)
            execute_process(
                COMMAND ${PYTHON_EXECUTABLE} etc/calc_release_version.py
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                OUTPUT_VARIABLE CALC_RELEASE_VERSION
                RESULT_VARIABLE CALC_RELEASE_VERSION_RESULT
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(NOT CALC_RELEASE_VERSION_RESULT STREQUAL 0)
                # If python failed above, stderr would tell the user about it
                message(FATAL_ERROR
                   "BUILD_VERSION not specified and could not be calculated\
 (script invocation failed); specify in CMake command, -DBUILD_VERSION=<version>"
                )
            else()
                set(BUILD_VERSION ${CALC_RELEASE_VERSION})
                file(WRITE ${CMAKE_BINARY_DIR}/VERSION_CURRENT ${CALC_RELEASE_VERSION})
            endif()
        else()
            message(FATAL_ERROR
               "BUILD_VERSION not specified and could not be calculated\
 (Python was not found on the system); specify in CMake command, -DBUILD_VERSION=<version>"
            )
        endif()
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default is Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

unset (dist_generated CACHE)
unset (dist_generated_depends CACHE)

set (BUILD_SOURCE_DIR ${CMAKE_BINARY_DIR})

include (MakeDistFiles)

add_custom_target(hugo_dir
    COMMAND ${CMAKE_COMMAND} -E make_directory hugo
)

add_custom_target(hugo
    DEPENDS hugo_dir
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
    COMMAND hugo
    VERBATIM
)

add_custom_target(hugo-deploy
    DEPENDS hugo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND etc/deploy-to-ghpages.pl --hugo git@github.com:mongodb/mongo-cxx-driver
    VERBATIM
)

add_custom_target(docs_dir_current
    COMMAND ${CMAKE_COMMAND} -E make_directory docs/api/current
)

add_custom_target(doxygen-current
    DEPENDS docs_dir_current
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    VERBATIM
)

add_custom_target(doxygen-all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND etc/generate-all-apidocs.pl
    VERBATIM
)

add_custom_target(doxygen-deploy
    DEPENDS doxygen-all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND etc/deploy-to-ghpages.pl --doxygen git@github.com:mongodb/mongo-cxx-driver
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

add_custom_target(docs
    DEPENDS hugo doxygen-current
)

set(THIRD_PARTY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party)

enable_testing()

add_subdirectory(src)

add_subdirectory(examples EXCLUDE_FROM_ALL)

add_subdirectory(benchmark EXCLUDE_FROM_ALL)

# Implement 'dist' target
#
# CMake does not implement anything like 'dist' from autotools.
# This implementation is based on the one in GnuCash.

add_subdirectory (cmake)
add_subdirectory (data)
add_subdirectory (docs)
add_subdirectory (etc)

set (PACKAGE_PREFIX "mongo-cxx-driver-r${BUILD_VERSION}")
set (DIST_FILE "${PACKAGE_PREFIX}.tar.gz")

set (top_DIST_local
   CMakeLists.txt
   CONTRIBUTING.md
   CREDITS.json
   LICENSE
   README.md
   THIRD-PARTY-NOTICES
   build/.gitignore
   build/VERSION_CURRENT
   # This sub-directory is added later, so manually include here
   generate_uninstall/CMakeLists.txt
)

set_local_dist (top_DIST ${top_DIST_local})

set (ALL_DIST
   ${top_DIST}
   ${benchmark_DIST}
   ${cmake_DIST}
   ${docs_DIST}
   ${data_DIST}
   ${etc_DIST}
   ${examples_DIST}
   ${src_DIST}
)

# Write a dist manifest
string (REPLACE ";" "\n" ALL_DIST_LINES "${ALL_DIST}")
file (WRITE ${CMAKE_BINARY_DIR}/dist_manifest.txt ${ALL_DIST_LINES})

install (FILES LICENSE README.md THIRD-PARTY-NOTICES
   DESTINATION ${CMAKE_INSTALL_DATADIR}/mongo-cxx-driver
)

# This is the command that produces the distribution tarball
add_custom_command (OUTPUT ${DIST_FILE}
   COMMAND ${CMAKE_COMMAND}
      -D CMAKE_MODULE_PATH=${PROJECT_SOURCE_DIR}/cmake/make_dist
      -D PACKAGE_PREFIX=${PACKAGE_PREFIX}
      -D MONGOCXX_SOURCE_DIR=${CMAKE_SOURCE_DIR}
      -D BUILD_SOURCE_DIR=${BUILD_SOURCE_DIR}
      -D SHELL=${SHELL}
      "-Ddist_generated=\"${dist_generated}\""
      -P ${PROJECT_SOURCE_DIR}/cmake/make_dist/MakeDist.cmake

   DEPENDS
      ${ALL_DIST} ${dist_generated_depends}
)

add_custom_target (dist DEPENDS ${DIST_FILE})

add_custom_target (distcheck DEPENDS dist
   COMMAND ${CMAKE_COMMAND}
      -D CMAKE_MODULE_PATH=${PROJECT_SOURCE_DIR}/cmake/make_dist
      -D CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
      -D PACKAGE_PREFIX=${PACKAGE_PREFIX}
      -D CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -P ${PROJECT_SOURCE_DIR}/cmake/make_dist/MakeDistCheck.cmake
)

if(ENABLE_UNINSTALL)
    if(WIN32)
        set(UNINSTALL_PROG "uninstall.cmd")
    else()
        set(UNINSTALL_PROG "uninstall.sh")
    endif()
    set(UNINSTALL_PROG_DIR "${CMAKE_INSTALL_DATADIR}/mongo-cxx-driver")

    # Create uninstall program and associated uninstall target
    #
    # This needs to be last (after all other add_subdirectory calls) to ensure
    # that the generated uninstall program is complete and correct
    add_subdirectory(generate_uninstall)
endif()

# Spit out some information regarding the generated build system
message (STATUS "Build files generated for:")
message (STATUS "\tbuild system: ${CMAKE_GENERATOR}")
if (CMAKE_GENERATOR_INSTANCE)
   message (STATUS "\tinstance: ${CMAKE_GENERATOR_INSTANCE}")
endif ()
if (CMAKE_GENERATOR_PLATFORM)
   message (STATUS "\tinstance: ${CMAKE_GENERATOR_PLATFORM}")
endif ()
if (CMAKE_GENERATOR_TOOLSET)
   message (STATUS "\tinstance: ${CMAKE_GENERATOR_TOOLSET}")
endif ()

