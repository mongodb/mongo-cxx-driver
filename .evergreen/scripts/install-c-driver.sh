#!/usr/bin/env bash

set -o errexit
set -o pipefail

declare -r mongoc_version="${mongoc_version:-"${mongoc_version_minimum:?"missing mongoc version"}"}"
: "${mongoc_version:?}"

declare mongoc_dir
mongoc_dir="$(pwd)/mongoc"

# "i" for "(platform-)independent".
declare mongoc_idir mongoc_install_idir
if [[ "${OSTYPE:?}" == "cygwin" ]]; then
  # CMake requires Windows paths for configuration variables on Windows.
  mongoc_idir="$(cygpath -aw "${mongoc_dir}")"
  mongoc_install_idir="$(cygpath -aw "${mongoc_dir}")"
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

. mongo-cxx-driver/.evergreen/scripts/install-build-tools.sh
install_build_tools

# Default CMake generator to use if not already provided.
declare CMAKE_GENERATOR CMAKE_GENERATOR_PLATFORM
case "${OSTYPE:?}" in
cygwin)
  if [[ "${generator:-}" == Visual\ Studio\ * ]]; then
    # MSBuild task-based parallelism (VS 2019 16.3 and newer).
    export UseMultiToolTask=true
    export EnforceProcessCountAcrossBuilds=true
    # MSBuild inter-project parallelism via CMake (3.26 and newer).
    export CMAKE_BUILD_PARALLEL_LEVEL
    CMAKE_BUILD_PARALLEL_LEVEL="$(nproc)" # /maxcpucount

    CMAKE_GENERATOR="${generator:?}"
    CMAKE_GENERATOR_PLATFORM="${platform:-"x64"}"
  else
    : "${generator:="Ninja Multi-Config"}"
    PATH="/cygdrive/c/ProgramData/chocolatey/lib/winlibs/tools/mingw64/bin:${PATH:-}" # mingw-w64 GCC
  fi
  ;;

darwin* | linux*)
  : "${generator:="Ninja"}"
  ;;

*)
  echo "unrecognized operating system ${OSTYPE:?}" 1>&2
  exit 1
  ;;
esac
export CMAKE_GENERATOR="${generator:?}"
export CMAKE_GENERATOR_PLATFORM="${platform:-}"

# Install libmongocrypt.
if [[ "${SKIP_INSTALL_LIBMONGOCRYPT:-}" != "1" ]]; then
  echo "Installing libmongocrypt into ${mongoc_install_idir}..."

  # Avoid using compile-libmongocrypt.sh (mongo-c-driver) -> compile.sh (libmongocrypt) -> build_all.sh (libmongocrypt),
  # which hardcodes MSVC-specific compiler flags (-EHsc) and does not support the Ninja Multi-Config generator (see:
  # references to the USE_NINJA environment variable).
  if [[ "${OSTYPE:?}" == cygwin && "${CMAKE_GENERATOR:?}" != Visual\ Studio\ * ]]; then
    (
      git clone -q --revision=6528eb5cffdf278ec21da952ba2324cc5e2517ac https://github.com/mongodb/libmongocrypt # 1.17.1 or 1.18.0 when released.

      declare -a crypt_cmake_flags=(
        "-DMONGOCRYPT_MONGOC_DIR=${mongoc_idir:?}"
        "-DBUILD_TESTING=OFF"
        "-DENABLE_ONLINE_TESTS=OFF"
        "-DENABLE_MONGOC=OFF"
        "-DBUILD_VERSION=1.18.0-dev"
      )

      . "${mongoc_dir}/.evergreen/scripts/find-ccache.sh"
      find_ccache_and_export_vars "$(pwd)/libmongocrypt" || true
      if command -v "${CMAKE_C_COMPILER_LAUNCHER:-}" && [[ "${OSTYPE:?}" == cygwin && "${generator:-}" == Visual\ Studio\ * ]]; then
        crypt_cmake_flags+=(
          "-DCMAKE_POLICY_DEFAULT_CMP0141=NEW"
          "-DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded"
        )
      fi

      # build_all.sh (libmongocrypt)
      cmake \
        "${crypt_cmake_flags[@]:?}" \
        -D CMAKE_INSTALL_PREFIX="${mongoc_install_idir:?}" \
        -D CMAKE_BUILD_TYPE=RelWithDebInfo \
        -S libmongocrypt \
        -B libmongocrypt/cmake-build
      cmake --build libmongocrypt/cmake-build --config RelWithDebInfo --target install
    ) &>output.txt || {
      cat output.txt >&2
      exit 1
    }
  else
    "${mongoc_dir}/.evergreen/scripts/compile-libmongocrypt.sh" "$(command -v cmake)" "${mongoc_idir}" "${mongoc_install_idir}"
  fi

  echo "Installing libmongocrypt into ${mongoc_install_idir}... done."
fi

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
  if [[ "${generator:-}" == Visual\ Studio\ * ]]; then
    # Replace `/Zi`, which is incompatible with ccache, with `/Z7` while preserving other default debug flags.
    configure_flags+=(
      "-DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded"
    )
  else
    # Avoid mingw-w64 linker issues (hangs indefinitely?).
    configure_flags+=(
      "-DMONGO_USE_LLD=OFF"
    )
  fi
  ;;
darwin*)
  configure_flags+=("-DCMAKE_C_FLAGS=-fPIC")
  configure_flags+=("-DCMAKE_MACOSX_RPATH=ON")
  ;;
*)
  configure_flags+=("-DCMAKE_C_FLAGS=-fPIC")
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
  cmake -S "${mongoc_idir}" -B "${mongoc_idir}" "${configure_flags[@]}"
  cmake --build "${mongoc_idir}" --config Debug --target install -- "${compile_flags[@]}"
  echo "Installing C Driver into ${mongoc_dir}... done." 1>&2
} >/dev/null
