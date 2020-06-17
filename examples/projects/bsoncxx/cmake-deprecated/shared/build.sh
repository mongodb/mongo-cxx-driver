#!/bin/bash
set -o errexit
set -o pipefail
set -o xtrace

BUILD_TYPE=${BUILD_TYPE:-Release}
CXX_STANDARD=${CXX_STANDARD:-11}
CMAKE=${CMAKE:-cmake}

rm -rf build/*
cd build
if [ -z "$MSVC" ]; then
    "$CMAKE" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" ..
    make run VERBOSE=1
else
    "$CMAKE" -G "Visual Studio 14 2015 Win64" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" -DBOOST_ROOT=c:/local/boost_1_60_0 ..
    MSBuild.exe /p:Configuration="${BUILD_TYPE}" run.vcxproj
fi
