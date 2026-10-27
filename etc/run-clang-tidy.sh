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
export UV_FROZEN='1'

version="$(uv run --only-group tidy -- clang-tidy --version | perl -lne 'print $1 if m|LLVM version (\d+\.\d+\.\d+)|')"
echo "clang-tidy version: ${version:?}"

# Ensure the run-clang-tidy.py script is consistent with the clang-tidy version.
# Helper script is located at <prefix>/clang-tidy/data/bin/run-clang-tidy.py.
# See: https://github.com/ssciwr/clang-tidy-wheel/blob/741851932dba91accf68c29f31b0dccf30567e06/CMakeLists.txt#L49-L57
script="$(
  cat <<'DOC'
  import clang_tidy, os
  print(os.path.join(os.path.dirname(clang_tidy.__file__), "data", "bin", "run-clang-tidy.py"))
DOC
)"
runner="$(uv run --only-group tidy -- python -c "${script:?}")"
if [[ ! -f "${runner:?}" ]]; then
  echo "missing run-clang-tidy.py: ${runner:?}" 1>&2
  exit 1
fi

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
uv run --only-group tidy -- python "${runner:?}" "${flags[@]:?}"
