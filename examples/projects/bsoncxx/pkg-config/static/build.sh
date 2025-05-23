#!/usr/bin/env bash

set -o errexit
set -o pipefail

# Sanity-check that static library macros are set when building against the static library.  Users
# don't need to include this section in their projects.
if ! pkg-config --cflags libbsoncxx-static | grep -q -- -DBSONCXX_STATIC; then
  echo "Expected BSONCXX_STATIC to be set" >&2
  exit 1
fi

# Sanity-check that static libbson is required. Regression test for CXX-3290.
if ! pkg-config --print-requires libbsoncxx-static | grep -q -- bson2-static; then
  echo "Expected bson2-static to be required" >&2
  exit 1
fi

rm -rf build/*
cd build
"${CXX:?}" $CXXFLAGS -Wall -Wextra -Werror -std="c++${CXX_STANDARD:?}" -c -o hello_bsoncxx.o ../../../hello_bsoncxx.cpp $(pkg-config --cflags libbsoncxx-static)
# TODO: remove `-pthread` once CDRIVER-4776 is resolved.
"${CXX:?}" $LDFLAGS -pthread -std="c++${CXX_STANDARD:?}" -o hello_bsoncxx hello_bsoncxx.o $(pkg-config --libs libbsoncxx-static)
./hello_bsoncxx
