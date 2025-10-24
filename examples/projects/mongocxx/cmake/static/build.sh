#!/usr/bin/env bash

set -o errexit
set -o pipefail

rm -rf build/*
cd build
if [ -z "$MSVC" ]; then
  uvx cmake -DCMAKE_BUILD_TYPE="${build_type}" -DCMAKE_CXX_STANDARD="${CXX_STANDARD:?}" ..
  uvx cmake --build . --target run
else
  uvx cmake -G "Visual Studio 15 2017" -A "x64" -DCMAKE_CXX_STANDARD="${CXX_STANDARD:?}" -DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT=Embedded ..
  uvx cmake --build . --target run --config "${build_type:?}" -- /verbosity:minimal
fi
