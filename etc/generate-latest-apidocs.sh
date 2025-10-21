#!/usr/bin/env bash
#
# generate-latest-apidocs.sh
#
# Usage:
#   ./etc/generate-latest-apidocs.sh
#
# This script is meant to be run from the project root directory.

set -o errexit
set -o pipefail

LATEST_VERSION="4.1.4"
DOXYGEN_VERSION_REQUIRED="1.14.0"

# Permit using a custom Doxygen binary.
: "${DOXYGEN_BINARY:=doxygen}"

# Use the current repository instead of a clean temporary repository.
: "${DOXYGEN_USE_CURRENT:=0}"

# Permit using any Doxygen version.
: "${DOXYGEN_ANY_VERSION:=0}"

if [[ ! -d build ]]; then
  echo "missing build directory: this script must be run from the project root directory" 1>&2
  exit 1
fi

# Validate Doxygen version.
doxygen_version="$("${DOXYGEN_BINARY:?}" -v | perl -lne 'print $1 if m/^(\d+\.\d+\.\d+).*$/')"
if [[ "${doxygen_version:-}" != "${DOXYGEN_VERSION_REQUIRED:?}" ]]; then
  if [[ "${DOXYGEN_ANY_VERSION:?}" == 1 ]]; then
    echo "using ${DOXYGEN_BINARY:?} version ${doxygen_version:-"unknown"} which does not equal ${DOXYGEN_VERSION_REQUIRED:?}" 1>&2
  else
    echo "${DOXYGEN_BINARY} version ${doxygen_version:-"unknown"} does not equal ${DOXYGEN_VERSION_REQUIRED:?}" 1>&2
    exit 1
  fi
fi

working_dir="$(pwd)"
apidocspath="${working_dir:?}/build/docs/api"

if [[ "${DOXYGEN_USE_CURRENT:?}" == 1 ]]; then
  # Use the current repository's build directory.
  output_directory="${apidocspath:?}/current"
  scratch_dir="$(pwd)"
else
  # Check required tools exist.
  command -V git >/dev/null
  command -V mktemp >/dev/null
  command -V sed >/dev/null

  # Use a clean copy of the repository.
  output_directory="${apidocspath:?}/mongocxx-${LATEST_VERSION:?}"
  scratch_dir="$(mktemp -d)"
  trap 'rm -rf "${scratch_dir:?}"' EXIT

  git clone -q -c advice.detachedHead=false -b "r${LATEST_VERSION}" . "${scratch_dir:?}"

  function sed_in_place {
    if [[ "${OSTYPE}" == darwin* ]]; then
      # macOS uses FreeBSD sed.
      sed -i '' "$@"
    else
      sed -i'' "$@"
    fi
  }

  # Update the Doxyfile configuration file:
  #  - set OUTPUT_DIRECTORY to `build/docs/api/mongocxx-<version>`.
  #  - set PROJECT_NUMBER to `<version>`.
  sed_in_place \
    -e "s|^OUTPUT_DIRECTORY[[:space:]]*=[[:space:]]*.*$|OUTPUT_DIRECTORY = ${output_directory:?}|g" \
    -e "s|^PROJECT_NUMBER[[:space:]]*=[[:space:]]*.*$|PROJECT_NUMBER = ${LATEST_VERSION:?}|g" \
    "${scratch_dir:?}/Doxyfile"
fi

mkdir -p "${output_directory:?}"

# Generate API documentation.
(
  cd "${scratch_dir:?}"

  set -o xtrace

  cmake -S . -B build --log-level=WARNING
  cmake --build build --target doxygen-install-headers
  "${DOXYGEN_BINARY:?}"
)
