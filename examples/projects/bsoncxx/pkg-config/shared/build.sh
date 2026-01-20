#!/usr/bin/env bash

set -o errexit
set -o pipefail

# Sanity-check that static library macros are not set when building against the shared library.
# Users don't need to include this section in their projects.
if ! pkgconf --cflags libbsoncxx | grep -v -q -- -DBSONCXX_STATIC; then
  echo "Expected BSONCXX_STATIC to not be set" >&2
  exit 1
fi

compile_flags=(
  "-std=c++${CXX_STANDARD:?}"
  -Wall -Wextra -Werror
  ${CXXFLAGS:-}
  $(pkg-config --cflags libbsoncxx)
)

link_flags=(
  ${LDFLAGS:-}
  $(pkg-config --libs libbsoncxx)
)

echo "Compiling with: ${compile_flags[*]}"
echo "Linking with: ${link_flags[*]}"

rm -rf build/*
cd build
"${CXX:?}" "${compile_flags[@]:?}" -o hello_bsoncxx.o -c ../../../hello_bsoncxx.cpp
"${CXX:?}" -o hello_bsoncxx hello_bsoncxx.o "${link_flags[@]:?}"
./hello_bsoncxx
