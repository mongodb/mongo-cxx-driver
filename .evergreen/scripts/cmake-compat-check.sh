#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${CMAKE_VERSION:?}"
: "${INSTALL_C_DRIVER:?}"
: "${UV_INSTALL_DIR:?}"

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

. mongo-cxx-driver/.evergreen/scripts/install-build-tools.sh
install_build_tools

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

# Support C Driver libraries obtained via both add_subdirectory() and find_package().
if [[ "${INSTALL_C_DRIVER:?}" == 1 ]]; then
  # Different install prefixes.
  cmake_flags+=("-DCMAKE_PREFIX_PATH=${mongocxx_prefix:?};${mongoc_prefix:?}")
else
  # Same install prefix.
  cmake_flags+=("-DCMAKE_PREFIX_PATH=${mongocxx_prefix:?}")
fi

echo "Configuring with CMake flags:"
printf " - %s\n" "${cmake_flags[@]:?}"

# Test importing C++ Driver libraries using find_package().
echo "Importing C++ Driver via find_package()..."
{
  cat >|CMakeLists.txt <<DOC
cmake_minimum_required(VERSION 3.15...4.0)
project(cmake-compat)

find_package(mongocxx REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE mongo::mongocxx_shared) # + mongo::bsoncxx_shared
DOC

  cmake -S . -B build-find "${cmake_flags[@]:?}" &&
    cmake --build build-find --target main &&
    ./build-find/main
} &>output.txt || {
  cat output.txt >&2
  exit 1
}
echo "Importing C++ Driver via find_package()... done."

echo "Importing C++ Driver via add_subdirectory()..."
{
  cat >|CMakeLists.txt <<DOC
cmake_minimum_required(VERSION 3.15...4.0)
project(cmake-compat)

set(ENABLE_EXAMPLES OFF)
set(ENABLE_TESTS OFF)

add_subdirectory(mongoc)
add_subdirectory(mongo-cxx-driver)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE mongocxx_shared) # + bsoncxx_shared
DOC

  cmake -S . -B build-add "${cmake_flags[@]:?}" &&
    cmake --build build-add --target main &&
    ./build-add/main
} &>output.txt || {
  cat output.txt >&2
  exit 1
}
echo "Importing C++ Driver via add_subdirectory()... done."
