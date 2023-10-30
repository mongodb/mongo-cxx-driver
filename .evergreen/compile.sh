#!/bin/bash

# Runs cmake and compiles the standard build targets (all, install, examples).  Any arguments passed
# to this script will be forwarded on as flags passed to cmake.
#
# This script should be run from the root of the repository.  This script will run the build from
# the default build directory './build'.  The following environment variables will change the
# behavior of this script:
# - build_type: must be set to "Release" or "Debug"

set -o errexit
set -o pipefail

# Add MSBuild.exe to path.
if [[ "${OSTYPE:?}" == "cygwin" ]]; then
   case "${generator:-}" in
   *2015*)
      PATH="/cygdrive/c/cmake/bin:/cygdrive/c/Program Files (x86)/MSBuild/14.0/Bin:$PATH"
      ;;
   *2017*)
      PATH="/cygdrive/c/cmake/bin:/cygdrive/c/Program Files (x86)/Microsoft Visual Studio/2017/Professional/MSBuild/15.0/Bin:$PATH"
      ;;
   esac
fi
export PATH

mongoc_prefix="$(pwd)/../mongoc"
echo "mongoc_prefix=${mongoc_prefix:?}"

if [[ "${OSTYPE:?}" =~ cygwin ]]; then
      mongoc_prefix=$(cygpath -m "${mongoc_prefix:?}")
fi

. "${mongoc_prefix:?}/.evergreen/scripts/find-cmake-latest.sh"
export cmake_binary
cmake_binary="$(find_cmake_latest)"
command -v "$cmake_binary"

if [ ! -d ../drivers-evergreen-tools ]; then
   git clone --depth 1 git@github.com:mongodb-labs/drivers-evergreen-tools.git ../drivers-evergreen-tools
fi
. ../drivers-evergreen-tools/.evergreen/find-python3.sh
. ../drivers-evergreen-tools/.evergreen/venv-utils.sh

venvcreate "$(find_python3)" venv
python -m pip install GitPython

if [ "$build_type" != "Debug" -a "$build_type" != "Release" ]; then
    echo "$0: expected build_type environment variable to be set to 'Debug' or 'Release'" >&2
    exit 1
fi

OS=$(uname -s | tr '[:upper:]' '[:lower:]')

if [ -f /proc/cpuinfo ]; then
    CMAKE_BUILD_PARALLEL_LEVEL=$(grep -c ^processor /proc/cpuinfo)
elif which sysctl; then
    CMAKE_BUILD_PARALLEL_LEVEL=$(sysctl -n hw.logicalcpu)
else
    echo "$0: can't figure out what build parallel level to use" >&2
    exit 1
fi
export CMAKE_BUILD_PARALLEL_LEVEL

case "$OS" in
   darwin|linux)
      CMAKE_EXAMPLES_TARGET=examples
      if [ "$RUN_DISTCHECK" ]; then
         _RUN_DISTCHECK=$RUN_DISTCHECK
      fi
      ;;

   cygwin*)
      CMAKE_BUILD_OPTS="/verbosity:minimal"
      CMAKE_EXAMPLES_TARGET=examples/examples
      ;;

   *)
      echo "$0: unsupported platform '$OS'" >&2
      exit 2
      ;;
esac

cd build

cmake_flags=(
   -DCMAKE_BUILD_TYPE=${build_type}
   -DCMAKE_PREFIX_PATH="${mongoc_prefix:?}"
   -DBUILD_TESTING=ON
   -DMONGOCXX_ENABLE_SLOW_TESTS=ON
   -DCMAKE_INSTALL_PREFIX=install
   -DENABLE_UNINSTALL=ON
)

case "${OSTYPE:?}" in
cygwin)
   case "${generator:-}" in
   *2015*) cmake_flags+=(-DBOOST_ROOT=C:/local/boost_1_60_0) ;;
   *2017*) cmake_flags+=(-DCMAKE_CXX_STANDARD=17) ;;
   *) echo "missing CMake Generator on Windows distro" 1>&2; exit 1 ;;
   esac
   ;;
darwin*|linux*)
   : ${generator:="Unix Makefiles"}
   ;;
*)
   echo "unrecognized operating system ${OSTYPE:?}" 1>&2
   exit 1
   ;;
esac
export CMAKE_GENERATOR="${generator:?}"

if [[ "${USE_POLYFILL_STD_EXPERIMENTAL:-}" == "ON" ]]; then
   cmake_flags+=(-DCMAKE_CXX_STANDARD=14 -DBSONCXX_POLY_USE_STD_EXPERIMENTAL=ON)
fi

if [[ "${USE_POLYFILL_BOOST:-}" == "ON" ]]; then
   cmake_flags+=(-DBSONCXX_POLY_USE_BOOST=ON)
fi
 
cc_flags_init=(-Wall -Wextra -Wno-attributes -Werror -Wno-missing-field-initializers)
cxx_flags_init=(-Wall -Wextra -Wconversion -Wnarrowing -pedantic -Werror)
cc_flags=()
cxx_flags=()

case "${OSTYPE:?}" in
cygwin)
   ;;
darwin*)
   cc_flags+=("${cc_flags_init[@]}")
   cxx_flags+=("${cxx_flags_init[@]}" -stdlib=libc++)
   ;;
linux*)
   cc_flags+=("${cc_flags_init[@]}")
   cxx_flags+=("${cxx_flags_init[@]}" -Wno-expansion-to-defined -Wno-missing-field-initializers)
   ;;
*)
   echo "unrecognized operating system ${OSTYPE:?}" 1>&2
   exit 1
   ;;
esac

# Sanitizers overwrite the usual compiler flags.
if [[ "${USE_SANITIZER_ASAN:-}" == "ON" ]]; then
   cxx_flags=(
      "${cxx_flags_init[@]}"
      -D_GLIBCXX_USE_CXX11_ABI=0
      -fsanitize=address
      -O1 -g -fno-omit-frame-pointer
   )
fi
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

# Ignore warnings generated by core::optional in mnmlstc/core.
if [[ "${HOSTTYPE:?}" == powerpc64le ]]; then
   cxx_flags+=(-Wno-error=maybe-uninitialized)
fi

# Ignore deprecation warnings when building on a release branch.
if [ "$(echo "${branch_name:?}" | cut -f2 -d'/')" != "${branch_name:?}" ]; then
   cc_flags+=(-Wno-deprecated-declarations)
   cxx_flags+=(-Wno-deprecated-declarations)
fi

if [[ "${#cc_flags[@]}" > 0 ]]; then
   cmake_flags+=("-DCMAKE_C_FLAGS=${cc_flags[*]}")
fi

if [[ "${#cxx_flags[@]}" > 0 ]]; then
   cmake_flags+=("-DCMAKE_CXX_FLAGS=${cxx_flags[*]}")
fi

if [[ "${ENABLE_CODE_COVERAGE:-}" == "ON" ]]; then
   cmake_flags+=("-DENABLE_CODE_COVERAGE=ON")
fi

if [ "${USE_STATIC_LIBS:-}" ]; then
   cmake_flags+=("-DBUILD_SHARED_LIBS=OFF")
fi

if [ "${ENABLE_TESTS:-}" = "OFF" ]; then
   cmake_flags+=("-DENABLE_TESTS=OFF")
fi

if [[ -n "${REQUIRED_CXX_STANDARD:-}" ]]; then
   cmake_flags+=("-DCMAKE_CXX_STANDARD=${REQUIRED_CXX_STANDARD:?}")
   cmake_flags+=("-DCMAKE_CXX_STANDARD_REQUIRED=ON")
fi

echo "Configuring with CMake flags: ${cmake_flags[*]}"

"${cmake_binary}" "${cmake_flags[@]}" ..

if [[ "${COMPILE_MACRO_GUARD_TESTS:-"OFF"}" == "ON" ]]; then
   # We only need to compile the macro guard tests.
   "${cmake_binary}" -DENABLE_MACRO_GUARD_TESTS=ON ..
   "${cmake_binary}" --build . --config $build_type --target test_bsoncxx_macro_guards test_mongocxx_macro_guards -- $CMAKE_BUILD_OPTS
   exit # Nothing else to be done.
fi

# Regular build and install routine.
"${cmake_binary}" --build . --config $build_type -- $CMAKE_BUILD_OPTS
"${cmake_binary}" --build . --config $build_type --target install -- $CMAKE_BUILD_OPTS
"${cmake_binary}" --build . --config $build_type --target $CMAKE_EXAMPLES_TARGET -- $CMAKE_BUILD_OPTS

if [ "$_RUN_DISTCHECK" ]; then
  "${cmake_binary}" --build . --config $build_type --target distcheck
fi
