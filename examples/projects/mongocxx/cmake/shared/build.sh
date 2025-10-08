#!/usr/bin/env bash

set -o errexit
set -o pipefail

rm -rf build/*
cd build
if [ -z "$MSVC" ]; then
  "${cmake_binary:?}" -DCMAKE_BUILD_TYPE="${build_type:?}" -DCMAKE_CXX_STANDARD="${CXX_STANDARD:?}" ..
  "${cmake_binary:?}" --build . --target run
else
  "${cmake_binary:?}" -G "Visual Studio 15 2017" -A "x64" -DCMAKE_CXX_STANDARD="${CXX_STANDARD:?}" -DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded ..
  "${cmake_binary:?}" --build . --target run --config "${build_type:?}" -- /verbosity:minimal
fi
