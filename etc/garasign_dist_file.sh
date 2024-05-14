#!/usr/bin/env bash

# Used by make_release.py.
# See: https://docs.devprod.prod.corp.mongodb.com/release-tools-container-images/garasign/garasign_signing/

set -o errexit
set -o pipefail

: "${1:?"missing dist_file as first argument"}"

if ! command -v podman >/dev/null; then
  echo "podman is required to sign distribution tarball" 1>&2
fi

if ! command -v gpg >/dev/null; then
  echo "gpg is required to verify distribution tarball signature" 1>&2
fi

creds=~/.secrets/garasign-creds.txt

if [[ ! -f "${creds:?}" ]]; then
  echo "missing file ${creds:?}" 1>&2
  exit 1
fi

# Avoid conflict/use of creds defined in the environment.
unset ARTIFACTORY_USER
unset ARTIFACTORY_PASSWORD
unset GRS_CONFIG_USER1_USERNAME
unset GRS_CONFIG_USER1_PASSWORD

. "${creds:?}"

: "${ARTIFACTORY_USER:?"missing ARTIFACTORY_USER in ${creds:?}"}"
: "${ARTIFACTORY_PASSWORD:?"missing ARTIFACTORY_PASSWORD in ${creds:?}"}"
: "${GRS_CONFIG_USER1_USERNAME:?"missing GRS_CONFIG_USER1_USERNAME in ${creds:?}"}"
: "${GRS_CONFIG_USER1_PASSWORD:?"missing GRS_CONFIG_USER1_PASSWORD in ${creds:?}"}"

dist_file="${1:?}"
dist_file_signed="${dist_file:?}.asc"

podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<< "${ARTIFACTORY_PASSWORD:?}"

plugin_commands=(
  gpg --yes -v --armor -o "${dist_file_signed:?}" --detach-sign "${dist_file:?}"
)
podman run \
  --env-file="${creds:?}" \
  -e "PLUGIN_COMMANDS=${plugin_commands[*]:?}" \
  --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)" \
  artifactory.corp.mongodb.com/release-tools-container-registry-local/garasign-gpg

# Validate the signature file works as intended.
keyring="$(mktemp)"
curl -sS https://pgp.mongodb.com/cpp-driver.pub | gpg -q --no-default-keyring --keyring "${keyring:?}" --import -
gpgv --keyring "${keyring:?}" "${dist_file_signed:?}" "${dist_file:?}"
