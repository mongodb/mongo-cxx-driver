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

command -v jq >/dev/null || {
  echo "missing required program jq" 1>&2
  exit 1
}

podman login --password-stdin --username "${ARTIFACTORY_USER:?}" artifactory.corp.mongodb.com <<<"${ARTIFACTORY_PASSWORD:?}"

silkbomb="artifactory.corp.mongodb.com/release-tools-container-registry-public-local/silkbomb:2.0"

# Ensure latest version of SilkBomb is being used.
podman pull "${silkbomb:?}"

silkbomb_augment_flags=(
  --repo mongodb/mongo-cxx-driver
  --branch "${branch_name:?}"
  --sbom-in /pwd/etc/cyclonedx.sbom.json
  --sbom-out /pwd/etc/augmented.sbom.json.new

  # Any notable updates to the Augmented SBOM version should be done manually after careful inspection.
  # Otherwise, it should be equal to the SBOM Lite version, which should normally be `1`.
  --no-update-sbom-version
)

# Allow the timestamp to be updated in the Augmented SBOM for update purposes.
podman run -it --rm -v "$(pwd):/pwd" --env 'KONDUKTO_TOKEN' "${silkbomb:?}" augment "${silkbomb_augment_flags[@]:?}"

[[ -f ./etc/augmented.sbom.json.new ]] || {
  echo "failed to download Augmented SBOM" 1>&2
  exit 1
}

echo "Comparing Augmented SBOM..."

# Format for easier diff while ignoring the timestamp field.
jq -S 'del(.metadata.timestamp)' ./etc/augmented.sbom.json >|old.json
jq -S 'del(.metadata.timestamp)' ./etc/augmented.sbom.json.new >|new.json

# Allow the task to upload the Augmented SBOM even if the diff failed.
if ! diff -sty --left-column -W 200 old.json new.json >|diff.txt; then
  declare status
  status='{"status":"failed", "type":"test", "should_continue":true, "desc":"detected significant changes in Augmented SBOM"}'
  curl -sS -d "${status:?}" -H "Content-Type: application/json" -X POST localhost:2285/task_status || true
fi

cat diff.txt

echo "Comparing Augmented SBOM... done."
