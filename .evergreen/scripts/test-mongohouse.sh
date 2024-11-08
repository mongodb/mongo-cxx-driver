#!/usr/bin/env bash

set -o errexit
set -o pipefail

echo "Waiting for mongohouse to start..."
wait_for_mongohouse() {
  for _ in $(seq 300); do
    # Exit code 7: "Failed to connect to host".
    if
      curl -s -m 1 "localhost:${1:?}"
      (("$?" != 7))
    then
      return 0
    else
      sleep 1
    fi
  done
  echo "Could not detect mongohouse on port ${1:?}" 1>&2
  return 1
}
wait_for_mongohouse 27017 || exit
echo "Waiting for mongohouse to start... done."
pgrep mongohouse

cd build
export PREFIX
PREFIX="$(pwd)/../../mongoc"

# Use LD_LIBRARY_PATH to inform the tests where to find dependencies on Linux.
# This task only runs on Linux.
if [ -n "${lib_dir:-}" ]; then
  export LD_LIBRARY_PATH=".:${PREFIX:?}/${lib_dir:?}/"
else
  export LD_LIBRARY_PATH=".:${PREFIX:?}/lib/"
fi

export MONGOHOUSE_TESTS_PATH
MONGOHOUSE_TESTS_PATH="$(pwd)/../data/mongohouse"

export RUN_MONGOHOUSE_TESTS=ON

ulimit -c unlimited || true

./src/mongocxx/test/test_mongohouse_specs
