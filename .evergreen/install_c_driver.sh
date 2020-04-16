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
  -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_SHM_COUNTERS=OFF
  -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF -DENABLE_STATIC=ON -DENABLE_EXTRA_ALIGNMENT=OFF
  -DCMAKE_MACOSX_RPATH=ON -DCMAKE_INSTALL_PREFIX=$PREFIX -DCMAKE_PREFIX_PATH=$PREFIX"

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
	# build libbson
	mkdir cmake_build
	cd cmake_build
	$CMAKE -DENABLE_MONGOC=OFF $CMAKE_ARGS ..
	make "-j$CONCURRENCY"
	make install
	cd ../../

	# fetch and build libmongocrypt
	git clone https://github.com/mongodb/libmongocrypt
	mkdir libmongocrypt/cmake_build
	cd libmongocrypt/cmake_build
	$CMAKE -DENABLE_SHARED_BSON=ON -DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_PREFIX_PATH="$PREFIX" -DCMAKE_BUILD_TYPE="Debug" -DENABLE_CLIENT_SIDE_ENCRYPTION=OFF ..
	make install
	cd ../../$DIR

	# build libmongoc
	cd cmake_build
        $CMAKE -DENABLE_MONGOC=ON -DENABLE_CLIENT_SIDE_ENCRYPTION=ON $CMAKE_ARGS ..
        make "-j$CONCURRENCY"
        make install
	cd ../
        ;;

    cygwin*)
        GENERATOR=${GENERATOR:-"Visual Studio 14 2015 Win64"}

	# build libbson
	mkdir cmake_build
	cd cmake_build
	"$CMAKE" -G "$GENERATOR" -DENABLE_MONGOC=OFF $CMAKE_ARGS ..
	"$CMAKE" --build . --target INSTALL --config "Debug" -- /m
	cd ../../

	# fetch and build libmongocrypt
	git clone https://github.com/mongodb/libmongocrypt
	mkdir libmongocrypt/cmake_build
	cd libmongocrypt/cmake_build
	"$CMAKE" -G "$GENERATOR" -DENABLE_SHARED_BSON=ON -DCMAKE_BUILD_TYPE="Debug" $CMAKE_ARGS ..
	"$CMAKE" --build . --target INSTALL --config "Debug" -- /m
	cd ../../$DIR

	# build libmongoc
	cd cmake_build
        "$CMAKE" -G "$GENERATOR" -DENABLE_MONGOC=ON -DENABLE_CLIENT_SIDE_ENCRYPTION=ON $CMAKE_ARGS ..
	"$CMAKE" --build . --target INSTALL --config "Debug" -- /m
	cd ../
        ;;

    *)
        echo "$0: unsupported platform '$OS'" >&2
        exit 2
        ;;
esac

echo "Done installing"

cd ..
ls -l ..
