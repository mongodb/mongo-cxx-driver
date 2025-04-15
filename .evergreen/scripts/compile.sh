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
: "${USE_SHARED_AND_STATIC_LIBS:-}"

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
PATH="${UV_INSTALL_DIR:?}:${PATH:-}" uv run --frozen python ./etc/calc_release_version.py >./build/VERSION_CURRENT
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

if [[ -n "${REQUIRED_CXX_STANDARD:-}" ]]; then
  echo "Checking requested C++ standard is supported..."
  pushd "$(mktemp -d)"
  cat >CMakeLists.txt <<DOC
cmake_minimum_required(VERSION 3.30)
project(cxx_standard_latest LANGUAGES CXX)
set(cxx_std_version "${REQUIRED_CXX_STANDARD:?}")
if(cxx_std_version STREQUAL "latest") # Special-case MSVC's /std:c++latest flag.
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("/std:c++latest" cxxflag_std_cxxlatest)
  if(cxxflag_std_cxxlatest)
    message(NOTICE "/std:c++latest is supported")
  else()
    message(FATAL_ERROR "/std:c++latest is not supported")
  endif()
else()
  macro(success)
    message(NOTICE "Latest C++ standard \${CMAKE_CXX_STANDARD_LATEST} is newer than \${cxx_std_version}")
  endmacro()
  macro(failure)
    message(FATAL_ERROR "Latest C++ standard \${CMAKE_CXX_STANDARD_LATEST} is older than \${cxx_std_version}")
  endmacro()

  if(CMAKE_CXX_STANDARD_LATEST GREATER_EQUAL cxx_std_version)
    success() # Both are new: latest >= version.
  else()
    failure() # Both are new: latest < version.
  endif()
endif()
DOC
  "${cmake_binary:?}" -S . -B build --log-level=notice
  popd # "$(tmpfile -d)"
  echo "Checking requested C++ standard is supported... done."
fi

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

  # Replace `/Zi`, which is incompatible with ccache, with `/Z7` while preserving other default debug flags.
  cmake_flags+=(
    "-DCMAKE_POLICY_DEFAULT_CMP0141=NEW"
    "-DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded"
  )

  # Ensure default MSVC flags are preserved despite explicit compiler flags.
  cc_flags+=(/DWIN32 /D_WINDOWS)
  cxx_flags+=(/DWIN32 /D_WINDOWS /GR /EHsc)
  if [[ "${build_type:?}" == "debug" ]]; then
    cxx_flags+=(/Ob0 /Od /RTC1)
  else
    cxx_flags+=(/O2 /Ob2 /DNDEBUG)
  fi
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
      -fsanitize=address
      -O1 -g -fno-omit-frame-pointer
    )
  fi

  # Sanitizers overwrite the usual compiler flags.
  if [[ "${USE_SANITIZER_UBSAN:-}" == "ON" ]]; then
    cxx_flags=(
      "${cxx_flags_init[@]}"
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

if [[ -n "${REQUIRED_CXX_STANDARD:-}" ]]; then
  cmake_flags+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")

  if [[ "${REQUIRED_CXX_STANDARD:?}" == "latest" ]]; then
    [[ "${CMAKE_GENERATOR:-}" =~ "Visual Studio" ]] || {
      echo "REQUIRED_CXX_STANDARD=latest to enable /std:c++latest is only supported with Visual Studio generators" 1>&2
      exit 1
    }

    cxx_flags+=("/std:c++latest") # CMake doesn't support "latest" as a C++ standard.
  else
    cmake_flags+=("-DCMAKE_CXX_STANDARD=${REQUIRED_CXX_STANDARD:?}")
  fi
fi

if [[ "${ENABLE_CODE_COVERAGE:-}" == "ON" ]]; then
  cmake_flags+=("-DENABLE_CODE_COVERAGE=ON")
fi

if [[ "${USE_STATIC_LIBS:-}" == 1 ]]; then
  cmake_flags+=("-DBUILD_SHARED_LIBS=OFF")
fi

if [[ "${USE_SHARED_AND_STATIC_LIBS:-}" == 1 ]]; then
  cmake_flags+=("-DUSE_SHARED_AND_STATIC_LIBS=ON")
fi

if [ "${ENABLE_TESTS:-}" = "ON" ]; then
  cmake_flags+=(
    "-DENABLE_TESTS=ON"
    "-DBUILD_TESTING=ON"
  )
fi

if [[ "${COMPILE_MACRO_GUARD_TESTS:-"OFF"}" == "ON" ]]; then
  cmake_flags+=("-DENABLE_MACRO_GUARD_TESTS=ON")
fi

# Must come after all cc_flags are set.
if [[ "${#cc_flags[@]}" -gt 0 ]]; then
  cmake_flags+=("-DCMAKE_C_FLAGS=${cc_flags[*]}")
fi

# Must come after all cxx_flags are set.
if [[ "${#cxx_flags[@]}" -gt 0 ]]; then
  cmake_flags+=("-DCMAKE_CXX_FLAGS=${cxx_flags[*]}")
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

if [[ -n "$(find "${mongoc_prefix:?}" -name 'bson-config.h')" ]]; then
  : # Used install-c-driver.sh.
elif [[ -n "$(find install -name 'bson-config.h')" ]]; then
  : # Used auto-downloaded C Driver.
else
  echo "unexpectedly compiled using a system libmongoc library" 1>&2
  exit 1
fi
