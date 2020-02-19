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

#pragma once

#include <bsoncxx/document/view.hpp>
#include <mongocxx/client.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace bsoncxx;
using namespace mongocxx;
bool should_skip_spec_test(const client& client, document::view test);
void disable_fail_point(std::string uri_string, options::client client_opts);
void initialize_collection(collection* coll, array::view initial_data);
uri get_uri(document::view test);

//
// Uses the given environment variable to load all tests in a test suite
// and run them with the provided callback function.
//
using test_runner = std::function<void(const std::string& file)>;
void run_tests_in_suite(std::string ev, test_runner cb);

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
#include <mongocxx/config/private/postlude.hh>
