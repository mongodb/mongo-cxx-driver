#!/bin/sh

#
# build_snapshot.sh
#

#
# Copyright 2018 MongoDB, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


for arg in "$@"; do
  if [ "$arg" = "-h" ]; then
    echo "Usage: ./debian/build_snapshot.sh"
    echo ""
    echo "  This script is used to build a .deb package directly from a snapshot of the"
    echo "  current repository."
    echo ""
    echo "  This script must be called from the base directory of the repository, and"
    echo "  requires utilites from these packages: dpkg-dev, git-buildpackage"
    echo ""
    exit
  fi
done

package="mongo-cxx-driver"

if [ ! -x /usr/bin/dpkg-parsechangelog ]; then
  echo "Missing the dpkg-parsechangelog utility from the dpkg-dev package"
  exit 1
fi

if [ ! -f debian/changelog ]; then
  echo "This script must be called from the base directory of the package"
  exit 1
fi

if [ ! -d .git ]; then
  echo "This script only works from within a repository"
  exit 1
fi

if [ ! -x /usr/bin/gbp ]; then
  echo "Missing git-buildpackage"
  exit 1
fi

changelog_package=$(dpkg-parsechangelog | sed -n 's/^Source: //p')
if [ "${package}" != "${changelog_package}" ]; then
  echo "This script is configured to create snapshots for ${package} but you are trying to create a snapshot for ${changelog_package}"
  exit 1
fi

bare_upstream_version=$(sed -E 's/([^-]+).*/\1/' build/VERSION_CURRENT)
echo "Found bare upstream version: ${bare_upstream_version}"
snapshot_version="${bare_upstream_version}-0+$(date +%Y%m%d)+git$(git rev-parse --short HEAD)"
echo "Upstream snapshot version: ${snapshot_version}"


if [ "$(dpkg-parsechangelog | sed -E -n 's/^Version: +(.*)/\1/p')" != "${snapshot_version}" ]; then
  echo "Making Debian changelog entry"
  dch --force-bad-version --release-heuristic log -v "${snapshot_version}" -D UNRELEASED "Built from Git snapshot."
fi

echo "Calling git-buildpackage ..."
if [ "${DEB_BUILD_PROFILES#*pkg.mongo-cxx-driver.mnmlstc}" != "${DEB_BUILD_PROFILES}" ]; then
   gbp buildpackage --git-no-pbuilder --git-export=WC --git-verbose --git-pbuilder-options="--use-network yes"
else
   gbp buildpackage --git-no-pbuilder --git-export=WC --git-verbose
fi

