#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${build_variant:?}"
: "${mongodb_version:?}"

: "${AUTH:-}"
: "${ORCHESTRATION_FILE:-}"
: "${REQUIRE_API_VERSION:-}"
: "${TOPOLOGY:-}"

if [ ! -d "drivers-evergreen-tools" ]; then
  git clone https://github.com/mongodb-labs/drivers-evergreen-tools.git
fi
cd drivers-evergreen-tools

DRIVERS_TOOLS="$(pwd)"
if [ "Windows_NT" == "$OS" ]; then
  DRIVERS_TOOLS="$(cygpath -m "${DRIVERS_TOOLS:?}")"
fi
export DRIVERS_TOOLS

export MONGODB_BINARIES="${DRIVERS_TOOLS:?}/mongodb/bin"
export MONGO_ORCHESTRATION_HOME="${DRIVERS_TOOLS:?}/.evergreen/orchestration"
export MONGODB_VERSION="${mongodb_version:?}"
export AUTH
export TOPOLOGY
export REQUIRE_API_VERSION
export ORCHESTRATION_FILE

export PATH="${MONGODB_BINARIES:?}:${PATH:-}"

echo "{ \"releases\": { \"default\": \"${MONGODB_BINARIES:?}\" }}" >"${MONGO_ORCHESTRATION_HOME:?}/orchestration.config"
./.evergreen/run-orchestration.sh

declare mongosh_binary
if command -v mongosh >/dev/null; then
  mongosh_binary="mongosh"
elif command -v mongo >/dev/null; then
  mongosh_binary="mongo"
else
  echo "could not find a MongoDB Shell binary to use" 1>&2
  exit 1
fi
: "${mongosh_binary:?}"

# Ensure server on port 27017 is the primary server.
if [[ "${TOPOLOGY:-}" == replica_set ]]; then
  # Replset members should be on the following ports.
  declare hosts="localhost:27017,localhost:27018,localhost:27019"

  # Authentication may be required.
  declare creds
  if [[ -n "${AUTH:-}" ]]; then
    creds="bob:pwd123@"
  else
    creds=""
  fi

  declare uri
  printf -v uri "mongodb://%s%s" "${creds:-}" "${hosts:?}"

  # Raise the priority of the member on port 27017. Others should have priority 1.
  declare script
  printf -v script "%s\n%s\n%s\n" \
    "let c = rs.conf()" \
    "c.members.find((m) => m.host.includes('27017')).priority = 10" \
    "rs.reconfig(c)"
  "${mongosh_binary:?}" --quiet "${uri:?}" --eval "${script:?}"

  # Wait up to a minute for member on port 27017 to become primary.
  wait_for_primary() {
    for _ in $(seq 60); do
      if "${mongosh_binary:?}" --quiet "${uri:?}" --eval "quit(rs.hello().primary.includes('27017') ? 0 : 1)"; then
        return 0
      else
        sleep 1
      fi
    done
    echo "Could not set replset member on port 27017 as primary"
    return 1
  }

  echo "Waiting for replset member 27017 to become primary..."
  wait_for_primary
  echo "Waiting for replset member 27017 to become primary... done."
fi

# Copy mongocryptd up so other functions can find it later, since we can't share PATHs
if [ -f "${MONGODB_BINARIES:?}/mongocryptd" ]; then
  cp "${MONGODB_BINARIES:?}/mongocryptd" ../mongocryptd
fi

cd ../
pwd
