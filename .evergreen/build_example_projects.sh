#!/bin/bash
set -o errexit
set -o pipefail
set -o xtrace

if [ "$USE_STATIC_LIBS" ]; then
    DIR=static
else
    DIR=shared
fi

cd examples/projects

for project in bsoncxx mongocxx; do
(
    cd $project
    ( cd cmake/$DIR && ./build.sh )
    if [ "Windows_NT" != "$OS" ]; then
        ( cd pkg-config/$DIR && ./build.sh )
    fi
)
done
