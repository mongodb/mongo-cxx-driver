#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${ARTIFACTORY_USER:?}"
: "${ARTIFACTORY_PASSWORD:?}"
: "${branch_name:?}"
: "${KONDUKTO_TOKEN:?}"

command -v podman >/dev/null || {
  echo "missing required program podman" 1>&2
  exit 1
}

podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

silkbomb="artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0"

# Ensure latest version of SilkBomb is being used.
podman pull "${silkbomb:?}"

# First validate the SBOM Lite.
podman run -it --rm -v "$(pwd):/pwd" "${silkbomb:?}" \
  validate --purls /pwd/etc/purls.txt --sbom-in /pwd/etc/cyclonedx.sbom.json --exclude jira

# Then upload the SBOM Lite.
podman run -it --rm -v "$(pwd):/pwd" --env 'KONDUKTO_TOKEN' "${silkbomb:?}" \
  upload --repo mongodb/mongo-cxx-driver --branch "${branch_name:?}" --sbom-in /pwd/etc/cyclonedx.sbom.json
