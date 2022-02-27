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
    CONCURRENCY=$(grep -c ^processor /proc/cpuinfo)
elif which sysctl; then
    CONCURRENCY=$(sysctl -n hw.logicalcpu)
else
    echo "$0: can't figure out what value of -j to pass to 'make'" >&2
    exit 1
fi

case "$OS" in
   darwin|linux)
      GENERATOR=${GENERATOR:-"Unix Makefiles"}
      CMAKE_BUILD_OPTS="-j $CONCURRENCY"
      CMAKE_EXAMPLES_TARGET=examples
      if [ "$RUN_DISTCHECK" ]; then
         _RUN_DISTCHECK=$RUN_DISTCHECK
      fi
      ;;

   cygwin*)
      GENERATOR=${GENERATOR:-"Visual Studio 14 2015 Win64"}
      CMAKE_BUILD_OPTS="/maxcpucount:$CONCURRENCY"
      CMAKE_EXAMPLES_TARGET=examples/examples
      ;;

   *)
      echo "$0: unsupported platform '$OS'" >&2
      exit 2
      ;;
esac

. .evergreen/find_cmake.sh

cd build
"$CMAKE" -G "$GENERATOR" "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" -DCMAKE_VERBOSE_MAKEFILE=ON -DMONGOCXX_ENABLE_SLOW_TESTS=ON -DENABLE_UNINSTALL=ON "$@" ..
"$CMAKE" --build . --config $BUILD_TYPE -- $CMAKE_BUILD_OPTS
"$CMAKE" --build . --config $BUILD_TYPE --target install
"$CMAKE" --build . --config $BUILD_TYPE --target $CMAKE_EXAMPLES_TARGET

if [ "$_RUN_DISTCHECK" ]; then
   DISTCHECK_BUILD_OPTS="-j$CONCURRENCY" "$CMAKE" --build . --config $BUILD_TYPE --target distcheck
fi
