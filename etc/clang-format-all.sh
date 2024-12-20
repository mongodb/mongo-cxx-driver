#!/usr/bin/env bash
#
# clang-format-all.sh
#
# Usage:
#   uv run --frozen etc/clang-format-all
#
# This script is meant to be run from the project root directory.

: "${DRYRUN:-}"

set -o errexit
set -o pipefail

clang-format --version

source_dirs=(
  src
  benchmark
  examples
)

mapfile -t source_files < <(
  find "${source_dirs[@]:?}" -regextype posix-egrep -regex '.*\.(hpp|hh|cpp)'
)

if [[ -n "${DRYRUN:-}" ]]; then
  clang-format --dry-run -Werror "${source_files[@]:?}"
else
  clang-format --verbose -i "${source_files[@]:?}"
fi
