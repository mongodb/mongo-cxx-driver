#!/usr/bin/env bash

set -o errexit
set -o pipefail

declare working_dir
working_dir="$(pwd)"

export PATH
PATH="${working_dir:?}/install/bin:${PATH:-}"

# Install prefix to use for ABI compatibility scripts.
[[ -d "${working_dir:?}/install" ]]

# Obtain libabigail.
echo "Fetching libabigail..."
[[ -d libabigail ]] || (
  # libabigail dependencies.
  sudo apt-get install -q -y libdw-dev libelf-dev libxxhash-dev >/dev/null

  archive="libabigail.tar.xz"
  curl -sSL -o "${archive:?}" https://mirrors.kernel.org/sourceware/libabigail/libabigail-2.9.tar.xz
  checksum="5bdf5ec49a5931a61bf28317b41eee583d6277d00ac621b2d2a97bbc0d816c3662bcfe13a5ac7aeee11c947afb69a5a0a9a8015fcebad09965b45af9b1e23606"
  echo "${checksum:?}" "${archive:?}" | sha512sum -c >/dev/null
  tar xJf "${archive:?}" --one-top-level=libabigail --strip-components=1

  cd libabigail
  mkdir build
  cd build

  ../configure --prefix="${working_dir:?}/install"
  MAKEFLAGS+=" -j $(nproc)" make all
  make install
) >/dev/null
echo "Fetching libabigail... done."

command -V abidiff
