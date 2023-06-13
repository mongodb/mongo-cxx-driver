#!/bin/bash
set -o errexit
set -o pipefail

if [ "$USE_STATIC_LIBS" ]; then
    DIR=static
else
    DIR=shared
fi

. .evergreen/find_cmake.sh
export CMAKE="$CMAKE"

cd examples/projects

for project in bsoncxx mongocxx; do
(
    cd $project

    if ! ( cd cmake/$DIR && ./build.sh >|output.txt 2>&1); then
        cat output.txt 1>&2
        exit 1
    fi

    if ! ( cd cmake-deprecated/$DIR && ./build.sh >|output.txt 2>&1); then
        cat output.txt 1>&2
        exit 1
    fi

    if [ "Windows_NT" != "$OS" ]; then
        if ! ( cd pkg-config/$DIR && ./build.sh >|output.txt 2>&1); then
            cat output.txt 1>&2
            exit 1
        fi
    fi
)
done
