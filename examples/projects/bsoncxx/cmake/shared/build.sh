#!/usr/bin/env bash

set -o errexit
set -o pipefail

rm -rf build/*

config_flags=("-DCMAKE_CXX_STANDARD=${CXX_STANDARD:?}")
build_flags=()

if [[ "${OSTYPE:?}" == cygwin ]]; then
  build_flags+=(--config "${build_type:?}")
else
  config_flags+=("-DCMAKE_BUILD_TYPE=${build_type:?}")
fi

build_flags+=(--target run)

uvx cmake "${config_flags[@]:?}" -B build
uvx cmake --build build "${build_flags[@]:?}"
