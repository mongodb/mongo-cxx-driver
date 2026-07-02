#!/usr/bin/env bash

set -o errexit
set -o pipefail

declare working_dir
working_dir="$(pwd)"

export PATH
PATH="${working_dir:?}/install/bin:${PATH:-}"

# Install prefix to use for ABI compatibility scripts.
[[ -d "${working_dir}/install" ]]

declare parallel_level
parallel_level="$(("$(nproc)" + 1))"

# Obtain abi-compliance-checker.
echo "Fetching abi-compliance-checker..."
[[ -d checker ]] || {
  git clone -b "2.3" --depth 1 https://github.com/lvc/abi-compliance-checker.git checker
  pushd checker
  make -j "${parallel_level:?}" --no-print-directory install prefix="${working_dir:?}/install"
  popd # checker
} >/dev/null
echo "Fetching abi-compliance-checker... done."

# Obtain ctags (required by abi-compliance-checker).
echo "Fetching ctags..."
[[ -d ctags ]] || {
  git clone -b "v6.0.0" --depth 1 https://github.com/universal-ctags/ctags.git ctags
  pushd ctags
  ./autogen.sh
  ./configure --prefix="${working_dir}/install"
  make -j "${parallel_level:?}"
  make install
  popd # ctags
} >/dev/null
echo "Fetching ctags... done."

# Obtain abi-dumper (required by abi-compliance-checker).
echo "Fetching abi-dumper..."
[[ -d abi-dumper ]] || {
  git clone -b "1.4" --depth 1 https://github.com/lvc/abi-dumper.git abi-dumper
  pushd abi-dumper
  make -j "${parallel_level:?}" install prefix="${working_dir:?}/install"
  popd # abi-dumper
} >/dev/null
echo "Fetching abi-dumper... done."

# Obtain vtable-dumper (required by abi-dumper).
echo "Fetching vtable-dumper..."
[[ -d vtable-dumper ]] || {
  git clone -b "1.2" --depth 1 https://github.com/lvc/vtable-dumper.git vtable-dumper
  pushd vtable-dumper
  make -j "${parallel_level:?}" install prefix="${working_dir:?}/install"
  popd # vtable-dumper
} >/dev/null
echo "Fetching vtable-dumper... done."

command -V abi-compliance-checker
command -V abi-dumper
command -V vtable-dumper
