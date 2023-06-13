#!/bin/bash
set -o errexit
set -o pipefail

if [ "$USE_STATIC_LIBS" ]; then
    DIR=static
else
    DIR=shared
fi

cd examples/projects

for project in bsoncxx mongocxx; do
(
    cd $project

    if ! ( cd cmake/$DIR && ./build.sh >|output.txt 2>&1); then
        cat cmake/$DIR/output.txt 1>&2
        exit 1
    fi

    if ! ( cd cmake-deprecated/$DIR && ./build.sh >|output.txt 2>&1); then
        cat cmake-deprecated/$DIR/output.txt 1>&2
        exit 1
    fi

    if [[ ! ( "$OSTYPE" =~ cygwin ) ]]; then
        if ! ( cd pkg-config/$DIR && ./build.sh >|output.txt 2>&1); then
            cat pkg-config/$DIR/output.txt 1>&2
            exit 1
        fi
    fi
)
done
