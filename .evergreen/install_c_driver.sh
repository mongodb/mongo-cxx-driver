#!/bin/bash
# Usage examples:
# MONGOC_VERSION=1.21.2  MONGOCRYPT_VERSION=1.4.1 ./install.sh
# MONGOC_VERSION=1.21.2 MONGOCRYPT_VERSION=1.4.1 ./install.sh
# PREFIX=/tmp/installdir MONGOC_VERSION=1.21.2 MONGOCRYPT_VERSION=1.4.1 ./install.sh

set -o errexit
set -o pipefail

print_usage() {
    echo "usage: MONGOC_VERSION=<version> MONGOCRYPT_VERSION=<version> ./install.sh"
}

if [[ -z $MONGOC_VERSION ]]; then print_usage; exit 2; fi
if [[ -z $MONGOCRYPT_VERSION ]]; then print_usage; exit 2; fi

VERSION=$MONGOC_VERSION
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
curl -sS -o $LIB.tar.gz -L https://api.github.com/repos/mongodb/$LIB/tarball/$VERSION
tar xzf $LIB.tar.gz
DIR=$(echo mongodb-$LIB-*)

# RegEx pattern to match SemVer strings. See https://semver.org/.
SEMVER_REGEX="^(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)(?:-(?P<prerelease>(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\+(?P<buildmetadata>[0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"
if [ $(echo "$VERSION" | perl -ne "$(printf 'exit 1 unless /%s/' $SEMVER_REGEX)") ]; then
    # If $VERSION is already SemVer compliant, use as-is.
    CMAKE_ARGS="$CMAKE_ARGS -DBUILD_VERSION=$BUILD_VERSION"
else
    # Otherwise, use the tag name of the latest release to construct a prerelease version string.

    # Extract "tag_name" from latest Github release.
    BUILD_VERSION=$(curl -sS -H "Accept: application/vnd.github.v3+json" https://api.github.com/repos/mongodb/mongo-c-driver/releases/latest | perl -ne 'print for /"tag_name": "(.+)"/')

    # Assert the tag name is a SemVer string via errexit.
    echo $BUILD_VERSION | perl -ne "$(printf 'exit 1 unless /%s/' $SEMVER_REGEX)"

    # Bump to the next minor version, e.g. 1.0.1 -> 1.1.0.
    BUILD_VERSION=$(echo $BUILD_VERSION | perl -ne "$(printf '/%s/; print $+{major} . "." . ($+{minor}+1) . ".0"' $SEMVER_REGEX)")

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
git checkout $MONGOCRYPT_VERSION
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
