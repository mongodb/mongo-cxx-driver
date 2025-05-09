#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${UV_INSTALL_DIR:?}"

export CC="${cc_compiler:?}"
export CXX="${cxx_compiler:?}"

if [[ "${distro_id:?}" != rhel* ]]; then
  echo "run-clang-tidy.sh expects to be run on a RHEL distro!" 1>&2
  exit 1
fi

if ! command -V parallel >/dev/null; then
  sudo yum install -q -y parallel
fi

PATH="${UV_INSTALL_DIR:?}:${PATH:-}"
uvx clang-tidy --version

. ../mongoc/.evergreen/scripts/find-cmake-latest.sh
cmake_binary="$(find_cmake_latest)"
command -v "${cmake_binary:?}"

# Use ccache if available.
if [[ -f "../mongoc/.evergreen/scripts/find-ccache.sh" ]]; then
  # shellcheck source=/dev/null
  . "../mongoc/.evergreen/scripts/find-ccache.sh"
  find_ccache_and_export_vars "$(pwd)" || true
fi

cmake_config_flags=(
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  -DCMAKE_BUILD_TYPE=Debug
  -DCMAKE_PREFIX_PATH="$(pwd)/../mongoc" # Avoid downloading C Driver.
  -DCMAKE_CXX_STANDARD=17
)

# Generate the compilation database file.
"${cmake_binary:?}" -S . -B build "${cmake_config_flags[@]}"

# Some files (i.e. headers) may need to be generated during the build step.
CMAKE_BUILD_PARALLEL_LEVEL="$(nproc)" "${cmake_binary:?}" --build build

#
# Each check has a name and the checks to run can be chosen using the -checks= option, which specifies a comma-separated
# list of positive and negative (prefixed with -) globs. For example:
#
#    $ clang-tidy test.cpp -checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*
#
# will disable all default checks (-*) and enable all clang-analyzer-* checks except for clang-analyzer-cplusplus* ones.
#
# The -list-checks option lists all the enabled checks. When used without -checks=, it shows checks enabled by default.
# Use -checks=* to see all available checks or with any other value of -checks= to see which checks are enabled by this
# value.
#
# see https://clang.llvm.org/extra/clang-tidy
#

echo "Running clang-tidy with configuration:"
uvx clang-tidy -p=build -dump-config

find_args=(
  -type f
  \( -name *.hh -o -name *.hpp -o -name *.cpp \)    # All sources including headers.
  -not -path "*/third_party/*"                      # Excluding third party headers.
  -not -path "*/config/*.hpp"                       # Excluding config headers.
  -not -path "*bsoncxx/v_noabi/bsoncxx/enums/*.hpp" # Excluding X macro headers.
)

echo "Scanning the following files:"
find src "${find_args[@]}"

# TODO: update clang-tidy config and address warnings.
{
  find src "${find_args[@]}" | parallel uvx clang-tidy --quiet -p=build {} 2>/dev/null
} || true
