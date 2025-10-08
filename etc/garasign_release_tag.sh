#!/usr/bin/env bash

# Used by make_release.py.
# See: https://docs.devprod.prod.corp.mongodb.com/release-tools-container-images/garasign/garasign_signing/

set -o errexit
set -o pipefail

: "${1:?"missing tag name as first argument"}"

release_tag="${1:?}"

# Allow customization point to use docker in place of podman.
launcher="${GARASIGN_LAUNCHER:-"podman"}"

if ! command -v "${launcher:?}" >/dev/null; then
  echo "${launcher:?} is required to create a GPG-signed release tag" 1>&2
fi

artifactory_creds=~/.secrets/artifactory-creds.txt
garasign_creds=~/.secrets/garasign-creds.txt

unset ARTIFACTORY_USER ARTIFACTORY_PASSWORD
# shellcheck source=/dev/null
. "${artifactory_creds:?}"
: "${ARTIFACTORY_USER:?"missing ARTIFACTORY_USER in ${artifactory_creds:?}"}"
: "${ARTIFACTORY_PASSWORD:?"missing ARTIFACTORY_PASSWORD in ${artifactory_creds:?}"}"

unset GRS_CONFIG_USER1_USERNAME GRS_CONFIG_USER1_PASSWORD
# shellcheck source=/dev/null
. "${garasign_creds:?}"
: "${GRS_CONFIG_USER1_USERNAME:?"missing GRS_CONFIG_USER1_USERNAME in ${garasign_creds:?}"}"
: "${GRS_CONFIG_USER1_PASSWORD:?"missing GRS_CONFIG_USER1_PASSWORD in ${garasign_creds:?}"}"

"${launcher:?}" login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

# Ensure latest version of Garasign is being used.
"${launcher:?}" pull artifactory.corp.mongodb.com/release-tools-container-registry-local/garasign-git

# Sign using "MongoDB C++ Release Signing Key <packaging@mongodb.com>" from https://pgp.mongodb.com/ (cpp-driver).
git_tag_command=(
  git
  -c "user.name=\"MongoDB C++ Release Signing Key\""
  -c "user.email=\"packaging@mongodb.com\""
  tag
  -u DC7F679B8A34DD606C1E54CAC4FC994D21532195
  -m "\"${release_tag:?}\""
  "\"${release_tag:?}\""
)
plugin_commands=""
plugin_commands+="gpg -q --update-trustdb"
plugin_commands+=" && gpg --list-key DC7F679B8A34DD606C1E54CAC4FC994D21532195"
plugin_commands+=" && ${git_tag_command[*]:?}"
"${launcher:?}" run \
  --env-file="${garasign_creds:?}" \
  -e "PLUGIN_COMMANDS=${plugin_commands:?}" \
  --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)" \
  artifactory.corp.mongodb.com/release-tools-container-registry-local/garasign-git

# Validate the release tag is signed as intended.
(
  GNUPGHOME="$(mktemp -d)"
  export GNUPGHOME
  curl -sS https://pgp.mongodb.com/cpp-driver.pub | gpg -q --no-default-keyring --import -
  gpg -q --import-ownertrust <<<"DC7F679B8A34DD606C1E54CAC4FC994D21532195:6:" # Set trust to "ultimate".
  gpg -q --update-trustdb
  git verify-tag "${release_tag:?}"
)
