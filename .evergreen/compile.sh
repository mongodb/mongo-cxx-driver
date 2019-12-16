#!/bin/bash

# Runs cmake and compiles the standard build targets (all, install, examples).  Any arguments passed
# to this script will be forwarded on as flags passed to cmake.
#
# This script should be run from the root of the repository.  This script will run the build from
# the default build directory './build'.  The following environment variables will change the
# behavior of this script:
# - BUILD_TYPE: must be set to "Release" or "Debug"

set -o xtrace
set -o errexit
set -o pipefail

if [ "$BUILD_TYPE" != "Debug" -a "$BUILD_TYPE" != "Release" ]; then
    echo "$0: expected BUILD_TYPE environment variable to be set to 'Debug' or 'Release'" >&2
    exit 1
fi

OS=$(uname -s | tr '[:upper:]' '[:lower:]')

cd build
cmake "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" -DMONGOCXX_ENABLE_SLOW_TESTS=ON -DENABLE_UNINSTALL=ON "$@" ..

case "$OS" in
    darwin|linux)
        if [ -f /proc/cpuinfo ]; then
            CONCURRENCY=$(grep -c ^processor /proc/cpuinfo)
        elif which sysctl; then
            CONCURRENCY=$(sysctl -n hw.logicalcpu)
        else
            echo "$0: can't figure out what value of -j to pass to 'make'" >&2
            exit 1
        fi
        make "-j$CONCURRENCY" all VERBOSE=1
        make install VERBOSE=1
        make "-j$CONCURRENCY" examples VERBOSE=1
        if [ "$RUN_DISTCHECK" ]; then
                make DISTCHECK_BUILD_OPTS="-j$CONCURRENCY" distcheck VERBOSE=1
        fi
        ;;
    cygwin*)
        MSBuild.exe /p:Configuration=${BUILD_TYPE} /m ALL_BUILD.vcxproj
        MSBuild.exe /p:Configuration=${BUILD_TYPE} INSTALL.vcxproj
        MSBuild.exe /p:Configuration=${BUILD_TYPE} /m examples/examples.vcxproj
        ;;
    *)
        echo "$0: unsupported platform '$OS'" >&2
        exit 1
        ;;
esac
