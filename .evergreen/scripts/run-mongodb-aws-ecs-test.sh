#!/usr/bin/env bash

# ECS tests have paths /root/mongoc/

echo "run-mongodb-aws-ecs-test.sh"


expect_success() {
  echo "Should succeed:"
  /root/mongoc/src/libmongoc/test-awsauth "${1:?}" "EXPECT_SUCCESS"
}

expect_success "${1:?}"
