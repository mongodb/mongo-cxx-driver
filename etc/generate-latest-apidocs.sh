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

LATEST_VERSION="4.0.0"
DOXYGEN_VERSION_REQUIRED="1.12.0"

# Permit using a custom Doxygen binary.
: "${DOXYGEN_BINARY:=doxygen}"

command -v git >/dev/null
command -v mktemp >/dev/null
command -v sed >/dev/null

if [[ ! -d build ]]; then
  echo "missing build directory: this script must be run from the project root directory" 1>&2
  exit 1
fi

# Validate Doxygen version.
doxygen_version="$("${DOXYGEN_BINARY:?}" -v | perl -lne 'print $1 if m/^(\d+\.\d+\.\d+).*$/')"
if [[ "${doxygen_version:-}" != "${DOXYGEN_VERSION_REQUIRED:?}" ]]; then
  echo "${DOXYGEN_BINARY} version ${doxygen_version:-"unknown"} does not equal ${DOXYGEN_VERSION_REQUIRED:?}" 1>&2
  exit 1
fi

working_dir="$(pwd)"
apidocpath="${working_dir:?}/build/docs/api/mongocxx-${LATEST_VERSION:?}"

# Use a temporary directory for the following operations.
tmpdir="$(mktemp -d)"
trap 'rm -rf "${tmpdir:?}"' EXIT

mkdir -p "${apidocpath:?}"

# Use a clean copy of the repository.
git clone -q -c advice.detachedHead=false -b "r${LATEST_VERSION}" . "${tmpdir}"

cd "${tmpdir:?}"

# Update the Doxyfile configuration file:
#  - set OUTPUT_DIRECTORY to `build/docs/api/mongocxx-<version>`.
#  - set PROJECT_NUMBER to `<version>`.
sed -i \
  -e "s|^OUTPUT_DIRECTORY\s*=\s*.*$|OUTPUT_DIRECTORY = ${apidocpath:?}|g" \
  -e "s|^PROJECT_NUMBER\s*=\s*.*$|PROJECT_NUMBER = ${LATEST_VERSION:?}|g" \
  Doxyfile

# Generate API documentation.
(
  set -o xtrace

  cmake -S . -B build -D "DOXYGEN_EXECUTABLE=${DOXYGEN_BINARY:?}" --log-level=WARNING
  cmake --build build --target docs_source_directory
  "${DOXYGEN_BINARY:?}"
)
