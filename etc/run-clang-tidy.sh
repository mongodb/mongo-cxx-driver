#!/usr/bin/env bash

set -o errexit
set -o pipefail

export CC="${cc_compiler:?}"
export CXX="${cxx_compiler:?}"

if [[ "${distro_id:?}" != rhel* ]]; then
  echo "run-clang-tidy.sh expects to be run on a RHEL distro!" 1>&2
  exit 1
fi

# shellcheck source=/dev/null
. .evergreen/scripts/install-build-tools.sh
install_build_tools
export CMAKE_GENERATOR="Ninja"

uv tool install -q clang-tidy
version="$(clang-tidy --version | perl -lne 'print $1 if m|LLVM version (\d+\.\d+\.\d+)|')"
echo "clang-tidy version: ${version:?}"

# Obtain the run-clang-tidy.py script.
bindir="$(mktemp -d)"
curl -sSL -o "${bindir:?}/run-clang-tidy.py" "https://raw.githubusercontent.com/llvm/llvm-project/refs/tags/llvmorg-${version:?}/clang-tools-extra/clang-tidy/tool/run-clang-tidy.py"
checksum=a651a6529eefbd12b7845afe6719773ba6578ecca222603d1262b4d2d48e1422 # 22.1.*
echo "${checksum:?}" "${bindir:?}/run-clang-tidy.py" | sha256sum -c >/dev/null

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

flags=(
  -quiet
  -p build

  # Restrict analysis to library sources.
  -source-filter '.*/src/(?:bsoncxx|mongocxx)/lib/.*'
  -header-filter '.*/src/(?:bsoncxx|mongocxx)/(?:include|lib)/.*'
)

# Use the parallel clang-tidy runner.
uv run --script "${bindir:?}/run-clang-tidy.py" "${flags[@]:?}"
