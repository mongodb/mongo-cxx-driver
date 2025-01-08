#!/usr/bin/env bash
#
# clang-format-all.sh
#
# Usage:
#   uv run --frozen etc/clang-format-all.sh
#
# This script is meant to be run from the project root directory.

: "${DRYRUN:-}"

set -o errexit
set -o pipefail

clang_format_binary="${CLANG_FORMAT_BINARY:-clang-format}"
"${clang_format_binary:?}" --version

source_dirs=(
  src
  benchmark
  examples
)

mapfile -t source_files < <(
  find "${source_dirs[@]:?}" | grep -E '.*\.(hpp|hh|cpp)$'
)

if [[ -n "${DRYRUN:-}" ]]; then
  "$clang_format_binary" --dry-run -Werror "${source_files[@]:?}"
else
  "$clang_format_binary" --verbose -i "${source_files[@]:?}"
fi
