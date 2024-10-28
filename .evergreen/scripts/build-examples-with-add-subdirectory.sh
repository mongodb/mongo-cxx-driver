#!/usr/bin/env bash

set -o errexit

cd examples/add_subdirectory

[ -d mongo-c-driver ] || git clone --depth 1 https://github.com/mongodb/mongo-c-driver

rsync -aq --exclude='examples/add_subdirectory' "$(readlink -f ../..)" .

[ -d build ] || mkdir build

# shellcheck source=/dev/null
. ./mongo-c-driver/.evergreen/scripts/find-cmake-latest.sh
export cmake_binary
cmake_binary="$(find_cmake_latest)"

# Use ccache if available.
# shellcheck source=/dev/null
. ./mongo-c-driver/.evergreen/scripts/find-ccache.sh
find_ccache_and_export_vars "$(pwd)" || true
command -v "$cmake_binary"

"$cmake_binary" -S . -B build -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF
"$cmake_binary" --build build

./build/hello_mongocxx
