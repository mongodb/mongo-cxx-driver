#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${install_prefix:?}"

cd "$(mktemp -d)"

# https://valgrind.org/downloads/current.html
curl -sSL -m 60 --retry 5 -o valgrind-3.24.0.tar.bz2 https://sourceware.org/pub/valgrind/valgrind-3.24.0.tar.bz2
cat >checksum.txt <<<'6fc0470fedc0d85dae3e042297cabd13c6100749 *valgrind-3.24.0.tar.bz2'
sha1sum -c checksum.txt >/dev/null

tar -xjf valgrind-3.24.0.tar.bz2
cd valgrind-3.24.0

# https://valgrind.org/docs/manual/manual-core.html#manual-core.install
./configure --prefix "${install_prefix:?}" >/dev/null
make --no-print-directory -j "$(nproc)" >/dev/null
make --no-print-directory install >/dev/null
