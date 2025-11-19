#!/usr/bin/env bash

set -o errexit
set -o pipefail

export CC="${cc_compiler:?}"
export CXX="${cxx_compiler:?}"

if [[ "${distro_id:?}" != rhel* ]]; then
  echo "run-clang-tidy.sh expects to be run on a RHEL distro!" 1>&2
  exit 1
fi

# Required to execute clang-tidy commands in parallel.
command -V parallel >/dev/null

# shellcheck source=/dev/null
. .evergreen/scripts/install-build-tools.sh
install_build_tools
export CMAKE_GENERATOR="Ninja"

uv tool install -q clang-tidy
version="$(clang-tidy --version | perl -lne 'print $1 if m|LLVM version (\d+\.\d+\.\d+)|')"
echo "clang-tidy version: ${version:?}"

# Use ccache if available.
if [[ -f "../mongoc/.evergreen/scripts/find-ccache.sh" ]]; then
  # shellcheck source=/dev/null
  . "../mongoc/.evergreen/scripts/find-ccache.sh"
  find_ccache_and_export_vars "$(pwd)" || true
fi

cmake_config_flags=(
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  -DCMAKE_BUILD_TYPE=Debug
  "-DCMAKE_PREFIX_PATH=$(pwd)/../mongoc" # Avoid downloading C Driver.
  -DCMAKE_CXX_STANDARD=17
)

# Generate the compilation database file.
cmake "${cmake_config_flags[@]}" -B build

mapfile -t sources < <(find src -type f \( -name *.cc -o -name *.cpp \) | perl -lne 'print if m$.*/(?:bsoncxx|mongocxx)/lib/.*$')

clang_tidy_flags=(
  --quiet
  -p build
  --header-filter '.*/(?:bsoncxx|mongocxx)/(?:include|lib)/.*'
)

printf "%s\n" "${sources[@]}" | parallel -q "${parallel_flags[@]}" clang-tidy "${clang_tidy_flags[@]:?}" {} 2>/dev/null
