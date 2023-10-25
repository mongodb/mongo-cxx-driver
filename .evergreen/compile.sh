#!/bin/bash

# Runs cmake and compiles the standard build targets (all, install, examples).  Any arguments passed
# to this script will be forwarded on as flags passed to cmake.
#
# This script should be run from the root of the repository.  This script will run the build from
# the default build directory './build'.  The following environment variables will change the
# behavior of this script:
# - BUILD_TYPE: must be set to "Release" or "Debug"

set -o errexit
set -o pipefail

if [ "$BUILD_TYPE" != "Debug" -a "$BUILD_TYPE" != "Release" ]; then
    echo "$0: expected BUILD_TYPE environment variable to be set to 'Debug' or 'Release'" >&2
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
      GENERATOR=${GENERATOR:-"Unix Makefiles"}
      CMAKE_EXAMPLES_TARGET=examples
      if [ "$RUN_DISTCHECK" ]; then
         _RUN_DISTCHECK=$RUN_DISTCHECK
      fi
      ;;

   cygwin*)
      GENERATOR=${GENERATOR:-"Visual Studio 14 2015 Win64"}
      CMAKE_BUILD_OPTS="/verbosity:minimal"
      CMAKE_EXAMPLES_TARGET=examples/examples
      ;;

   *)
      echo "$0: unsupported platform '$OS'" >&2
      exit 2
      ;;
esac

cd build
"${cmake_binary}" -G "$GENERATOR" "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" -DBUILD_TESTING=ON -DMONGOCXX_ENABLE_SLOW_TESTS=ON -DENABLE_UNINSTALL=ON "$@" ..

if [[ "${COMPILE_MACRO_GUARD_TESTS:-"OFF"}" == "ON" ]]; then
   # We only need to compile the macro guard tests.
   "${cmake_binary}" -DENABLE_MACRO_GUARD_TESTS=ON ..
   "${cmake_binary}" --build . --config $BUILD_TYPE --target test_bsoncxx_macro_guards test_mongocxx_macro_guards -- $CMAKE_BUILD_OPTS
   exit # Nothing else to be done.
fi

# Regular build and install routine.
"${cmake_binary}" --build . --config $BUILD_TYPE -- $CMAKE_BUILD_OPTS
"${cmake_binary}" --build . --config $BUILD_TYPE --target install -- $CMAKE_BUILD_OPTS
"${cmake_binary}" --build . --config $BUILD_TYPE --target $CMAKE_EXAMPLES_TARGET -- $CMAKE_BUILD_OPTS

if [ "$_RUN_DISTCHECK" ]; then
  "${cmake_binary}" --build . --config $BUILD_TYPE --target distcheck
fi
