#!/usr/bin/env bash

# Test runner for AWS authentication.
#
# This script is meant to be run in parts (so to isolate the AWS tests).
# Pass the desired test as the environment variable TESTCASE: (REGULAR, EC2, ECS, ASSUME_ROLE, LAMBDA)
#
# Example:
# TESTCASE=EC2 run-aws-tests.sh
#
# Optional environment variables:
# iam_auth_ecs_account and iam_auth_ecs_secret_access_key
#   Set to access key id/secret access key. Required for some tests.

set -o errexit
set -o pipefail

# Do not trace
set +o xtrace

# shellcheck source=.evergreen/scripts/env-var-utils.sh
. "$(dirname "${BASH_SOURCE[0]}")/env-var-utils.sh"
. "$(dirname "${BASH_SOURCE[0]}")/use-tools.sh" paths

check_var_req TESTCASE

declare script_dir
script_dir="$(to_absolute "$(dirname "${BASH_SOURCE[0]}")")"

declare mongoc_dir
mongoc_dir="$(to_absolute "${script_dir}/../..")"

declare drivers_tools_dir
drivers_tools_dir="$(to_absolute "${mongoc_dir}/../drivers-evergreen-tools")"

declare mongodb_bin_dir="${mongoc_dir}/mongodb/bin"
declare test_awsauth="${mongoc_dir}/src/libmongoc/test-awsauth"

if [[ "${OSTYPE}" == "cygwin" ]]; then
  test_awsauth="${mongoc_dir}/src/libmongoc/Debug/test-awsauth.exe"
fi

expect_success() {
  echo "Should succeed:"
  "${test_awsauth}" "${1:?}" "EXPECT_SUCCESS" || exit
}

expect_failure() {
  echo "Should fail:"
  "${test_awsauth}" "${1:?}" "EXPECT_FAILURE" || exit
}

url_encode() {
  declare encoded=""
  for c in $(echo ${1:?} | grep -o .); do
    case "${c}" in
    [a-zA-Z0-9.~_-])
      encoded="${encoded}${c}"
      ;;
    *)
      encoded="${encoded}$(printf '%%%02X' "'${c}")"
      ;;
    esac
  done
  echo "${encoded}"
}

# Some of the setup scripts expect mongo to be on path.
export PATH
PATH+=":${mongodb_bin_dir}"

if [[ "${TESTCASE}" == "REGULAR" ]]; then
  echo "===== Testing regular auth via URI ====="

  # Create user on $external db.
  pushd "${drivers_tools_dir}/.evergreen/auth_aws"
  mongo --verbose aws_e2e_regular_aws.js
  popd # "${drivers_tools_dir}/.evergreen/auth_aws"

  declare user_encoded pass_encoded
  user_encoded="$(url_encode "${iam_auth_ecs_account:?}")"
  pass_encoded="$(url_encode "${iam_auth_ecs_secret_access_key:?}")"

  expect_success "mongodb://${user_encoded:?}:${pass_encoded:?}@localhost/?authMechanism=MONGODB-AWS"
  expect_failure "mongodb://${user_encoded:?}:bad_password@localhost/?authMechanism=MONGODB-AWS"

  exit
fi

if [[ "${TESTCASE}" == "ASSUME_ROLE" ]]; then
  echo "===== Testing auth with session token via URI with AssumeRole ====="
  pushd "${drivers_tools_dir}/.evergreen/auth_aws"
  mongo --verbose aws_e2e_assume_role.js
  popd # "${drivers_tools_dir}/.evergreen/auth_aws"

  declare user pass token
  user="$(jq -r '.AccessKeyId' "${drivers_tools_dir}/.evergreen/auth_aws/creds.json")"
  pass="$(jq -r '.SecretAccessKey' "${drivers_tools_dir}/.evergreen/auth_aws/creds.json")"
  token="$(jq -r '.SessionToken' "${drivers_tools_dir}/.evergreen/auth_aws/creds.json")"

  declare user_encoded pass_encoded token_encoded
  user_encoded="$(url_encode "${user:?}")"
  pass_encoded="$(url_encode "${pass:?}")"
  token_encoded="$(url_encode "${token:?}")"

  expect_success "mongodb://${user_encoded}:${pass_encoded}@localhost/aws?authMechanism=MONGODB-AWS&authSource=\$external&authMechanismProperties=AWS_SESSION_TOKEN:${token_encoded}"
  expect_failure "mongodb://${user_encoded}:${pass_encoded}@localhost/aws?authMechanism=MONGODB-AWS&authSource=\$external&authMechanismProperties=AWS_SESSION_TOKEN:bad_token"
  exit
fi

if [[ "LAMBDA" = "$TESTCASE" ]]; then
  echo "===== Testing auth via environment variables ====="

  pushd "${drivers_tools_dir}/.evergreen/auth_aws"
  mongo --verbose aws_e2e_assume_role.js
  popd # "${drivers_tools_dir}/.evergreen/auth_aws"

  declare user pass token
  user="$(jq -r '.AccessKeyId' "${drivers_tools_dir}/.evergreen/auth_aws/creds.json")"
  pass="$(jq -r '.SecretAccessKey' "${drivers_tools_dir}/.evergreen/auth_aws/creds.json")"
  token="$(jq -r '.SessionToken' "${drivers_tools_dir}/.evergreen/auth_aws/creds.json")"

  echo "Valid credentials - should succeed:"
  export AWS_ACCESS_KEY_ID="${user:?}"
  export AWS_SECRET_ACCESS_KEY="${pass:?}"
  export AWS_SESSION_TOKEN="${token:?}"
  expect_success "mongodb://localhost/?authMechanism=MONGODB-AWS"
  exit
fi

if [[ "${TESTCASE}" == "EC2" ]]; then
  echo "===== Testing auth via EC2 task metadata ====="
  # Do necessary setup for EC2
  # Create user on $external db.
  pushd "${drivers_tools_dir}/.evergreen/auth_aws"
  mongo --verbose aws_e2e_ec2.js
  popd # "${drivers_tools_dir}/.evergreen/auth_aws"

  echo "Valid credentials via EC2 - should succeed"
  expect_success "mongodb://localhost/?authMechanism=MONGODB-AWS"
  exit
fi

if [[ "${TESTCASE}" == "ECS" ]]; then
  echo "===== Testing auth via ECS task metadata ====="
  [[ -d "${drivers_tools_dir}" ]]
  # Overwrite the test that gets run by remote ECS task.
  cp "${mongoc_dir}/.evergreen/etc/ecs_hosted_test.js" "${drivers_tools_dir}/.evergreen/auth_aws/lib"
  chmod 777 "${script_dir}/run-mongodb-aws-ecs-test.sh"

  pushd "${drivers_tools_dir}/.evergreen/auth_aws"

  cat <<EOF >setup.js
    const mongo_binaries = "${mongodb_bin_dir}";
    const project_dir = "${mongoc_dir}";
EOF

  "${mongodb_bin_dir}/mongo" --nodb setup.js aws_e2e_ecs.js
  exit
fi

if [[ "${TESTCASE}" == "ASSUME_ROLE_WITH_WEB_IDENTITY" ]]; then
  echo "===== Testing auth via Web Identity ====="
  # Do necessary setup.
  # Create user on $external db.
  pushd "${drivers_tools_dir}/.evergreen/auth_aws"
  mongo --verbose aws_e2e_web_identity.js
  popd # "${drivers_tools_dir}/.evergreen/auth_aws"

  declare iam_auth_assume_web_role_name iam_web_identity_token_file
  iam_auth_assume_web_role_name="$(jq -r '.iam_auth_assume_web_role_name' "${drivers_tools_dir}/.evergreen/auth_aws/aws_e2e_setup.json")"
  iam_web_identity_token_file="$(jq -r '.iam_web_identity_token_file' "${drivers_tools_dir}/.evergreen/auth_aws/aws_e2e_setup.json")"

  echo "Valid credentials via Web Identity - should succeed"
  AWS_ROLE_ARN="${iam_auth_assume_web_role_name}" \
  AWS_WEB_IDENTITY_TOKEN_FILE="${iam_web_identity_token_file}" \
    expect_success "mongodb://localhost/?authMechanism=MONGODB-AWS"

  echo "Valid credentials via Web Identity with session name - should succeed"
  AWS_ROLE_ARN="${iam_auth_assume_web_role_name}" \
  AWS_WEB_IDENTITY_TOKEN_FILE="${iam_web_identity_token_file}" \
  AWS_ROLE_SESSION_NAME=test \
    expect_success "mongodb://localhost/?authMechanism=MONGODB-AWS"

  echo "Invalid AWS_ROLE_ARN via Web Identity with session name - should fail"
  AWS_ROLE_ARN="invalid_role_arn" \
  AWS_WEB_IDENTITY_TOKEN_FILE="${iam_web_identity_token_file}" \
    expect_failure "mongodb://localhost/?authMechanism=MONGODB-AWS"

  echo "Invalid AWS_WEB_IDENTITY_TOKEN_FILE via Web Identity with session name - should fail"
  AWS_ROLE_ARN="${iam_auth_assume_web_role_name}" \
  AWS_WEB_IDENTITY_TOKEN_FILE="/invalid/path" \
    expect_failure "mongodb://localhost/?authMechanism=MONGODB-AWS"

  echo "Invalid AWS_ROLE_SESSION_NAME via Web Identity with session name - should fail"
  AWS_ROLE_ARN="${iam_auth_assume_web_role_name}" \
  AWS_WEB_IDENTITY_TOKEN_FILE="${iam_web_identity_token_file}" \
  AWS_ROLE_SESSION_NAME="contains_invalid_character_^" \
    expect_failure "mongodb://localhost/?authMechanism=MONGODB-AWS"
  exit
fi

echo "Unexpected testcase '${TESTCASE}'" 1>&2
exit 1
