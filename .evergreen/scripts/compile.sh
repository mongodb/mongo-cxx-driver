#!/usr/bin/env bash

# Runs cmake and compiles the standard build targets (all, install, examples).  Any arguments passed
# to this script will be forwarded on as flags passed to cmake.
#
# This script should be run from the root of the repository.  This script will run the build from
# the default build directory './build'.  The following environment variables will change the
# behavior of this script:
# - build_type: must be set to "Release" or "Debug"

set -o errexit
set -o pipefail

: "${branch_name:?}"
: "${build_type:?}"
: "${distro_id:?}" # Required by find-cmake-latest.sh.

: "${BSON_EXTRA_ALIGNMENT:-}"
: "${BSONCXX_POLYFILL:-}"
: "${COMPILE_MACRO_GUARD_TESTS:-}"
: "${ENABLE_CODE_COVERAGE:-}"
: "${ENABLE_TESTS:-}"
: "${generator:-}"
: "${platform:-}"
: "${REQUIRED_CXX_STANDARD:-}"
: "${RUN_DISTCHECK:-}"
: "${USE_SANITIZER_ASAN:-}"
: "${USE_SANITIZER_UBSAN:-}"
: "${USE_STATIC_LIBS:-}"

mongoc_prefix="$(pwd)/../mongoc"
echo "mongoc_prefix=${mongoc_prefix:?}"

if [[ "${OSTYPE:?}" =~ cygwin ]]; then
  mongoc_prefix=$(cygpath -m "${mongoc_prefix:?}")
fi

# shellcheck source=/dev/null
. "${mongoc_prefix:?}/.evergreen/scripts/find-cmake-latest.sh"
export cmake_binary
cmake_binary="$(find_cmake_latest)"
command -v "$cmake_binary"

if [ ! -d ../drivers-evergreen-tools ]; then
  git clone --depth 1 https://github.com/mongodb-labs/drivers-evergreen-tools.git ../drivers-evergreen-tools
fi
# shellcheck source=/dev/null
. ../drivers-evergreen-tools/.evergreen/find-python3.sh
# shellcheck source=/dev/null
. ../drivers-evergreen-tools/.evergreen/venv-utils.sh

venvcreate "$(find_python3)" venv
python -m pip install GitPython

if [[ "${build_type:?}" != "Debug" && "${build_type:?}" != "Release" ]]; then
  echo "$0: expected build_type environment variable to be set to 'Debug' or 'Release'" >&2
  exit 1
fi

if [[ "${OSTYPE}" == darwin* ]]; then
  # MacOS does not have nproc.
  nproc() {
    sysctl -n hw.logicalcpu
  }
fi
CMAKE_BUILD_PARALLEL_LEVEL="$(nproc)"
export CMAKE_BUILD_PARALLEL_LEVEL

# Use ccache if available.
if [[ -f "${mongoc_prefix:?}/.evergreen/scripts/find-ccache.sh" ]]; then
  # shellcheck source=/dev/null
  . "${mongoc_prefix:?}/.evergreen/scripts/find-ccache.sh"
  find_ccache_and_export_vars "$(pwd)" || true
fi

build_targets=()
cmake_build_opts=()
case "${OSTYPE:?}" in
cygwin)
  cmake_build_opts+=("/verbosity:minimal")
  build_targets+=(--target ALL_BUILD --target examples/examples)
  ;;

darwin* | linux*)
  build_targets+=(--target all --target examples)
  ;;

*)
  echo "unrecognized operating system ${OSTYPE:?}" 1>&2
  exit 1
  ;;
esac

# Create a VERSION_CURRENT file in the build directory to include in the dist tarball.
python ./etc/calc_release_version.py >./build/VERSION_CURRENT
cd build

cmake_flags=(
  "-DCMAKE_BUILD_TYPE=${build_type:?}"
  "-DCMAKE_PREFIX_PATH=${mongoc_prefix:?}"
  -DMONGOCXX_ENABLE_SLOW_TESTS=ON
  -DCMAKE_INSTALL_PREFIX=install
  -DENABLE_UNINSTALL=ON
)

# System-installed libmongoc must not prevent fetch-and-build of libmongoc.
if [[ -z "$(find "${mongoc_prefix:?}" -name 'bson-config.h')" ]]; then
  cmake_flags+=("-DCMAKE_DISABLE_FIND_PACKAGE_mongoc-1.0=ON")
fi

_RUN_DISTCHECK=""
case "${OSTYPE:?}" in
cygwin)
  case "${generator:-}" in
  *2015* | *2017* | *2019* | *2022*) ;;
  *)
    echo "missing explicit CMake Generator on Windows distro" 1>&2
    exit 1
    ;;
  esac
  ;;
darwin* | linux*)
  : "${generator:="Unix Makefiles"}"

  # If enabled, limit distcheck to Unix-like systems only.
  _RUN_DISTCHECK="${RUN_DISTCHECK:-}"
  ;;
*)
  echo "unrecognized operating system ${OSTYPE:?}" 1>&2
  exit 1
  ;;
esac
export CMAKE_GENERATOR="${generator:?}"
export CMAKE_GENERATOR_PLATFORM="${platform:-}"

case "${BSONCXX_POLYFILL:-}" in
impls) cmake_flags+=("-DBSONCXX_POLY_USE_IMPLS=ON") ;;
std) cmake_flags+=("-DBSONCXX_POLY_USE_STD=ON") ;;
*) ;;
esac

cc_flags_init=(-Wall -Wextra -Wno-attributes -Werror -Wno-missing-field-initializers)
cxx_flags_init=(-Wall -Wextra -Wconversion -Wnarrowing -pedantic -Werror)
cc_flags=()
cxx_flags=()

case "${OSTYPE:?}" in
cygwin)
  # Most compiler flags are not applicable to builds on Windows distros.
  ;;
darwin*)
  cc_flags+=("${cc_flags_init[@]}")
  cxx_flags+=("${cxx_flags_init[@]}" -stdlib=libc++)

  if [[ "${distro_id:?}" == macos-14* ]]; then
    cxx_flags+=(-Wno-align-mismatch)
  fi
  ;;
linux*)
  cc_flags+=("${cc_flags_init[@]}")
  cxx_flags+=("${cxx_flags_init[@]}" -Wno-missing-field-initializers)

  if [[ "${CXX:-}" != "clang++" ]]; then
    cxx_flags+=(-Wno-aligned-new)
  fi

  if [[ "${distro_id:?}" != rhel7* ]]; then
    cxx_flags+=("-Wno-expansion-to-defined")
  else
    cc_flags+=("-Wno-maybe-uninitialized") # Ignore false-positive warning in C driver build.
  fi

  if [[ "${distro_id:?}" == debian12* ]]; then
    # Disable `restrict` warning on GCC 12 due to  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105651 and https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105329
    cxx_flags+=("-Wno-error=restrict")
  fi
  ;;
*)
  echo "unrecognized operating system ${OSTYPE:?}" 1>&2
  exit 1
  ;;
esac

# Most compiler flags are not applicable to builds on Windows distros.
if [[ "${OSTYPE:?}" != cygwin ]]; then
  # Sanitizers overwrite the usual compiler flags.
  if [[ "${USE_SANITIZER_ASAN:-}" == "ON" ]]; then
    cxx_flags=(
      "${cxx_flags_init[@]}"
      -D_GLIBCXX_USE_CXX11_ABI=0
      -fsanitize=address
      -O1 -g -fno-omit-frame-pointer
    )
  fi

  # Sanitizers overwrite the usual compiler flags.
  if [[ "${USE_SANITIZER_UBSAN:-}" == "ON" ]]; then
    cxx_flags=(
      "${cxx_flags_init[@]}"
      -D_GLIBCXX_USE_CXX11_ABI=0
      -fsanitize=undefined
      -fsanitize-blacklist="$(pwd)/../etc/ubsan.ignorelist"
      -fno-sanitize-recover=undefined
      -O1 -g -fno-omit-frame-pointer
    )
  fi

  # Ignore deprecation warnings when building on a release branch.
  if [[ "$(echo "${branch_name:?}" | cut -f2 -d'/')" != "${branch_name:?}" ]]; then
    cc_flags+=(-Wno-deprecated-declarations)
    cxx_flags+=(-Wno-deprecated-declarations)
  fi
fi

if [[ "${#cc_flags[@]}" -gt 0 ]]; then
  cmake_flags+=("-DCMAKE_C_FLAGS=${cc_flags[*]}")
fi

if [[ "${#cxx_flags[@]}" -gt 0 ]]; then
  cmake_flags+=("-DCMAKE_CXX_FLAGS=${cxx_flags[*]}")
fi

if [[ "${ENABLE_CODE_COVERAGE:-}" == "ON" ]]; then
  cmake_flags+=("-DENABLE_CODE_COVERAGE=ON")
fi

if [ "${USE_STATIC_LIBS:-}" ]; then
  cmake_flags+=("-DBUILD_SHARED_LIBS=OFF")
fi

if [ "${ENABLE_TESTS:-}" = "ON" ]; then
  cmake_flags+=(
    "-DENABLE_TESTS=ON"
    "-DBUILD_TESTING=ON"
  )
fi

if [[ -n "${REQUIRED_CXX_STANDARD:-}" ]]; then
  cmake_flags+=("-DCMAKE_CXX_STANDARD=${REQUIRED_CXX_STANDARD:?}")
  cmake_flags+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
fi

if [[ "${COMPILE_MACRO_GUARD_TESTS:-"OFF"}" == "ON" ]]; then
  cmake_flags+=("-DENABLE_MACRO_GUARD_TESTS=ON")
fi

echo "Configuring with CMake flags:"
printf " - %s\n" "${cmake_flags[@]}"

"${cmake_binary}" "${cmake_flags[@]}" ..

if [[ "${COMPILE_MACRO_GUARD_TESTS:-"OFF"}" == "ON" ]]; then
  # We only need to compile the macro guard tests.
  "${cmake_binary}" --build . --config "${build_type:?}" --target test_bsoncxx_macro_guards test_mongocxx_macro_guards -- "${cmake_build_opts[@]}"
  exit # Nothing else to be done.
fi

# Regular build and install routine.
"${cmake_binary}" --build . --config "${build_type:?}" "${build_targets[@]:?}" -- "${cmake_build_opts[@]}"
"${cmake_binary}" --install . --config "${build_type:?}"

if [[ "${_RUN_DISTCHECK:-}" ]]; then
  "${cmake_binary}" --build . --config "${build_type:?}" --target distcheck
fi

# Ensure extra alignment is enabled or disabled as expected.
if [[ -n "$(find "${mongoc_prefix:?}" -name 'bson-config.h')" ]]; then
  if [[ "${BSON_EXTRA_ALIGNMENT:-}" == "1" ]]; then
    grep -R "#define BSON_EXTRA_ALIGN 1" "${mongoc_prefix:?}" || {
      echo "BSON_EXTRA_ALIGN is not 1 despite BSON_EXTRA_ALIGNMENT=1" 1>&2
      exit 1
    }
  else
    grep -R "#define BSON_EXTRA_ALIGN 0" "${mongoc_prefix:?}" || {
      echo "BSON_EXTRA_ALIGN is not 0 despite BSON_EXTRA_ALIGNMENT=0" 1>&2
      exit 1
    }
  fi
elif [[ -n "$(find install -name 'bson-config.h')" ]]; then
  if [[ "${BSON_EXTRA_ALIGNMENT:-}" == "1" ]]; then
    grep -R "#define BSON_EXTRA_ALIGN 1" install || {
      echo "BSON_EXTRA_ALIGN is not 1 despite BSON_EXTRA_ALIGNMENT=1" 1>&2
      exit 1
    }
  else
    grep -R "#define BSON_EXTRA_ALIGN 0" install || {
      echo "BSON_EXTRA_ALIGN is not 0 despite BSON_EXTRA_ALIGNMENT=0" 1>&2
      exit 1
    }
  fi
else
  echo "unexpectedly compiled using a system libmongoc library" 1>&2
  exit 1
fi
