#!/usr/bin/env bash

# Used by make_release.py.
# See: https://docs.devprod.prod.corp.mongodb.com/release-tools-container-images/garasign/garasign_signing/

set -o errexit
set -o pipefail

: "${1:?"missing dist_file as first argument"}"

# Allow customization point to use docker in place of podman.
launcher="${GARASIGN_LAUNCHER:-"podman"}"

if ! command -v "${launcher:?}" >/dev/null; then
  echo "${launcher:?} is required to sign distribution tarball" 1>&2
fi

if ! command -v gpg >/dev/null; then
  echo "gpg is required to verify distribution tarball signature" 1>&2
fi

garasign_creds=~/.secrets/garasign-creds.txt

unset GRS_CONFIG_USER1_USERNAME GRS_CONFIG_USER1_PASSWORD
# shellcheck source=/dev/null
. "${garasign_creds:?}"
: "${GRS_CONFIG_USER1_USERNAME:?"missing GRS_CONFIG_USER1_USERNAME in ${garasign_creds:?}"}"
: "${GRS_CONFIG_USER1_PASSWORD:?"missing GRS_CONFIG_USER1_PASSWORD in ${garasign_creds:?}"}"

dist_file="${1:?}"
dist_file_signed="${dist_file:?}.asc"

# Ensure latest version of Garasign is being used.
"${launcher:?}" pull 901841024863.dkr.ecr.us-east-1.amazonaws.com/release-infrastructure/garasign-gpg

plugin_commands=(
  gpg --yes -v --armor -o "${dist_file_signed:?}" --detach-sign "${dist_file:?}"
)
"${launcher:?}" run \
  --env-file="${garasign_creds:?}" \
  -e "PLUGIN_COMMANDS=${plugin_commands[*]:?}" \
  --rm \
  -v "$(pwd):$(pwd)" \
  -w "$(pwd)" \
  901841024863.dkr.ecr.us-east-1.amazonaws.com/release-infrastructure/garasign-gpg

# Validate the signature file works as intended.
(
  GNUPGHOME="$(mktemp -d)"
  export GNUPGHOME
  curl -sS https://pgp.mongodb.com/cpp-driver.pub | gpg -q --no-default-keyring --import -
  gpg -q --import-ownertrust <<<"DC7F679B8A34DD606C1E54CAC4FC994D21532195:6:" # Set trust to "ultimate".
  gpg -q --update-trustdb
  gpg -q --verify "${dist_file_signed:?}" "${dist_file:?}"
)
