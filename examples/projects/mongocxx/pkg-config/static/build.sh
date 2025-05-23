#!/usr/bin/env bash

set -o errexit
set -o pipefail

# Sanity-check that static library macros are set when building against the static library.  Users
# don't need to include this section in their projects.
if ! pkg-config --cflags libmongocxx-static | grep -q -- -DBSONCXX_STATIC; then
  echo "Expected BSONCXX_STATIC to be set" >&2
  exit 1
fi

if ! pkg-config --cflags libmongocxx-static | grep -q -- -DMONGOCXX_STATIC; then
  echo "Expected MONGOCXX_STATIC to be set" >&2
  exit 1
fi

# Sanity-check that static libmongoc is required. Regression test for CXX-3290.
if ! pkg-config --print-requires libmongocxx-static | grep -q -- mongoc2-static; then
  echo "Expected mongoc2-static to be required" >&2
  exit 1
fi

rm -rf build/*
cd build
"${CXX:?}" $CXXFLAGS -Wall -Wextra -Werror -std="c++${CXX_STANDARD:?}" -c -o hello_mongocxx.o ../../../hello_mongocxx.cpp $(pkg-config --cflags libmongocxx-static)
# TODO: remove `-pthread` once CDRIVER-4776 is resolved.
"${CXX:?}" $LDFLAGS -pthread -std="c++${CXX_STANDARD:?}" -o hello_mongocxx hello_mongocxx.o $(pkg-config --libs libmongocxx-static)
./hello_mongocxx
