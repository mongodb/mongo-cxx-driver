#!/usr/bin/env bash

set -o errexit
set -o pipefail

declare working_dir
working_dir="$(pwd)"

declare base current
base="$(cat base-commit.txt)"
current="$(cat current-commit.txt)"

export PATH
PATH="${working_dir:?}/install/bin:${PATH:-}"

# Remove 'r' prefix in version string.
declare old_ver new_ver
old_ver="${base:1}-base"
new_ver="${current:1}-current"

command -V abi-compliance-checker >/dev/null
command -V abi-dumper >/dev/null
command -V parallel >/dev/null

mkdir cxx-abi cxx-noabi

declare bsoncxx_old bsoncxx_new mongocxx_old mongocxx_new
bsoncxx_old="${working_dir:?}/install/old/lib/libbsoncxx1.so"
bsoncxx_new="${working_dir:?}/install/new/lib/libbsoncxx1.so"
mongocxx_old="${working_dir:?}/install/old/lib/libmongocxx1.so"
mongocxx_new="${working_dir:?}/install/new/lib/libmongocxx1.so"

# Forward compatibility with upcoming stable ABI library filenames.
[[ -f "${bsoncxx_old:?}" ]] || bsoncxx_old="${working_dir:?}/install/old/lib/libbsoncxx.so"
[[ -f "${bsoncxx_new:?}" ]] || bsoncxx_new="${working_dir:?}/install/new/lib/libbsoncxx.so"
[[ -f "${mongocxx_old:?}" ]] || mongocxx_old="${working_dir:?}/install/old/lib/libmongocxx.so"
[[ -f "${mongocxx_new:?}" ]] || mongocxx_new="${working_dir:?}/install/new/lib/libmongocxx.so"

# Usage: generate_abi_report <label> <dir> <abi> <can_fail_task>
#   label: "stable" or "unstable".
#   dir: "cxx-abi" or "cxx-noabi".
#   abi: "v1" or "v_noabi".
#   can_fail_task: "true" or "false".
generate_abi_report() (
  declare label="${1:?}" dir="${2:?}" abi="${3:?}" can_fail_task="${4:?}"

  cd "${dir:?}"
  echo "Generating ${label:?} ABI report..."

  # Use "new" usage pattern based on binary debug-info analysis.
  # -keep-registers-and-offsets: abi-dumper doesn't detect `-Og` with Clang.
  # "WARNING: a "Struct" type with no attributes detected in the DWARF dump" is caused by missing mongoc debug-info.
  # "ERROR: missed type id <N>" is false-positive caused by otherwise-unused forward-decls in a template parameter.
  parallel --halt now,fail=1 --keep-order --tagstring '[{#}]' ::: \
    "abi-dumper '${bsoncxx_old:?}' \
      -o bsoncxx-old.dump \
      -lver '${old_ver:?}' \
      -keep-registers-and-offsets \
      -public-headers '${working_dir:?}/install/old/include/bsoncxx/${abi:?}' \
      -include-paths '${working_dir:?}/install/old/include/bsoncxx/v_noabi/bsoncxx' \
      -include-paths '${working_dir:?}/install/old/include/bsoncxx'" \
    "abi-dumper '${bsoncxx_new:?}' \
      -o bsoncxx-new.dump \
      -lver '${new_ver:?}' \
      -keep-registers-and-offsets \
      -public-headers '${working_dir:?}/install/new/include/bsoncxx/${abi:?}' \
      -include-paths '${working_dir:?}/install/new/include/bsoncxx/v_noabi/bsoncxx' \
      -include-paths '${working_dir:?}/install/new/include/bsoncxx'" \
    "abi-dumper '${mongocxx_old:?}' \
      -o mongocxx-old.dump \
      -lver '${old_ver:?}' \
      -keep-registers-and-offsets \
      -public-headers '${working_dir:?}/install/old/include/mongocxx/${abi:?}' \
      -include-paths '${working_dir:?}/install/old/include/mongocxx/v_noabi/mongocxx' \
      -include-paths '${working_dir:?}/install/old/include/mongocxx'" \
    "abi-dumper '${mongocxx_new:?}' \
      -o mongocxx-new.dump \
      -lver '${new_ver:?}' \
      -keep-registers-and-offsets \
      -public-headers '${working_dir:?}/install/new/include/mongocxx/${abi:?}' \
      -include-paths '${working_dir:?}/install/new/include/mongocxx/v_noabi/mongocxx' \
      -include-paths '${working_dir:?}/install/new/include/mongocxx'" \
    &>abi-dumper.log || {
    cat abi-dumper.log 1>&2
    exit 1
  }

  # Exclude internal detail namespaces, matching <skip_namespaces> in the legacy XML descriptors.
  declare -a skip_args=(-skip-internal-types "::detail::")

  if [[ "${abi:?}" == "v1" ]]; then
    skip_args+=(
      -skip-internal-symbols "\dv_noabi\d"
      -skip-internal-types "::v_noabi::"
    )
  elif [[ "${abi:?}" == "v_noabi" ]]; then
    skip_args+=(
      -skip-internal-symbols "\dv1\d"
      -skip-internal-types "::v1::"
    )
  fi

  declare bsoncxx_ret mongocxx_ret bsoncxx_pid mongocxx_pid
  abi-compliance-checker \
    -report-path bsoncxx.html \
    -lib bsoncxx \
    -old bsoncxx-old.dump \
    -new bsoncxx-new.dump \
    "${skip_args[@]}" &
  bsoncxx_pid=$!

  abi-compliance-checker \
    -report-path mongocxx.html \
    -lib mongocxx \
    -old mongocxx-old.dump \
    -new mongocxx-new.dump \
    "${skip_args[@]}" &
  mongocxx_pid=$!
  wait "${bsoncxx_pid:?}" && bsoncxx_ret=$? || bsoncxx_ret=$?
  wait "${mongocxx_pid:?}" && mongocxx_ret=$? || mongocxx_ret=$?

  # Allow task to upload logs and HTML reports despite failed status by using the EVG task HTTP endpoint.
  if [[ "${bsoncxx_ret:?}" -gt 1 || "${mongocxx_ret:?}" -gt 1 ]]; then
    declare status='{"status":"failed","type":"test","should_continue":true,"desc":"abi-compliance-checker emitted one or more errors"}'
    curl -sS -d "${status:?}" -H "Content-Type: application/json" -X POST localhost:2285/task_status || true
  elif [[ "${can_fail_task:?}" == "true" ]] && [[ "${bsoncxx_ret:?}" -gt 0 || "${mongocxx_ret:?}" -gt 0 ]]; then
    declare status='{"status":"failed","type":"test","should_continue":true,"desc":"detected stable ABI incompatibility"}'
    curl -sS -d "${status:?}" -H "Content-Type: application/json" -X POST localhost:2285/task_status || true
  fi

  echo "Generating ${label:?} ABI report... done."
)

export working_dir old_ver new_ver bsoncxx_old bsoncxx_new mongocxx_old mongocxx_new
export -f generate_abi_report
parallel --link --tagstring '[{#}]' \
  generate_abi_report {1} {2} {3} {4} \
  ::: stable unstable \
  ::: cxx-abi cxx-noabi \
  ::: v1 v_noabi \
  ::: true false
