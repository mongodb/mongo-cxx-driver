#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${CMAKE_VERSION:?}"
: "${INSTALL_C_DRIVER:?}"
: "${UV_INSTALL_DIR:?}"

[[ -d ../mongoc ]] || {
  echo "missing mongoc directory"
  exit 1
} >&2

mongoc_prefix="$(cd ../mongoc && pwd)"
if [[ "${OSTYPE:?}" =~ cygwin ]]; then
  mongoc_prefix="$(cygpath -m "${mongoc_prefix:?}")"
fi

# Obtain preferred build tools.
export UV_TOOL_DIR UV_TOOL_BIN_DIR
if [[ "${OSTYPE:?}" == cygwin ]]; then
  UV_TOOL_DIR="$(cygpath -aw "$(pwd)/uv-tool")"
  UV_TOOL_BIN_DIR="$(cygpath -aw "$(pwd)/uv-bin")"
else
  UV_TOOL_DIR="$(pwd)/uv-tool"
  UV_TOOL_BIN_DIR="$(pwd)/uv-bin"
fi
PATH="${UV_TOOL_BIN_DIR:?}:${UV_INSTALL_DIR:?}:${PATH:-}"
uv tool install -q cmake
[[ "${distro_id:?}" == rhel* ]] && PATH="${PATH:-}:/opt/mongodbtoolchain/v4/bin" || uv tool install -q ninja
export CMAKE_GENERATOR
CMAKE_GENERATOR="Ninja"

cmake --version | head -n 1
echo "ninja version: $(ninja --version)"

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

# Use generated header bson-config.h to detect installation of C Driver libraries.
bson_config_h="$(find install -name 'bson-config.h')"
if [[ "${INSTALL_C_DRIVER:?}" == 1 ]] && [[ -n "${bson_config_h:-}" ]]; then
  echo "bson-config.h SHOULD NOT be present in $(pwd)/install" >&2
  exit 1
fi
if [[ "${INSTALL_C_DRIVER:?}" != 1 ]] && [[ -z "${bson_config_h:-}" ]]; then
  echo "bson-config.h SHOULD be present in $(pwd)/install" >&2
  exit 1
fi
