// Copyright 2018-present MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/test/spec/util.hh>

namespace {

using namespace mongocxx;
using namespace spec;

TEST_CASE("Read / Write concern spec tests", "[read_write_concern_spec]") {
    instance::current();

    // Reuse the transactions test runner.
    auto tests_path = std::getenv("READ_WRITE_CONCERN_OPERATION_TESTS_PATH")
                          ? std::getenv("READ_WRITE_CONCERN_OPERATION_TESTS_PATH")
                          : std::string{MONGOCXX_SOURCE_DIR} + "/data/read-write-concern/operation";
    run_tests_in_suite(tests_path, &run_transactions_tests_in_file);
}
}  // namespace
