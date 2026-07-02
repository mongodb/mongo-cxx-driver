#!/usr/bin/env bash

set -o errexit
set -o pipefail

# Sanity-check that static library macros are set when building against the static library.  Users
# don't need to include this section in their projects.
if ! pkgconf --cflags "lib${BSONCXX_BASENAME:?}-static" | grep -q -- -DBSONCXX_STATIC; then
  echo "Expected BSONCXX_STATIC to be set" >&2
  exit 1
fi

# Sanity-check that static libbson is required. Regression test for CXX-3290.
if ! pkgconf --print-requires "lib${BSONCXX_BASENAME:?}-static" | grep -q -- bson2-static; then
  echo "Expected bson2-static to be required" >&2
  exit 1
fi

compile_flags=(
  "-std=c++${CXX_STANDARD:?}"
  -Wall -Wextra -Werror
  ${CXXFLAGS:-}
  $(pkg-config --cflags "lib${BSONCXX_BASENAME:?}-static")
)

link_flags=(
  ${LDFLAGS:-}
  $(pkg-config --libs "lib${BSONCXX_BASENAME:?}-static")
)

echo "Compiling with: ${compile_flags[*]}"
echo "Linking with: ${link_flags[*]}"

rm -rf build/*
cd build
"${CXX:?}" "${compile_flags[@]:?}" -o hello_bsoncxx.o -c ../../../hello_bsoncxx.cpp
"${CXX:?}" -o hello_bsoncxx hello_bsoncxx.o "${link_flags[@]:?}"
./hello_bsoncxx
