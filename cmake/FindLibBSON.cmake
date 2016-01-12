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

# Find libbson, either via pkg-config, find-package in config mode,
# or other less admirable jiggery-pokery

SET(LIBBSON_DIR "" CACHE STRING "Manual search path for libbson")

include(FindPackageHandleStandardArgs)

# Load up PkgConfig if we have it
find_package(PkgConfig QUIET)

if (PKG_CONFIG_FOUND)
  pkg_check_modules(LIBBSON REQUIRED libbson-1.0>=${LibBSON_FIND_VERSION} )
  # We don't reiterate the version information here because we assume that
  # pkg_check_modules has honored our request.
  find_package_handle_standard_args(LIBBSON DEFAULT_MSG LIBBSON_FOUND)
elseif(LIBBSON_DIR)
  # The best we can do until libbson starts installing a libbson-config.cmake file
  set(LIBBSON_LIBRARIES bson-1.0 CACHE INTERNAL "")
  set(LIBBSON_LIBRARY_DIRS ${LIBBSON_DIR}/lib CACHE INTERNAL "")
  set(LIBBSON_INCLUDE_DIRS ${LIBBSON_DIR}/include/libbson-1.0 CACHE INTERNAL "")
  find_package_handle_standard_args(LIBBSON DEFAULT_MSG LIBBSON_LIBRARIES LIBBSON_LIBRARY_DIRS LIBBSON_INCLUDE_DIRS)
else()
  message(FATAL_ERROR "Don't know how to find libbson; please set LIBBSON_DIR to the prefix directory with which libbson was configured.")
endif()
