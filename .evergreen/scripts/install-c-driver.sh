#!/usr/bin/env bash

set -o errexit
set -o pipefail

declare -r mongoc_version="${mongoc_version:-"${mongoc_version_minimum:?"missing mongoc version"}"}"
: "${mongoc_version:?}"

# Usage:
#   to_windows_path "./some/unix/style/path"
#   to_windows_path "/some/unix/style/path"
to_windows_path() {
  cygpath -aw "${1:?"to_windows_path requires a path to convert"}"
}

declare mongoc_dir
mongoc_dir="$(pwd)/mongoc"

# "i" for "(platform-)independent".
declare mongoc_idir mongoc_install_idir
if [[ "${OSTYPE:?}" == "cygwin" ]]; then
  # CMake requires Windows paths for configuration variables on Windows.
  mongoc_idir="$(to_windows_path "${mongoc_dir}")"
  mongoc_install_idir="$(to_windows_path "${mongoc_dir}")"
else
  mongoc_idir="${mongoc_dir}"
  mongoc_install_idir="${mongoc_dir}"
fi
: "${mongoc_idir:?}"
: "${mongoc_install_idir:?}"

echo "mongoc version: ${mongoc_version}"

# Download tarball from GitHub and extract into ${mongoc_dir}.
rm -rf "${mongoc_dir}"
mkdir "${mongoc_dir}"
curl -sS -o mongo-c-driver.tar.gz -L "https://api.github.com/repos/mongodb/mongo-c-driver/tarball/${mongoc_version}"
tar xzf mongo-c-driver.tar.gz --directory "${mongoc_dir}" --strip-components=1

# shellcheck source=/dev/null
. "${mongoc_dir}/.evergreen/scripts/find-cmake-latest.sh"
declare cmake_binary
cmake_binary="$(find_cmake_latest)"
command -v "${cmake_binary:?}"

# Install libmongocrypt.
if [[ "${SKIP_INSTALL_LIBMONGOCRYPT:-}" != "1" ]]; then
  {
    echo "Installing libmongocrypt into ${mongoc_dir}..." 1>&2
    "${mongoc_dir}/.evergreen/scripts/compile-libmongocrypt.sh" "${cmake_binary}" "${mongoc_idir}" "${mongoc_install_idir}"
    echo "Installing libmongocrypt into ${mongoc_dir}... done." 1>&2
  } >/dev/null
fi

if [[ "${OSTYPE}" == darwin* ]]; then
  # MacOS does not have nproc.
  nproc() {
    sysctl -n hw.logicalcpu
  }
fi

# Default CMake generator to use if not already provided.
declare CMAKE_GENERATOR CMAKE_GENERATOR_PLATFORM
if [[ "${OSTYPE:?}" == "cygwin" ]]; then
  CMAKE_GENERATOR="${generator:-"Visual Studio 14 2015"}"
  CMAKE_GENERATOR_PLATFORM="${platform:-"x64"}"
else
  CMAKE_GENERATOR="${generator:-"Unix Makefiles"}"
  CMAKE_GENERATOR_PLATFORM="${platform:-""}"
fi
export CMAKE_GENERATOR CMAKE_GENERATOR_PLATFORM

declare -a configure_flags=(
  "-DCMAKE_BUILD_TYPE=Debug"
  "-DCMAKE_INSTALL_PREFIX=${mongoc_install_idir}"
  "-DCMAKE_PREFIX_PATH=${mongoc_idir}"
  "-DENABLE_EXAMPLES=OFF"
  "-DENABLE_SHM_COUNTERS=OFF"
  "-DENABLE_STATIC=ON"
)

if [[ "${SKIP_INSTALL_LIBMONGOCRYPT:-}" != "1" ]]; then
  configure_flags+=("-DENABLE_CLIENT_SIDE_ENCRYPTION=ON")
fi

declare -a compile_flags

case "${OSTYPE:?}" in
cygwin)
  compile_flags+=("/maxcpucount:$(nproc)")

  # Replace `/Zi`, which is incompatible with ccache, with `/Z7` while preserving other default debug flags.
  cmake_flags+=(
    "-DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded"
  )
  ;;
darwin*)
  configure_flags+=("-DCMAKE_C_FLAGS=-fPIC")
  configure_flags+=("-DCMAKE_MACOSX_RPATH=ON")
  compile_flags+=("-j" "$(nproc)")
  ;;
*)
  configure_flags+=("-DCMAKE_C_FLAGS=-fPIC")
  compile_flags+=("-j" "$(nproc)")
  ;;
esac

# Use ccache if available.
if [[ -f "${mongoc_dir:?}/.evergreen/scripts/find-ccache.sh" ]]; then
  # shellcheck source=/dev/null
  . "${mongoc_dir:?}/.evergreen/scripts/find-ccache.sh"
  find_ccache_and_export_vars "$(pwd)" || true
fi

# Install C Driver libraries.
{
  echo "Installing C Driver into ${mongoc_dir}..." 1>&2
  "${cmake_binary}" -S "${mongoc_idir}" -B "${mongoc_idir}" "${configure_flags[@]}"
  "${cmake_binary}" --build "${mongoc_idir}" --config Debug --target install -- "${compile_flags[@]}"
  echo "Installing C Driver into ${mongoc_dir}... done." 1>&2
} >/dev/null
