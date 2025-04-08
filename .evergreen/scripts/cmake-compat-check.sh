#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${CMAKE_MAJOR_VERSION:?}"
: "${CMAKE_MINOR_VERSION:?}"
: "${CMAKE_PATCH_VERSION:?}"
: "${INSTALL_C_DRIVER:?}"
: "${cxx_standard:?}"
: "${find_c_driver:?}"

[[ -d mongoc ]] || {
  echo "missing mongoc directory"
  exit 1
} >&2

[[ -d mongo-cxx-driver/install ]] || {
  echo "missing mongo-cxx-driver install directory"
  exit 1
} >&2

mongoc_prefix="$(pwd)/mongoc"
mongocxx_prefix="$(pwd)/mongo-cxx-driver/install"
if [[ "${OSTYPE:?}" =~ cygwin ]]; then
  mongoc_prefix="$(cygpath -m "${mongoc_prefix:?}")"
  mongocxx_prefix="$(cygpath -m "${mongocxx_prefix:?}")"
fi

# shellcheck source=/dev/null
. "${mongoc_prefix:?}/.evergreen/scripts/find-cmake-version.sh"
export cmake_binary
cmake_binary="$(find_cmake_version "${CMAKE_MAJOR_VERSION:?}" "${CMAKE_MINOR_VERSION:?}" "${CMAKE_PATCH_VERSION:?}")"
"${cmake_binary:?}" --version

CMAKE_BUILD_PARALLEL_LEVEL="$(nproc)"
export CMAKE_BUILD_PARALLEL_LEVEL

# Use ccache if available.
if [[ -f "${mongoc_prefix:?}/.evergreen/scripts/find-ccache.sh" ]]; then
  # shellcheck source=/dev/null
  . "${mongoc_prefix:?}/.evergreen/scripts/find-ccache.sh"
  find_ccache_and_export_vars "$(pwd)" || true
fi

cmake_flags=(
  "-Werror=dev"
  "-Werror=deprecated"
  "-DCMAKE_BUILD_TYPE=Debug"
  "-DCMAKE_FIND_NO_INSTALL_PREFIX=ON"
  "-DCMAKE_CXX_STANDARD=${cxx_standard:?}"
)

cat >main.cpp <<DOC
#include <bsoncxx/config/version.hpp>
#include <mongocxx/config/version.hpp>

#include <mongocxx/instance.hpp>

#include <iostream>

int main() {
    mongocxx::instance instance;

    std::cout << "bsoncxx:  " << BSONCXX_VERSION_STRING << std::endl;
    std::cout << "mongocxx: " << MONGOCXX_VERSION_STRING << std::endl;
}
DOC

if [[ "${find_c_driver:?}" == 1 ]]; then
  # Test importing C++ Driver libraries using find_package().

  if [[ "${INSTALL_C_DRIVER:?}" == 1 ]]; then
    # Different install prefixes.
    cmake_flags+=("-DCMAKE_PREFIX_PATH=${mongocxx_prefix:?};${mongoc_prefix:?}")
  else
    # Same install prefix.
    cmake_flags+=("-DCMAKE_PREFIX_PATH=${mongocxx_prefix:?}")
  fi

  cat >CMakeLists.txt <<DOC
cmake_minimum_required(VERSION ${CMAKE_MAJOR_VERSION:?}.${CMAKE_MINOR_VERSION:?}.${CMAKE_PATCH_VERSION:?})
project(cmake-compat)

find_package(mongocxx REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE mongo::mongocxx_shared) # + mongo::bsoncxx_shared
DOC
else
  # Test importing C++ Driver libraries using add_subdirectory().
  cat >CMakeLists.txt <<DOC
cmake_minimum_required(VERSION ${CMAKE_MAJOR_VERSION:?}.${CMAKE_MINOR_VERSION:?}.${CMAKE_PATCH_VERSION:?})
project(cmake-compat)

add_subdirectory(mongoc)
add_subdirectory(mongo-cxx-driver)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE mongocxx_shared) # + bsoncxx_shared
DOC
fi

echo "Configuring with CMake flags:"
printf " - %s\n" "${cmake_flags[@]:?}"

"${cmake_binary:?}" -S . -B build "${cmake_flags[@]:?}"
"${cmake_binary:?}" --build build --target main
./build/main
