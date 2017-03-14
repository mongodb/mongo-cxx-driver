#!/bin/sh
# Usage examples:
# ./install.sh master
# ./install.sh 1.3.5
# PREFIX=/tmp/installdir ./install.sh 1.3.5

set -o errexit
set -o xtrace

usage() {
    echo "Please say which version"
    echo "$0 <master|x.y.z>"
}
[ $# -lt 1 ] && { usage; exit 2; }

VERSION=${1}
PREFIX=${PREFIX:-$(pwd)"/deps-install"}
OS=$(uname -s | tr '[:upper:]' '[:lower:]')

# TODO CXX-1204: improve/tweak the flags below.
case "$OS" in
    darwin|linux)
        CONFIGURE_ARGS_MONGOC="--disable-automatic-init-and-cleanup --disable-shm-counters --with-libbson=system"
        CONFIGURE_ARGS_BSON="--disable-extra-align"
        CONFIGURE_ARGS_EXTRA="--enable-tests=no --enable-examples=no --enable-debug --enable-optimizations --disable-static --disable-dependency-tracking --with-pic --prefix=$PREFIX"
        ;;

    cygwin*)
        CONFIGURE_ARGS_MONGOC=
        CONFIGURE_ARGS_BSON=
        CONFIGURE_ARGS_EXTRA="-DCMAKE_INSTALL_PREFIX=$(cygpath -m "$PREFIX")"
        ;;

    *)
        echo "$0: unsupported platform '$OS'" >&2
        exit 2
        ;;
esac

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

cd $DIR

compile_and_install() {
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
            PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig" ./configure "$@"
            make "-j$CONCURRENCY"
            make install
            ;;

        cygwin*)
            /cygdrive/c/cmake/bin/cmake -G "Visual Studio 14 2015 Win64" "$@"
            "/cygdrive/c/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe" /m INSTALL.vcxproj
            ;;

        *)
            echo "$0: unsupported platform '$OS'" >&2
            exit 2
            ;;
    esac
}

# Compile and install libbson.
( cd src/libbson && compile_and_install ${CONFIGURE_ARGS_BSON} ${CONFIGURE_ARGS_EXTRA} )

# Compile and install libmongoc.
compile_and_install ${CONFIGURE_ARGS_MONGOC} ${CONFIGURE_ARGS_EXTRA}

