#!/usr/bin/env bash

set -o errexit
set -o pipefail

command -V pkgconf

# Sanity-check that static library macros are not set when building against the shared library.
# Users don't need to include this section in their projects.
if ! pkgconf --cflags "lib${MONGOCXX_BASENAME:?}" | grep -v -q -- -DBSONCXX_STATIC; then
  echo "Expected BSONCXX_STATIC to not be set" >&2
  exit 1
fi

if ! pkgconf --cflags "lib${MONGOCXX_BASENAME:?}" | grep -v -q -- -DMONGOCXX_STATIC; then
  echo "Expected MONGOCXX_STATIC to not be set" >&2
  exit 1
fi

compile_flags=(
  "-std=c++${CXX_STANDARD:?}"
  -Wall -Wextra -Werror
  ${CXXFLAGS:-}
  $(pkg-config --cflags "lib${MONGOCXX_BASENAME:?}")
)

link_flags=(
  ${LDFLAGS:-}
  $(pkg-config --libs "lib${MONGOCXX_BASENAME:?}")
)

echo "Compiling with: ${compile_flags[*]}"
echo "Linking with: ${link_flags[*]}"

rm -rf build/*
cd build
"${CXX:?}" "${compile_flags[@]:?}" -o hello_mongocxx.o -c ../../../hello_mongocxx.cpp
"${CXX:?}" -o hello_mongocxx hello_mongocxx.o "${link_flags[@]:?}"
./hello_mongocxx
