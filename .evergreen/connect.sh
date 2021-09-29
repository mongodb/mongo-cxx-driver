#!/bin/bash

set -o errexit
set -o pipefail

LIB_DIR=${LIB_DIR:-"lib"}
BUILD_TYPE=${BUILD_TYPE:-"Debug"}

if [ -z "$URI" ]; then
    echo "URI is a required environment variable.";
    exit 1;
fi
if [ -z "$MONGOC_INSTALL_PREFIX" ]; then
    echo "MONGOC_INSTALL_PREFIX is a required environment variable.";
    exit 1;
fi
if [ -z "$MONGOCXX_INSTALL_PREFIX" ]; then
    echo "MONGOCXX_INSTALL_PREFIX is a required environment variable.";
    exit 1;
fi
if [ -z "$BUILD_DIR" ]; then
    echo "BUILD_DIR is a required environment variable.";
    exit 1;
fi

# Use PATH / LD_LIBRARY_PATH / DYLD_LIBRARY_PATH to inform the tests where to find
# mongoc library dependencies on Windows / Linux / Mac OS, respectively.
export PATH=$PATH:$MONGOC_INSTALL_PREFIX/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MONGOC_INSTALL_PREFIX/$LIB_DIR/
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$MONGOC_INSTALL_PREFIX/$LIB_DIR/

# Windows also needs to be informed where to find mongocxx library dependencies.
export PATH=$PATH:$BUILD_DIR/src/bsoncxx/$BUILD_TYPE
export PATH=$PATH:$BUILD_DIR/src/mongocxx/$BUILD_TYPE
export PATH=$PATH:$MONGOCXX_INSTALL_PREFIX/bin

if [ "Windows_NT" == "$OS" ]; then
    $BUILD_DIR/examples/mongocxx/$BUILD_TYPE/connect.exe "$URI"
else
    $BUILD_DIR/examples/mongocxx/connect "$URI"
fi