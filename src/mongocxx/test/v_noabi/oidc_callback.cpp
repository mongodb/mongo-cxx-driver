// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/v1/oidc_callback.hpp>
#include <mongocxx/v1/oidc_callback_params.hpp>
#include <mongocxx/v1/oidc_credential.hpp>

#include <mongocxx/test/private/scoped_bson.hh>
#include <mongocxx/test/v_noabi/catch_helpers.hh>
#include <mongocxx/test/v_noabi/client_helpers.hh>

#include <fstream>
#include <string>

#include <bsoncxx/v_noabi/bsoncxx/document/view.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/v_noabi/mongocxx/database.hpp>
#include <mongocxx/v_noabi/mongocxx/exception/exception.hpp>
#include <mongocxx/v_noabi/mongocxx/pool.hpp>
#include <mongocxx/v_noabi/mongocxx/uri.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v_noabi {

class OIDCTestFixture {
   public:
    OIDCTestFixture(v_noabi::uri uri, mongocxx::v_noabi::options::client opts, bool is_pooled) : _is_pooled(is_pooled) {
        if (is_pooled) {
            _pool.emplace(uri, opts);
            _pool_entry = _pool->acquire();
        } else {
            _client.emplace(uri, opts);
        }
    }
    mongocxx::v_noabi::client& client() {
        if (_is_pooled) {
            return *(_pool_entry.value());
        } else {
            return *_client;
        }
    }

   private:
    bool _is_pooled;
    bsoncxx::v_noabi::stdx::optional<v_noabi::pool> _pool;
    bsoncxx::v_noabi::stdx::optional<v_noabi::pool::entry> _pool_entry;
    bsoncxx::v_noabi::stdx::optional<v_noabi::client> _client;
};

TEST_CASE("OIDC (v_noabi)", "[oidc]") {
    if (nullptr == std::getenv("OIDC_TOKEN_FILE")) {
        SKIP("Set OIDC_TOKEN_FILE to run OIDC tests");
    }

    SECTION("Works") {
        auto callback_call_count = 0u;
        v1::oidc_callback cb = [&](v1::oidc_callback_params const&) {
            callback_call_count++;
            std::ifstream token_file(test_util::getenv_or_fail("OIDC_TOKEN_FILE"));
            REQUIRE(token_file.is_open());
            auto token_str =
                std::string((std::istreambuf_iterator<char>(token_file)), std::istreambuf_iterator<char>());
            return v1::oidc_credential(token_str);
        };

        auto opts = v_noabi::options::client();
        opts.oidc_callback(cb);

        // Create an OIDC configured client:
        bool is_pooled = GENERATE(true, false);
        OIDCTestFixture tf(
            v_noabi::uri("mongodb://localhost:27017/?retryReads=false&authMechanism=MONGODB-OIDC"), opts, is_pooled);

        /// Expect auth to succeed:
        tf.client().database("test").run_command(bsoncxx::v_noabi::from_v1(scoped_bson{R"({"ping": 1})"}.view()));

        // Expect callback was called:
        CHECK(callback_call_count == 1u);
    }

    SECTION("Throws exception in v_noabi namespace") {
        v1::oidc_callback cb = [&](v1::oidc_callback_params const&) { return v1::oidc_credential(std::string("")); };

        auto opts = v_noabi::options::client();
        opts.oidc_callback(cb);

        // Create an OIDC configured client with a bad callback:
        bool is_pooled = GENERATE(true, false);
        OIDCTestFixture tf(
            v_noabi::uri("mongodb://localhost:27017/?retryReads=false&authMechanism=MONGODB-OIDC"), opts, is_pooled);

        CHECK_THROWS_MATCHES(
            tf.client().database("test").run_command(bsoncxx::v_noabi::from_v1(scoped_bson{R"({"ping": 1})"}.view())),
            mongocxx::exception,
            mongocxx::test_util::mongocxx_exception_matcher{"Authentication failed"});
    }
}

} // namespace v_noabi
} // namespace mongocxx
