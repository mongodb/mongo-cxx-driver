#!/bin/sh

# Runs cmake and compiles the standard build targets (all, install, examples).  Any arguments passed
# to this script will be forwarded on as flags passed to cmake.
#
# This script should be run from the root of the repository.  This script will run the build from
# the default build directory './build'.  The following environment variables will change the
# behavior of this script:
# - BUILD_TYPE: must be set to "Release" or "Debug"
# - PATH_TO_CMAKE: full path to cmake (defaults to searching $PATH)
# - PATH_TO_BUILD_TOOL: full path to make / msbuild.exe (defaults to searching $PATH)

set -o xtrace
set -o errexit

if [ "$BUILD_TYPE" != "Debug" -a "$BUILD_TYPE" != "Release" ]; then
    echo "$0: expected BUILD_TYPE environment variable to be set to 'Debug' or 'Release'" >&2
    exit 1
fi

OS=$(uname -s | tr '[:upper:]' '[:lower:]')

PATH_TO_CMAKE="${PATH_TO_CMAKE:-cmake}"

cd build
"$PATH_TO_CMAKE" "-DCMAKE_BUILD_TYPE=${BUILD_TYPE}" "$@" ..

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
        PATH_TO_BUILD_TOOL="${PATH_TO_BUILD_TOOL:-make}"
        "$PATH_TO_BUILD_TOOL" "-j$CONCURRENCY" all
        "$PATH_TO_BUILD_TOOL" install
        "$PATH_TO_BUILD_TOOL" "-j$CONCURRENCY" examples
        ;;
    cygwin*)
        PATH_TO_BUILD_TOOL="${PATH_TO_BUILD_TOOL:-msbuild.exe}"
        "$PATH_TO_BUILD_TOOL" /p:Configuration=${BUILD_TYPE} /m ALL_BUILD.vcxproj
        "$PATH_TO_BUILD_TOOL" /p:Configuration=${BUILD_TYPE} INSTALL.vcxproj
        "$PATH_TO_BUILD_TOOL" /p:Configuration=${BUILD_TYPE} /m examples/examples.vcxproj
        ;;
    *)
        echo "$0: unsupported platform '$OS'" >&2
        exit 1
        ;;
esac
