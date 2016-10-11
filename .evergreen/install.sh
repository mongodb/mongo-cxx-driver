#!/bin/sh
# Usage examples:
# ./install.sh libbson master
# ./install.sh mongoc 1.3.5
# PREFIX=/tmp/installdir CONFIGURE_ARGS="--enable-debug" ./install.sh mongoc 1.3.5


set -o errexit
set -o xtrace


usage() {
	echo "Please either say mongoc, or libbson, optionally which version"
	echo "$0 <mongoc|libbson> [master|x.y.z]"
}
[ $# -lt 1 ] && { usage; exit 2; }

CONFIGURE_ARGS_MONGOC="--disable-automatic-init-and-cleanup --disable-shm-counters --with-libbson=system"
CONFIGURE_ARGS_BSON="--disable-extra-align"
CONFIGURE_ARGS_EXTRA="--enable-tests=no --enable-examples=no --enable-debug --enable-optimizations --disable-static --disable-dependency-tracking --with-pic"
VERSION=${2:-master}
PREFIX=${PREFIX:-$(pwd)"/deps-install"}
OS=$(uname -s | tr '[:upper:]' '[:lower:]')

case "$1" in
	libbson)
		LIB="libbson"
		CONFIGURE_ARGS=${CONFIGURE_ARGS:-$CONFIGURE_ARGS_BSON $CONFIGURE_ARGS_EXTRA}
	;;

	mongoc)
		LIB="mongo-c-driver"
		CONFIGURE_ARGS=${CONFIGURE_ARGS:=$CONFIGURE_ARGS_MONGOC $CONFIGURE_ARGS_EXTRA}
	;;
	*)
		echo "Got '$2'"
		usage;
		exit 2
	;;
esac

echo "About to install $LIB ($VERSION) into $PREFIX configured with '$CONFIGURE_ARGS'"

if [ "${VERSION}" = "master" ]; then
	rm -rf $LIB
	# Must be http as rhel55 has https issues
	curl -o $LIB.tgz -L http://s3.amazonaws.com/mciuploads/$LIB/$LIB-latest.tar.gz
	tar --extract --file $LIB.tgz
	rm -rf $LIB
	DIR=$(echo $LIB-*)
else
	DIR=$LIB-${VERSION}
	rm -rf $LIB.tgz $DIR
	curl -o $LIB.tgz -L https://github.com/mongodb/$LIB/releases/download/${VERSION}/$LIB-${VERSION}.tar.gz
	tar --extract --file $LIB.tgz
fi

case "$OS" in
	darwin)
		MAKEFLAGS="-j"$(sysctl -n hw.logicalcpu)
		;;

	linux)
		MAKEFLAGS="-j"$(grep -c ^processor /proc/cpuinfo)
		;;
esac

cd $DIR

PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig" ./configure $CONFIGURE_ARGS --prefix="$PREFIX"
make
make install

