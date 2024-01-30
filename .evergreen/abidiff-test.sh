#!/usr/bin/env bash

set -o errexit
set -o pipefail

declare working_dir
working_dir="$(pwd)"

export PATH
PATH="${working_dir:?}/install/bin:${PATH:-}"

declare -a common_flags
flags=(
  --headers-dir1 install/old/include
  --headers-dir2 install/new/include
  --non-reachable-types
  --fail-no-debug-info
)

declare -a abi_flags=("${common_flags[@]}" --suppressions cxx-abi/abignore)
declare -a noabi_flags=("${common_flags[@]}" --suppressions cxx-noabi/abignore)

command -V abidiff >/dev/null

mkdir cxx-abi cxx-noabi

cat >cxx-abi/abignore <<DOC
[suppress_type]
name_not_regexp = ^(bsoncxx|mongocxx)::v[[:digit:]]+::

[suppress_function]
name_not_regexp = ^(bsoncxx|mongocxx)::v[[:digit:]]+::

[suppress_variable]
name_not_regexp = ^(bsoncxx|mongocxx)::v[[:digit:]]+::
DOC

cat >cxx-noabi/abignore <<DOC
[suppress_type]
name_regexp = ^(bsoncxx|mongocxx)::v[[:digit:]]+::

[suppress_function]
name_regexp = ^(bsoncxx|mongocxx)::v[[:digit:]]+::

[suppress_variable]
name_regexp = ^(bsoncxx|mongocxx)::v[[:digit:]]+::
DOC

# Allow task to upload the diff reports despite failed status.
echo "Comparing stable ABI for bsoncxx..."
if ! abidiff "${abi_flags[@]}" install/old/lib/libbsoncxx.so install/new/lib/libbsoncxx.so &>cxx-abi/bsoncxx.txt; then
  declare status
  status='{"status":"failed", "type":"test", "should_continue":true, "desc":"abidiff returned an error for bsoncxx (stable)"}'
  curl -sS -d "${status:?}" -H "Content-Type: application/json" -X POST localhost:2285/task_status || true
fi
echo "Comparing stable ABI for bsoncxx... done."

# Allow task to upload the diff reports despite failed status.
echo "Comparing stable ABI for mongocxx..."
if ! abidiff "${abi_flags[@]}" install/old/lib/libmongocxx.so install/new/lib/libmongocxx.so &>cxx-abi/mongocxx.txt; then
  declare status
  status='{"status":"failed", "type":"test", "should_continue":true, "desc":"abidiff returned an error for mongocxx (stable)"}'
  curl -sS -d "${status:?}" -H "Content-Type: application/json" -X POST localhost:2285/task_status || true
fi
echo "Comparing stable ABI for mongocxx... done."

echo "Comparing unstable ABI for bsoncxx..."
abidiff "${noabi_flags[@]}" install/old/lib/libbsoncxx.so install/new/lib/libbsoncxx.so &>cxx-noabi/bsoncxx.txt || true
echo "Comparing unstable ABI for bsoncxx... done."

echo "Comparing unstable ABI for mongocxx..."
abidiff "${noabi_flags[@]}" install/old/lib/libmongocxx.so install/new/lib/libmongocxx.so &>cxx-noabi/mongocxx.txt || true
echo "Comparing unstable ABI for mongocxx... done."

# Ensure files have content even when abidiff produces no output.
printf "\n" >>cxx-abi/bsoncxx.txt
printf "\n" >>cxx-abi/mongocxx.txt
printf "\n" >>cxx-noabi/bsoncxx.txt
printf "\n" >>cxx-noabi/mongocxx.txt
