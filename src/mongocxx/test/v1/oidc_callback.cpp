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

//

#include <mongocxx/v1/client.hpp>
#include <mongocxx/v1/collection.hpp>
#include <mongocxx/v1/database.hpp>
#include <mongocxx/v1/insert_one_result.hpp>
#include <mongocxx/v1/oidc_callback_params.hpp>
#include <mongocxx/v1/oidc_credential.hpp>
#include <mongocxx/v1/pool.hpp>
#include <mongocxx/v1/uri.hpp>

#include <bsoncxx/test/v1/stdx/optional.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <atomic>
#include <cstdint>
#include <fstream>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

class OIDCTestURI {
   public:
    OIDCTestURI& with_username(std::string username) {
        _username = std::move(username);
        return *this;
    }
    OIDCTestURI& with_extra_options(std::string extra_opts) {
        _extra_opts = std::move(extra_opts);
        return *this;
    }
    OIDCTestURI& with_appname(std::string appname) {
        _appname = std::move(appname);
        return *this;
    }
    v1::uri build() const {
        std::string uri_str = "mongodb://";
        if (_username) {
            uri_str += *_username + "@";
        }
        uri_str += "localhost:27017/?retryReads=false&authMechanism=MONGODB-OIDC";
        if (_extra_opts) {
            uri_str += "&" + *_extra_opts;
        }
        if (_appname) {
            uri_str += "&appName=" + *_appname;
        }
        return v1::uri(uri_str);
    }

   private:
    bsoncxx::v1::stdx::optional<std::string> _username;
    bsoncxx::v1::stdx::optional<std::string> _appname;
    bsoncxx::v1::stdx::optional<std::string> _extra_opts;
};

class OIDCTestFixture {
   public:
    OIDCTestFixture(v1::uri const& uri, v1::client::options opts, bool is_pooled) {
        if (is_pooled) {
            _pool.emplace(uri, std::move(opts));
            _pool_entry = _pool->acquire();
        } else {
            _client.emplace(uri, std::move(opts));
        }
    }
    v1::client& client() {
        if (_pool_entry) {
            return **_pool_entry;
        } else {
            return *_client;
        }
    }
    v1::pool& pool() {
        if (!_pool) {
            throw std::logic_error("Not a pooled client");
        }
        return *_pool;
    }

   private:
    bsoncxx::v1::stdx::optional<v1::pool> _pool;
    bsoncxx::v1::stdx::optional<v1::pool::entry> _pool_entry;
    bsoncxx::v1::stdx::optional<v1::client> _client;
};

namespace {
// Read token from OIDC_TOKEN_FILE
std::string read_token_from_file() {
    auto oidc_token_file = std::getenv("OIDC_TOKEN_FILE");
    REQUIRE(oidc_token_file);
    std::ifstream token_file(oidc_token_file);
    REQUIRE(token_file.is_open());
    return std::string((std::istreambuf_iterator<char>(token_file)), std::istreambuf_iterator<char>());
}

struct fail_command_guard {
    fail_command_guard(std::string const& cmd_name, std::int32_t err_code, std::string appname)
        : _appname(std::move(appname)) {
        // clang-format off
        scoped_bson data(BCON_NEW(
            "failCommands", "[", cmd_name.c_str(), "]",
            "errorCode", BCON_INT32(err_code),
            "appName", _appname.c_str()));
        // clang-format on

        auto cmd = scoped_bson(R"({
            "configureFailPoint": "failCommand",
            "mode": { "times": 1 }
        })");
        cmd += scoped_bson(BCON_NEW("data", BCON_DOCUMENT(data.bson())));
        admin_command(cmd.view());
    }

    ~fail_command_guard() {
        // Try to disable failpoint. Ignore error.
        auto cmd = scoped_bson(R"({
            "configureFailPoint": "failCommand",
            "mode": "off"
        })");
        cmd += scoped_bson(BCON_NEW("data", "{", "appName", _appname.c_str(), "}"));
        try {
            admin_command(cmd.view());
        } catch (...) {
        }
    }

    fail_command_guard(fail_command_guard const&) = delete;
    fail_command_guard& operator=(fail_command_guard const&) = delete;
    fail_command_guard(fail_command_guard&&) = delete;
    fail_command_guard& operator=(fail_command_guard&&) = delete;

   private:
    static void admin_command(bsoncxx::v1::document::view cmd) {
        // configureFailPoint does not require auth.
        v1::client(v1::uri()).database("admin").run_command(cmd);
    }
    std::string _appname;
};

} // namespace

TEST_CASE("OIDC prose tests", "[oidc]") {
    if (nullptr == std::getenv("OIDC_TOKEN_FILE")) {
        SKIP("Set OIDC_TOKEN_FILE to run OIDC tests");
    }

    SECTION("1.1 Callback is called during authentication") {
        auto callback_call_count = 0u;
        auto const token = read_token_from_file();
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;
            return oidc_credential(token);
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        OIDCTestFixture tf(OIDCTestURI().build(), opts, is_pooled);

        // Spec: "Perform a `find` operation that succeeds"
        CHECK_NOTHROW(tf.client().database("test").collection("test").find_one(scoped_bson{}.view()));

        // Spec: "Assert that the callback was called 1 time"
        CHECK(callback_call_count == 1u);
    }

    SECTION("1.2 Callback is called once for multiple connections") {
        std::atomic_uint callback_call_count{0};
        auto const token = read_token_from_file();
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count.fetch_add(1);

            return oidc_credential(token);
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        // Only test pool. Single-threaded client cannot be shared among threads.
        OIDCTestFixture tf(OIDCTestURI().build(), opts, true);

        // Spec: "Start 10 threads and run 100 `find` operations in each thread that all succeed."
        std::vector<std::thread> threads;
        std::vector<v1::pool::entry> pool_entries;

        auto const num_threads = 10u;
        // Acquire one client per thread:
        for (auto i = 0u; i < num_threads; i++) {
            pool_entries.push_back(tf.pool().acquire());
        }

        // Start threads:
        for (auto i = 0u; i < num_threads; i++) {
            auto& client = *pool_entries.at(i);
            threads.emplace_back([&client]() {
                auto const finds_per_thread = 100u;
                for (auto i = 0u; i < finds_per_thread; i++) {
                    // Omit CHECK_NOTHROW. Thread-safe assertions requires Catch2 opt-in.
                    client.database("test").collection("test").find_one(scoped_bson{}.view());
                }
            });
        }

        // Join threads:
        for (auto& thread : threads) {
            thread.join();
        }

        // Spec: "Assert that the callback was called 1 time"
        CHECK(callback_call_count == 1u);
    }

    SECTION("2.1 Valid Callback Inputs") {
        auto callback_call_count = 0u;
        bool with_username = GENERATE(/* true, */ false); // TODO CDRIVER-6310: test 'true' once upgraded to 2.3.1.
        CAPTURE(with_username);
        auto const token = read_token_from_file();

        oidc_callback cb = [&](oidc_callback_params const& params) {
            // Expect timeout is roughly 60 seconds:
            {
                auto const timeout = params.timeout();
                auto const now = std::chrono::steady_clock::now();
                REQUIRE(timeout);
                auto const expected_duration = std::chrono::seconds(60);
                auto const epsilon = std::chrono::seconds(10);
                auto const low = now + expected_duration - epsilon;
                auto const high = now + expected_duration + epsilon;
                CHECK(timeout.value() >= low);
                CHECK(timeout.value() <= high);
            }

            CHECK(params.version() == 1);

            if (with_username) {
                CHECK(params.username() == "user");
            } else {
                CHECK(!params.username().has_value());
            }
            callback_call_count++;

            return oidc_credential(token);
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);

        v1::uri uri = (with_username) ? OIDCTestURI().with_username("user").build() : OIDCTestURI().build();

        OIDCTestFixture tf(uri, opts, is_pooled);

        // Spec: "Perform a `find` operation that succeeds"
        CHECK_NOTHROW(tf.client().database("test").collection("test").find_one(scoped_bson{}.view()));

        // Spec: "Assert that the OIDC callback was called"
        CHECK(callback_call_count == 1u);
    }

    SECTION("2.2 OIDC Callback Returns Null") {
        auto callback_call_count = 0u;
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;

            // Cannot construct oidc_credential with null. Use empty string.
            return oidc_credential(std::string(""));
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client with an OIDC callback that returns `null`"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        OIDCTestFixture tf(OIDCTestURI().build(), opts, is_pooled);

        // Spec: "Perform a `find` operation that fails"
        CHECK_THROWS_WITH(
            tf.client().database("test").collection("test").find_one(scoped_bson{}.view()),
            Catch::Matchers::ContainsSubstring("Authentication failed"));
        CHECK(callback_call_count == 1u);
    }

    SECTION("2.3 OIDC Callback Returns Missing Data") {
        auto callback_call_count = 0u;
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;

            return oidc_credential(std::string("bad_token"));
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client with an OIDC callback that returns data not conforming to the
        // `OIDCCredential` with missing fields."
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        OIDCTestFixture tf(OIDCTestURI().build(), opts, is_pooled);

        // Spec: "Perform a `find` operation that fails"
        CHECK_THROWS_WITH(
            tf.client().database("test").collection("test").find_one(scoped_bson{}.view()),
            Catch::Matchers::ContainsSubstring("Authentication failed"));
        CHECK(callback_call_count == 1u);
    }

    SECTION("2.4 Invalid Client Configuration with Callback") {
        oidc_callback cb = [&](oidc_callback_params const&) { return oidc_credential(std::string("")); };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);

        OIDCTestFixture tf(
            OIDCTestURI().with_extra_options("authMechanismProperties=ENVIRONMENT:test").build(), opts, is_pooled);

        // Spec: "Assert it returns a client configuration error upon client creation, or client connect if your driver
        // validates on connection". C driver errors on connection.
        CHECK_THROWS_WITH(
            tf.client().database("test").collection("test").find_one(scoped_bson{}.view()),
            Catch::Matchers::ContainsSubstring("requested with both ENVIRONMENT and an OIDC Callback"));
    }

    SECTION("2.5 Invalid use of ALLOWED_HOSTS") {
        oidc_callback cb = [&](oidc_callback_params const&) { return oidc_credential(std::string("")); };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);

        CHECK_THROWS_WITH(
            OIDCTestFixture(
                OIDCTestURI().with_extra_options("authMechanismProperties=ENVIRONMENT:azure,ALLOWED_HOSTS:").build(),
                opts,
                is_pooled),
            Catch::Matchers::ContainsSubstring("Unsupported 'MONGODB-OIDC' authentication mechanism property"));
    }

    // 3.1 is not implemented. Requires modifying "Client Cache" which is internal to libmongoc.

    SECTION("3.2 Authentication failures without cached tokens return an error") {
        auto callback_call_count = 0u;

        // Spec: "Create an OIDC configured client with an OIDC callback that always returns invalid access tokens"
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;
            return oidc_credential(std::string("bad_token"));
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        OIDCTestFixture tf(OIDCTestURI().build(), opts, is_pooled);

        // Spec: "Perform a `find` operation that fails"
        CHECK_THROWS_WITH(
            tf.client().database("test").collection("test").find_one(scoped_bson{}.view()),
            Catch::Matchers::ContainsSubstring("Authentication failed"));

        // Spec: "Assert that the callback was called 1 time"
        CHECK(callback_call_count == 1u);
    }

    SECTION("3.3 Unexpected error code does not clear the cache") {
        auto callback_call_count = 0u;
        auto const token = read_token_from_file();
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;
            return oidc_credential(token);
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        auto const appname = "oidc-3.3";
        OIDCTestFixture tf(OIDCTestURI().with_appname(appname).build(), opts, is_pooled);

        // Spec: "Set a fail point for `saslStart` commands"
        fail_command_guard const guard("saslStart", 20, appname);

        // Spec: "Perform a `find` operation that fails"
        CHECK_THROWS_WITH(
            tf.client().database("test").collection("test").find_one(scoped_bson{}.view()),
            Catch::Matchers::ContainsSubstring("failpoint"));

        // Spec: "Assert that the callback has been called once"
        CHECK(callback_call_count == 1u);

        // Spec: "Perform a `find` operation that succeeds"
        CHECK_NOTHROW(tf.client().database("test").collection("test").find_one(scoped_bson{}.view()));

        // Spec: "Assert that the callback has been called once"
        CHECK(callback_call_count == 1u);
    }

    SECTION("4.1 Reauthentication Succeeds") {
        auto callback_call_count = 0u;
        auto const token = read_token_from_file();
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;
            return oidc_credential(token);
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        auto const appname = "oidc-4.1";
        OIDCTestFixture tf(OIDCTestURI().with_appname(appname).build(), opts, is_pooled);

        // Spec: "Set a fail point for `find` commands"
        fail_command_guard const guard("find", 391, appname);

        // Spec: "Perform a `find` operation that succeeds"
        CHECK_NOTHROW(tf.client().database("test").collection("test").find_one(scoped_bson{}.view()));

        // Spec: "Assert that the callback was called 2 times"
        CHECK(callback_call_count == 2u);
    }

    SECTION("4.2 Read Commands Fail If Reauthentication Fails") {
        // Spec: "Create a `MongoClient` whose OIDC callback returns one good token and then bad tokens after the first
        // call."
        auto callback_call_count = 0u;
        auto const token = read_token_from_file();
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;
            if (callback_call_count == 1u) {
                return oidc_credential(token);
            } else {
                return oidc_credential(std::string("bad_token"));
            }
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        auto const appname = "oidc-4.2";
        OIDCTestFixture tf(OIDCTestURI().with_appname(appname).build(), opts, is_pooled);

        // Spec: "Perform a `find` operation that succeeds"
        CHECK_NOTHROW(tf.client().database("test").collection("test").find_one(scoped_bson{}.view()));

        // Spec: "Set a fail point for `find` commands"
        fail_command_guard const guard("find", 391, appname);

        // Spec: "Perform a `find` operation that fails."
        CHECK_THROWS_WITH(
            tf.client().database("test").collection("test").find_one(scoped_bson{}.view()),
            Catch::Matchers::ContainsSubstring("Authentication failed"));

        // Spec: "Assert that the callback was called 2 times"
        CHECK(callback_call_count == 2u);
    }

    SECTION("4.3 Write Commands Fail If Reauthentication Fails") {
        // Spec: "Create a `MongoClient` whose OIDC callback returns one good token and then bad tokens after the first
        // call."
        auto callback_call_count = 0u;
        auto const token = read_token_from_file();
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;
            if (callback_call_count == 1u) {
                return oidc_credential(token);
            } else {
                return oidc_credential(std::string("bad_token"));
            }
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        auto const appname = "oidc-4.3";
        OIDCTestFixture tf(OIDCTestURI().with_appname(appname).build(), opts, is_pooled);

        // Spec: "Perform a `insert` operation that succeeds"
        CHECK_NOTHROW(tf.client().database("test").collection("test").insert_one(scoped_bson{}.view()));

        // Spec: "Set a fail point for `insert` commands"
        fail_command_guard const guard("insert", 391, appname);

        // Spec: "Perform a `insert` operation that fails."
        CHECK_THROWS_WITH(
            tf.client().database("test").collection("test").insert_one(scoped_bson{}.view()),
            Catch::Matchers::ContainsSubstring("Authentication failed"));

        // Spec: "Assert that the callback was called 2 times"
        CHECK(callback_call_count == 2u);
    }

    // 4.4 is not implemented. Requires modifying "Client Cache" which is internal to libmongoc.

    SECTION("4.5 Reauthentication Succeeds when a Session is involved") {
        auto callback_call_count = 0u;
        auto const token = read_token_from_file();
        oidc_callback cb = [&](oidc_callback_params const&) {
            callback_call_count++;
            return oidc_credential(token);
        };

        auto opts = v1::client::options();
        opts.oidc_callback(cb);

        // Spec: "Create an OIDC configured client"
        bool const is_pooled = GENERATE(true, false);
        CAPTURE(is_pooled);
        auto const appname = "oidc-4.5";
        OIDCTestFixture tf(OIDCTestURI().with_appname(appname).build(), opts, is_pooled);

        // Spec: "Set a fail point for `find` commands"
        fail_command_guard const guard("find", 391, appname);

        // Spec: "Start a new session"
        auto session = tf.client().start_session();

        // Spec: "Perform a `find` operation that succeeds"
        CHECK_NOTHROW(tf.client().database("test").collection("test").find_one(session, scoped_bson{}.view()));

        // Spec: "Assert that the callback was called 2 times"
        CHECK(callback_call_count == 2u);
    }

    // 5 is not implemented. Tested by libmongoc and requires no additional C++ driver code.
}

} // namespace v1
} // namespace mongocxx
