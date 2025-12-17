#!/usr/bin/env bash

set -o errexit
set -o pipefail

declare link_type
if [[ "${USE_STATIC_LIBS:-}" == 1 ]]; then
  link_type=static
else
  link_type=shared
fi
: "${link_type:?}"

cd examples/projects

for project in bsoncxx mongocxx; do
  (
    cd "${project:?}"

    if ! (cd "cmake/${link_type:?}" && ./build.sh >|output.txt 2>&1); then
      echo "Example ${project:?}/cmake/${link_type:?} failed" 1>&2
      cat "cmake/${link_type:?}/output.txt" 1>&2
      exit 1
    fi

    # pkg-config is only applicable to non-Visual Studio generators.
    if [[ "${generator:-}" != Visual\ Studio\ * ]]; then
      if ! (cd "pkg-config/${link_type:?}" && ./build.sh >|output.txt 2>&1); then
        echo "Example ${project:?}/pkg-config/${link_type:?} failed" 1>&2
        cat "pkg-config/${link_type:?}/output.txt" 1>&2
        exit 1
      fi
    fi
  )
done
