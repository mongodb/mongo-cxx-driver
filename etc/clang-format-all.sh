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

CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"
"$CLANG_FORMAT" --version

source_dirs=(
  src
  benchmark
  examples
)

mapfile -t source_files < <(
  find "${source_dirs[@]:?}" | grep -E '.*\.(hpp|hh|cpp)$'
)

if [[ -n "${DRYRUN:-}" ]]; then
  "$CLANG_FORMAT" --dry-run -Werror "${source_files[@]:?}"
else
  "$CLANG_FORMAT" --verbose -i "${source_files[@]:?}"
fi
