#!/usr/bin/env bash

# This script is used to fetch the latest JSON tests for the CRUD spec. It puts the tests in the
# direcory $reporoot/data/retryable-reads. It should be run from the root of the repository.

set -o errexit
set -o nounset

if [ ! -d ".git" ]; then
    echo "$0: This script must be run from the root of the repository" >&2
    exit 1
fi

tmpdir=`perl -MFile::Temp=tempdir -wle 'print tempdir(TMPDIR => 1, CLEANUP => 0)'`
curl -sL https://github.com/mongodb/specifications/archive/master.zip -o "$tmpdir/specs.zip"
unzip -d "$tmpdir" "$tmpdir/specs.zip" > /dev/null
mkdir -p data/retryable-reads
rsync -ah "$tmpdir/specifications-master/source/retryable-reads/tests/" data/retryable-reads
( cd data/retryable-reads && find * -name \*.json | sort > test_files.txt )
rm -rf "$tmpdir"
