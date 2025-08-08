#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${cxx_standard:?}"
: "${distro_id:?}"
: "${polyfill:?}"
: "${UV_INSTALL_DIR:?}"

command -V git >/dev/null

# Files prepared by EVG config.
[[ -d "mongoc" ]] || {
  echo "missing mongoc" 1>&2
  exit 1
}
[[ -d "mongo-cxx-driver" ]] || {
  echo "missing mongo-cxx-driver" 1>&2
  exit 1
}

# For latest Clang versions supporting recent C++ standards.
export CC CXX
case "${distro_id:?}" in
rhel95*)
  CC="clang-19"
  CXX="clang++-19"
  ;;
ubuntu22*)
  CC="clang-12"
  CXX="clang++-12"
  ;;
*)
  echo "unexpected distro: ${distro_id:?}" 1>&2
  exit 1
  ;;
esac

declare working_dir
working_dir="$(pwd)"

. mongo-cxx-driver/.evergreen/scripts/install-build-tools.sh
install_build_tools

# Use ccache if available.
if [[ -f "./mongoc/.evergreen/scripts/find-ccache.sh" ]]; then
  # shellcheck source=/dev/null
  . "./mongoc/.evergreen/scripts/find-ccache.sh"
  find_ccache_and_export_vars "$(pwd)" || true
fi

# To use a different base commit, replace `--abbrev 0` with the intended commit.
# Note: EVG treat all changes relative to the EVG base commit as staged changes!
declare base current
base="$(git -C mongo-cxx-driver describe --tags --abbrev=0)"
current="$(git -C mongo-cxx-driver describe --tags)"

echo "Old Version (Base): ${base:?}"
echo "New Version (Current): ${current:?}"

printf "%s" "${base:?}" >base-commit.txt
printf "%s" "${current:?}" >current-commit.txt

# Remove 'r' prefix in version string.
declare old_ver new_ver
old_ver="${base:1}"
new_ver="${current:1}"

# Install prefix to use for ABI compatibility scripts.
mkdir -p "${working_dir}/install"

# As encouraged by ABI compatibility checkers.
export CFLAGS CXXFLAGS
CFLAGS="-g -Og"
CXXFLAGS="-g -Og"

# Common configuration flags.
configure_flags=(
  "-DCMAKE_PREFIX_PATH=${working_dir:?}/mongoc"
  "-DCMAKE_CXX_STANDARD=${cxx_standard:?}"
  "-DCMAKE_INSTALL_LIBDIR=lib" # Avoid dealing with lib vs. lib64.
)

# Polyfill library selection.
case "${polyfill:?}" in
impls) configure_flags+=("-DBSONCXX_POLY_USE_IMPLS=ON") ;;
stdlib) configure_flags+=("-DBSONCXX_POLY_USE_STD=ON") ;;
*)
  echo "invalid polyfill: ${polyfill:?}"
  exit 1
  ;;
esac

# Build and install the base commit first.
git -C mongo-cxx-driver stash push -u
git -C mongo-cxx-driver reset --hard "${base:?}"

# Install old (base) to install/old.
echo "Building old libraries..."
(
  cmake \
    -S mongo-cxx-driver \
    -B build/old \
    -DCMAKE_INSTALL_PREFIX=install/old \
    -DBUILD_VERSION="${old_ver:?}-base" \
    "${configure_flags[@]:?}" || exit
  cmake --build build/old || exit
  cmake --install build/old || exit
) &>old.log || {
  cat old.log 1>&2
  exit 1
}
echo "Building old libraries... done."

# Restore all pending changes.
git -C mongo-cxx-driver reset --hard "HEAD@{1}"
git -C mongo-cxx-driver stash pop -q || true # Only patch builds have stashed changes.

# Install new (current) to install/new.
echo "Building new libraries..."
(
  cmake \
    -S mongo-cxx-driver \
    -B build/new \
    -DCMAKE_INSTALL_PREFIX=install/new \
    -DBUILD_VERSION="${new_ver:?}-current" \
    "${configure_flags[@]:?}" || exit
  cmake --build build/new || exit
  cmake --install build/new || exit
) &>new.log || {
  cat new.log 1>&2
  exit 1
}
echo "Building new libraries... done."
