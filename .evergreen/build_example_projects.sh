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
    ( cd cmake/$DIR && ./build.sh )
    ( cd cmake-deprecated/$DIR && ./build.sh )
    if [ "Windows_NT" != "$OS" ]; then
        ( cd pkg-config/$DIR && ./build.sh )
    fi
)
done
