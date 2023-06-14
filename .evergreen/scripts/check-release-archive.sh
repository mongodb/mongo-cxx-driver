#!/usr/bin/env bash

set -o errexit # Exit the script with error if any of the commands fail

# Check that a CLion user didn't accidentally convert NEWS from UTF-8 to ASCII
grep "á" NEWS > /dev/null || (echo "NEWS file appears to have lost its UTF-8 encoding?" || exit 1)

type sphinx-build
sphinx-build --version

DIR=$(dirname "$0")
. "$DIR/find-cmake-latest.sh"
CMAKE=$(find_cmake_latest)

python build/calc_release_version.py >VERSION_CURRENT
python build/calc_release_version.py -p >VERSION_RELEASED

# Make the dist tarball outside of source directory to avoid interfering with
# file checks.
mkdir cmake_build
cd cmake_build
$CMAKE -DENABLE_MAN_PAGES=ON -DENABLE_HTML_DOCS=ON -DENABLE_ZLIB=BUNDLED ../
DISTCHECK_BUILD_OPTS="-j 8" $CMAKE --build . --target distcheck

# Check that docs were included, but sphinx temp files weren't.
tarfile=mongo-c-driver-*.tar.gz
docs='mongo-c-driver-*/doc/html/index.html mongo-c-driver-*/doc/man/mongoc_client_t.3'
tmpfiles='mongo-c-driver-*/doc/html/.doctrees \
   mongo-c-driver-*/doc/html/.buildinfo \
   mongo-c-driver-*/doc/man/.doctrees \
   mongo-c-driver-*/doc/man/.buildinfo'

echo "Checking for built docs"
for doc in $docs; do
  # Check this doc is in the archive.
  tar --wildcards -tzf $tarfile $doc
done

echo "Checking that temp files are not included in tarball"
for tmpfile in $tmpfiles; do
  # Check this temp file doesn't exist.
  if tar --wildcards -tzf $tarfile $tmpfile >/dev/null 2>&1; then
    echo "Found temp file in archive: $tmpfile"
    exit 1
  fi
done

echo "Checking that index.3 wasn't built"
if tar --wildcards -tzf $tarfile 'mongo-c-driver-*/doc/man/index.3' >/dev/null 2>&1; then
  echo "Found index.3 in archive"
  exit 1
fi

# Back to the repo source directory.
cd ..

python .evergreen/scripts/check-files.py ./src/ cmake_build/mongo-c-driver-*/src
