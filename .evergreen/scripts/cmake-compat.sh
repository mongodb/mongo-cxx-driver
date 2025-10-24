#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${CMAKE_VERSION:?}"
: "${INSTALL_C_DRIVER:?}"

[[ -d ../mongoc ]] || {
  echo "missing mongoc directory"
  exit 1
} >&2

mongoc_prefix="$(cd ../mongoc && pwd)"
if [[ "${OSTYPE:?}" =~ cygwin ]]; then
  mongoc_prefix="$(cygpath -m "${mongoc_prefix:?}")"
fi

. .evergreen/scripts/install-build-tools.sh
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
  "-DCMAKE_INSTALL_PREFIX=install"
  "-DCMAKE_FIND_NO_INSTALL_PREFIX=ON"
)

if [[ "${INSTALL_C_DRIVER:?}" == 1 ]]; then
  # Use find_package().
  cmake_flags+=(
    "-DCMAKE_PREFIX_PATH=${mongoc_prefix:?}"
  )
else
  # Use add_subdirectory().
  cmake_flags+=(
    "-DCMAKE_DISABLE_FIND_PACKAGE_bson=ON"
    "-DCMAKE_DISABLE_FIND_PACKAGE_mongoc=ON"
  )
fi

echo "Configuring with CMake flags:"
printf " - %s\n" "${cmake_flags[@]:?}"

cmake -S . -B build "${cmake_flags[@]:?}"
cmake --build build --target install

# Use header bson.h to detect installation of C Driver libraries.
bson_h="$(find install -name 'bson.h')"
if [[ "${INSTALL_C_DRIVER:?}" == 1 ]] && [[ -n "${bson_h:-}" ]]; then
  echo "bson.h SHOULD NOT be present in $(pwd)/install" >&2
  exit 1
fi
if [[ "${INSTALL_C_DRIVER:?}" != 1 ]] && [[ -z "${bson_h:-}" ]]; then
  echo "bson.h SHOULD be present in $(pwd)/install" >&2
  exit 1
fi
