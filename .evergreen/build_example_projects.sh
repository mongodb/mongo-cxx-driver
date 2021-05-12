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
    # If building against old libmongoc/libbson, then we can't
    # use new cmake target names, so only test the old deprecated
    # target names.
    if [ "$MONGOC_VERSION" = "master" ]; then
        ( cd cmake/$DIR && ./build.sh )
    fi
    ( cd cmake-deprecated/$DIR && ./build.sh )
    if [ "Windows_NT" != "$OS" ]; then
        ( cd pkg-config/$DIR && ./build.sh )
    fi
)
done
