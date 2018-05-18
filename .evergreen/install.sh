#!/bin/bash
# Usage examples:
# ./install.sh master
# ./install.sh 1.3.5
# PREFIX=/tmp/installdir ./install.sh 1.3.5

set -o errexit
set -o xtrace
set -o pipefail

usage() {
    echo "Please say which version"
    echo "$0 <master|x.y.z>"
}
[ $# -lt 1 ] && { usage; exit 2; }

VERSION=${1}
PREFIX=${PREFIX:-$(pwd)"/../mongoc/"}
OS=$(uname -s | tr '[:upper:]' '[:lower:]')

CMAKE_ARGS="
  -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_SHM_COUNTERS=OFF -DENABLE_BSON=ON
  -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF -DENABLE_STATIC=ON -DENABLE_EXTRA_ALIGNMENT=OFF
  -DCMAKE_MACOSX_RPATH=ON -DCMAKE_INSTALL_PREFIX=$PREFIX"

echo "About to install C driver ($VERSION) into $PREFIX"

LIB=mongo-c-driver
if [ -n "$(echo "${VERSION}" | grep '^[a-z]' )" ]; then
    rm -rf $LIB
    # Must be http as rhel55 has https issues
    curl -o $LIB.tgz -L http://s3.amazonaws.com/mciuploads/$LIB/$VERSION/$LIB-latest.tar.gz
    tar --extract --file $LIB.tgz
    rm -rf $LIB
    DIR=$(echo $LIB-*)
else
    DIR=$LIB-${VERSION}
    rm -rf $LIB.tgz $DIR
    curl -o $LIB.tgz -L https://github.com/mongodb/$LIB/releases/download/${VERSION}/$LIB-${VERSION}.tar.gz
    tar --extract --file $LIB.tgz
fi

if [ -f "/Applications/cmake-3.2.2-Darwin-x86_64/CMake.app/Contents/bin/cmake" ]; then
    CMAKE="/Applications/cmake-3.2.2-Darwin-x86_64/CMake.app/Contents/bin/cmake"
elif [ -f "/Applications/Cmake.app/Contents/bin/cmake" ]; then
    CMAKE="/Applications/Cmake.app/Contents/bin/cmake"
elif [ -f "/opt/cmake/bin/cmake" ]; then
    CMAKE="/opt/cmake/bin/cmake"
elif command -v cmake 2>/dev/null; then
    CMAKE=cmake
fi

cd $DIR

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
        PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig" $CMAKE $CMAKE_ARGS .
        make "-j$CONCURRENCY"
        make install
        ;;

    cygwin*)
        /cygdrive/c/cmake/bin/cmake -G "Visual Studio 14 2015 Win64" $CMAKE_ARGS .
        "/cygdrive/c/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe" /m INSTALL.vcxproj
        ;;

    *)
        echo "$0: unsupported platform '$OS'" >&2
        exit 2
        ;;
esac

echo "Done installing"

cd ..
ls -l ..
