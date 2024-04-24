#!/usr/bin/env bash

find_scan_build() {
  # scan-build binary is available in different locations depending on the distro.
  # Search for a match in order of preference as listed.
  declare -a scan_build_directories

  # Prioritize Apple LLVM on MacOS to avoid confusing CMake with inconsistent
  # compilers and linkers.
  if [[ -d /usr/local/Cellar/llvm ]]; then
    for dir in /opt/homebrew/Cellar/llvm /usr/local/Cellar/llvm; do
      # Max depth: llvm/bin/scan-build. Sort: prefer newer versions.
      for bin in $(find "${dir}" -maxdepth 3 -name 'scan-build' 2>/dev/null | sort -rV); do
        if command -v "${bin}"; then
          scan_build_directories+=("$(dirname "${bin}")")
        fi
      done
    done
  fi

  scan_build_directories+=(
    # Prefer toolchain scan-build if available.
    "/opt/mongodbtoolchain/v4/bin"
    "/opt/mongodbtoolchain/v3/bin"

    # Use system scan-build otherwise.
    "/usr/bin"
  )

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

  echo "could not find a scan-build binary!" 1>&2

  return 1
}
