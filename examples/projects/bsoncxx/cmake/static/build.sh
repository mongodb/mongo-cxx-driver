#!/bin/bash

set -x
set -o errexit
set -o pipefail

BUILD_TYPE=${BUILD_TYPE:-Release}
CXX_STANDARD=${CXX_STANDARD:-11}
CMAKE=${cmake_binary:-cmake}

rm -rf build/*
cd build
if [ -z "$MSVC" ]; then
    printf "\n\nLISTING INSTALL DIR\n\n"
    pwd
    find ../../../../../../build/install
    printf "\n\nDONE LISTING INSTALL DIR\n\n"
    ../../../../../../.evergreen/install_c_driver.sh

    "$CMAKE" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" -DCMAKE_INSTALL_PREFIX="../../../../../../build/install" ..
    "$CMAKE" --build . --target run
else
    if [ "$CXX_STANDARD" = "17" ]; then
        "$CMAKE" -G "Visual Studio 15 2017 Win64" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" ..
    else
        # Boost is needed for pre-17 Windows polyfill.
        "$CMAKE" -G "Visual Studio 14 2015 Win64" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" -DBOOST_ROOT=c:/local/boost_1_60_0 ..
    fi
    "$CMAKE" --build . --target run --config "${BUILD_TYPE}" -- /verbosity:minimal
fi
