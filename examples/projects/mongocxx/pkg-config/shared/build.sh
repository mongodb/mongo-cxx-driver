#!/usr/bin/env bash

set -o errexit
set -o pipefail

# Sanity-check that static library macros are not set when building against the shared library.
# Users don't need to include this section in their projects.
(pkg-config --cflags libmongocxx | grep -v -- -DBSONCXX_STATIC) ||
  (
    echo "Expected BSONCXX_STATIC to not be set" >&2
    exit 1
  )
(pkg-config --cflags libmongocxx | grep -v -- -DMONGOCXX_STATIC) ||
  (
    echo "Expected MONGOCXX_STATIC to not be set" >&2
    exit 1
  )

rm -rf build/*
cd build

"${CXX:?}" $CXXFLAGS -Wall -Wextra -Werror -std="c++${CXX_STANDARD:?}" -c -o hello_mongocxx.o ../../../hello_mongocxx.cpp $(pkg-config --cflags libmongocxx)

"${CXX:?}" $LDFLAGS -std="c++${CXX_STANDARD:?}" -o hello_mongocxx hello_mongocxx.o $(pkg-config --libs libmongocxx)

./hello_mongocxx
