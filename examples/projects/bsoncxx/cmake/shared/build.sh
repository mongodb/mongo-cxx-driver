#!/usr/bin/env bash

set -o errexit
set -o pipefail

BUILD_TYPE=${BUILD_TYPE:-Release}
CXX_STANDARD=${CXX_STANDARD:-11}
CMAKE=${cmake_binary:-cmake}

rm -rf build/*
cd build
if [ -z "$MSVC" ]; then
    "$CMAKE" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" ..
    "$CMAKE" --build . --target run
else
    "$CMAKE" -G "Visual Studio 15 2017" -A "x64" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" ..
    "$CMAKE" --build . --target run --config "${BUILD_TYPE}" -- /verbosity:minimal
fi
