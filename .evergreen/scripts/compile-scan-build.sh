#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${BSONCXX_POLYFILL:-}"
: "${CXX_STANDARD:?}"
: "${UV_INSTALL_DIR:?}"

mongoc_prefix="$(pwd)/../mongoc"

# Obtain preferred build tools.
export UV_TOOL_DIR UV_TOOL_BIN_DIR
UV_TOOL_DIR="$(pwd)/uv-tool"
UV_TOOL_BIN_DIR="$(pwd)/uv-bin"
PATH="${UV_TOOL_BIN_DIR:?}:${UV_INSTALL_DIR:?}:${PATH:-}"
uv tool install -q cmake
[[ "${distro_id:?}" == rhel* ]] && PATH="${PATH:-}:/opt/mongodbtoolchain/v4/bin" || uv tool install -q ninja
export CMAKE_GENERATOR
CMAKE_GENERATOR="Ninja"

cmake --version | head -n 1
echo "ninja version: $(ninja --version)"

# Use ccache if available.
if [[ -f "../mongoc/.evergreen/scripts/find-ccache.sh" ]]; then
  # shellcheck source=/dev/null
  . "../mongoc/.evergreen/scripts/find-ccache.sh"
  find_ccache_and_export_vars "$(pwd)" || true
fi

# scan-build binary is available in different locations depending on the distro.
# Search for a match in order of preference as listed.
declare -a scan_build_directories

scan_build_directories+=(
  # Prefer toolchain scan-build if available.
  "/opt/mongodbtoolchain/v4/bin"
  "/opt/mongodbtoolchain/v3/bin"
)

# Use system scan-build otherwise.
IFS=: read -ra sys_dirs <<<"${PATH:-}"
scan_build_directories+=("${sys_dirs[@]:-}")

declare scan_build_binary
for dir in "${scan_build_directories[@]}"; do
  if command -v "${dir}/scan-build" && command -v "${dir}/clang" && command -v "${dir}/clang++"; then
    # Ensure compilers are consistent with scan-build binary. All three binaries
    # should be present in the same directory.
    scan_build_binary="${dir}/scan-build"
    CC="${dir}/clang"
    CXX="${dir}/clang++"
    break
  fi
done
: "${scan_build_binary:?"could not find a scan-build binary!"}"
export CC
export CXX

cmake_flags=(
  "-DCMAKE_BUILD_TYPE=Debug"
  "-DCMAKE_CXX_STANDARD=${CXX_STANDARD:?}"
  "-DCMAKE_CXX_STANDARD_REQUIRED=ON"
)

scan_build_flags=(
  --use-cc "${CC:?}"
  --use-c++ "${CXX:?}"
  -analyze-headers
  --exclude "$(pwd)/build/_deps" # mongoc
)

case "${BSONCXX_POLYFILL:-}" in
impls) cmake_flags+=("-DBSONCXX_POLY_USE_IMPLS=ON") ;;
std) cmake_flags+=("-DBSONCXX_POLY_USE_STD=ON") ;;
esac

echo "Configuring with CMake flags:"
printf " - %s\n" "${cmake_flags[@]}"

# Configure via scan-build for consistency.
CCCACHE_DISABLE=1 "${scan_build_binary}" "${scan_build_flags[@]}" cmake -S . -B build "${cmake_flags[@]}"

# If scan-build emits warnings, continue the task and upload scan results before marking task as a failure.
declare -r continue_command='{"status":"failed", "type":"test", "should_continue":true, "desc":"scan-build emitted one or more warnings or errors"}'

# Put clang static analyzer results in scan/ and fail build if warnings found.
"${scan_build_binary}" "${scan_build_flags[@]}" -o scan --status-bugs cmake --build build ||
  curl -sS -d "${continue_command}" -H "Content-Type: application/json" -X POST localhost:2285/task_status
