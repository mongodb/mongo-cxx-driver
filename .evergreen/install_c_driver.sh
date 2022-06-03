#!/bin/bash
# Usage examples:
# ./install.sh master
# ./install.sh 1.3.5
# PREFIX=/tmp/installdir ./install.sh 1.3.5

set -o errexit
set -o pipefail

usage() {
    echo "Please say which version"
    echo "$0 <master|x.y.z>"
}
[ $# -lt 1 ] && { usage; exit 2; }

VERSION=${1}
PREFIX=${PREFIX:-$(pwd)"/../mongoc/"}
OS=$(uname -s | tr '[:upper:]' '[:lower:]')

if [ "$BSON_EXTRA_ALIGNMENT" = "1" ]; then
    ENABLE_EXTRA_ALIGNMENT="ON"
else
    ENABLE_EXTRA_ALIGNMENT="OFF"
fi
CMAKE_ARGS="
  -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_SHM_COUNTERS=OFF
  -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF -DENABLE_STATIC=ON
  -DENABLE_EXTRA_ALIGNMENT=${ENABLE_EXTRA_ALIGNMENT} -DCMAKE_MACOSX_RPATH=ON
  -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_PREFIX_PATH=$PREFIX"

echo "About to install C driver ($VERSION) into $PREFIX"

LIB=mongo-c-driver
rm -rf $(echo $LIB*)
curl -sS -o $LIB.zip -L https://github.com/mongodb/$LIB/archive/$VERSION.zip
unzip -q $LIB.zip
DIR=$(echo $LIB-*)

# RegEx pattern to match SemVer strings. See https://semver.org/.
SEMVER_REGEX="^(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)(?:-(?P<prerelease>(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+(?P<buildmetadata>[0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"
if [ $(echo "$VERSION" | grep -P "$SEMVER_REGEX") ]; then
    # If $VERSION is already SemVer compliant, use as-is.
    CMAKE_ARGS="$CMAKE_ARGS -DBUILD_VERSION=$BUILD_VERSION"
else
    # Otherwise, use the tag name of the latest release to construct a prerelease version string.

    # Extract "tag_name" from latest Github release.
    BUILD_VERSION=$(curl -sS -H "Accept: application/vnd.github.v3+json" https://api.github.com/repos/mongodb/mongo-c-driver/releases/latest | jq -r ".tag_name")

    # Assert the tag name is a SemVer string.
    test $(echo $BUILD_VERSION | grep -P "$SEMVER_REGEX")

    # Bump to the next minor version, e.g. 1.0.1 -> 1.1.0.
    BUILD_VERSION=$(echo $BUILD_VERSION | perl -pe "$(printf 's/%s/$+{major} . "." . ($+{minor}+1) . ".0"/e' $SEMVER_REGEX)")

    # Append a prerelease tag, e.g. 1.1.0-pre+<version>.
    BUILD_VERSION=$(printf "%s-pre+%s" $BUILD_VERSION $VERSION)

    # Use the constructed prerelease build version when building the C driver.
    CMAKE_ARGS="$CMAKE_ARGS -DBUILD_VERSION=$BUILD_VERSION"
fi

. .evergreen/find_cmake.sh

cd $DIR

if [ -f /proc/cpuinfo ]; then
    CONCURRENCY=$(grep -c ^processor /proc/cpuinfo)
elif which sysctl; then
    CONCURRENCY=$(sysctl -n hw.logicalcpu)
else
    echo "$0: can't figure out what value of -j to pass to 'make'" >&2
    exit 1
fi

export CFLAGS="-fPIC"

case "$OS" in
    darwin|linux)
        GENERATOR=${GENERATOR:-"Unix Makefiles"}
        CMAKE_BUILD_OPTS="-j $CONCURRENCY"
        ;;

    cygwin*)
        GENERATOR=${GENERATOR:-"Visual Studio 14 2015 Win64"}
        CMAKE_BUILD_OPTS="/maxcpucount:$CONCURRENCY"
        ;;

    *)
        echo "$0: unsupported platform '$OS'" >&2
        exit 2
        ;;
esac

# build libbson
mkdir cmake_build
cd cmake_build
"$CMAKE" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="Debug" -DENABLE_MONGOC=OFF $CMAKE_ARGS ..
"$CMAKE" --build . --config Debug -- $CMAKE_BUILD_OPTS
"$CMAKE" --build . --config Debug --target install
cd ../../

# fetch and build libmongocrypt
git clone https://github.com/mongodb/libmongocrypt
mkdir libmongocrypt/cmake_build
cd libmongocrypt/cmake_build
"$CMAKE" -G "$GENERATOR" -DENABLE_SHARED_BSON=ON -DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_PREFIX_PATH="$PREFIX" -DCMAKE_BUILD_TYPE="Debug" -DENABLE_CLIENT_SIDE_ENCRYPTION=OFF ..
"$CMAKE" --build . --config Debug -- $CMAKE_BUILD_OPTS
"$CMAKE" --build . --config Debug --target install
cd ../../$DIR

# build libmongoc
cd cmake_build
"$CMAKE" -G "$GENERATOR" -DCMAKE_BUILD_TYPE="Debug" -DENABLE_MONGOC=ON -DENABLE_CLIENT_SIDE_ENCRYPTION=ON $CMAKE_ARGS ..
"$CMAKE" --build . --config Debug -- $CMAKE_BUILD_OPTS
"$CMAKE" --build . --config Debug --target install
cd ../

echo "Done installing"

cd ..
ls -l ..
