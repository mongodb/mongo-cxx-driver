#!/usr/bin/env bash

# This script is used to fetch transactions latest JSON tests for the CRUD spec. It puts the tests
# in the directory $reporoot/data/transactions. It also syncs the transactions-convenient-api tests
# and places those in $reporoot/data/with_transaction.
#
# This script should be run from the root of the repository.

set -o errexit
set -o nounset

if [ ! -d ".git" ]; then
    echo "$0: This script must be run from the root of the repository" >&2
    exit 1
fi

tmpdir=`perl -MFile::Temp=tempdir -wle 'print tempdir(TMPDIR => 1, CLEANUP => 0)'`
curl -sL https://github.com/mongodb/specifications/archive/master.zip -o "$tmpdir/specs.zip"
unzip -d "$tmpdir" "$tmpdir/specs.zip" > /dev/null

# Sync transactions tests
mkdir -p data/transactions
rsync -ah "$tmpdir/specifications-master/source/transactions/tests/" data/transactions
( cd data/transactions && find * -name \*.json | sort > test_files.txt  )

# Sync with_transactions tests
mkdir -p data/with_transaction
rsync -ah "$tmpdir/specifications-master/source/transactions-convenient-api/tests/" data/with_transaction
( cd data/with_transaction && find * -name \*.json | sort > test_files.txt  )

rm -rf "$tmpdir"

