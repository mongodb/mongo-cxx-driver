#!/bin/bash
set -o errexit
set -o pipefail
set -o xtrace

if [ -z "$BUILD_TYPE" ]; then
    BUILD_TYPE=Release
fi

if [ -z "$CXX_STANDARD" ]; then
    CXX_STANDARD=11
fi

rm -rf build/*
cd build
if [ -z "$MSVC" ]; then
    cmake -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" ..
    make run VERBOSE=1
else
    cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" -DBOOST_ROOT=c:/local/boost_1_60_0 ..
    MSBuild.exe /p:Configuration="${BUILD_TYPE}" run.vcxproj
fi
