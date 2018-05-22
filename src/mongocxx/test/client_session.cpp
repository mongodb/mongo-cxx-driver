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

#include <sstream>

#include <helpers.hpp>

#include <bsoncxx/private/helpers.hh>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/test_util/client_helpers.hh>

namespace {
using bsoncxx::from_json;
using bsoncxx::document::value;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::types::b_timestamp;

using namespace mongocxx;

bool server_has_sessions(const client& conn) {
    auto result = conn["admin"].run_command(make_document(kvp("isMaster", 1)));
    auto result_view = result.view();

    if (result_view["logicalSessionTimeoutMinutes"]) {
        return true;
    }

    WARN("skip: server does not support sessions");
    return false;
}

TEST_CASE("session options", "[session]") {
    instance::current();

    client c{uri{}};

    if (!server_has_sessions(c)) {
        return;
    }

    SECTION("default") {
        auto s = c.start_session();

        REQUIRE(s.options().causal_consistency());
    }

    SECTION("set causal consistency") {
        options::client_session opts;
        REQUIRE(opts.causal_consistency());
        opts.causal_consistency(false);
        REQUIRE(!opts.causal_consistency());

        auto s = c.start_session(opts);
        REQUIRE(!s.options().causal_consistency());
    }
}

TEST_CASE("start_session failure", "[session]") {
    using namespace mongocxx::test_util;

    MOCK_CLIENT

    instance::current();

    client_start_session
        ->interpose([](mongoc_client_t*, const mongoc_session_opt_t*, bson_error_t* error) {
            bson_set_error(error, MONGOC_ERROR_CLIENT, MONGOC_ERROR_CLIENT_SESSION_FAILURE, "foo");
            return (mongoc_client_session_t*)nullptr;
        })
        .forever();

    client c{uri{}};

    REQUIRE_THROWS_MATCHES(
        c.start_session(), mongocxx::exception, mongocxx_exception_matcher{"foo"});
}

TEST_CASE("session", "[session]") {
    using namespace mongocxx::test_util;

    instance::current();

    client c{uri{}};

    if (!server_has_sessions(c)) {
        return;
    }

    auto s = c.start_session();

    SECTION("id") {
        REQUIRE(!s.id().empty());
    }

    SECTION("cluster time and operation time") {
        b_timestamp zero, nonzero;
        zero.timestamp = 0;
        zero.increment = 0;
        nonzero.timestamp = 1;
        nonzero.increment = 0;

        // The session hasn't been used for anything yet.
        REQUIRE(s.cluster_time().empty());
        REQUIRE(s.operation_time() == zero);

        // Advance the cluster time - just a basic test, rely on libmongoc's logic.
        s.advance_cluster_time(
            from_json("{\"clusterTime\": {\"$timestamp\": {\"t\": 1, \"i\": 0}}}"));
        REQUIRE(!s.cluster_time().empty());
        REQUIRE(s.operation_time() == zero);

        // Advance the operation time, just a basic test again.
        s.advance_operation_time(nonzero);
        REQUIRE(s.operation_time() == nonzero);
    }

    SECTION("pool") {
        // "Pool is LIFO" test from Driver Sessions Spec.
        auto session_a = stdx::make_unique<client_session>(c.start_session());
        auto session_b = stdx::make_unique<client_session>(c.start_session());
        auto a_id = value(session_a->id());
        auto b_id = value(session_b->id());

        // End session A, then session B.
        session_a = nullptr;
        session_b = nullptr;

        auto session_c = stdx::make_unique<client_session>(c.start_session());
        REQUIRE(session_c->id() == b_id);
        auto session_d = stdx::make_unique<client_session>(c.start_session());
        REQUIRE(session_d->id() == a_id);
    }

    SECTION("wrong client") {
        using Catch::Matchers::Contains;

        // "Session argument is for the right client" test from Driver Sessions Spec.
        // Passing a session from client "c" should fail with client "c2" and related objects.
        client c2{uri{}};
        auto db2 = c2["db"];
        auto collection2 = db2["collection"];

#define REQUIRE_THROWS_INVALID_SESSION(_expr) \
    REQUIRE_THROWS_MATCHES(                   \
        (_expr), mongocxx::exception, mongocxx_exception_matcher{"Invalid sessionId"})

        REQUIRE_THROWS_INVALID_SESSION(collection2.count(s, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.create_index(s, make_document(kvp("a", 1))));
        REQUIRE_THROWS_INVALID_SESSION(collection2.distinct(s, "a", {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.drop(s));
        REQUIRE_THROWS_INVALID_SESSION(collection2.find_one(s, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.rename(s, "foo", true));
        REQUIRE_THROWS_INVALID_SESSION(db2.create_collection(s, "foo"));
        REQUIRE_THROWS_INVALID_SESSION(db2.run_command(s, {}));

        // Test iterators.
        auto cursor = c2.list_databases(s);
        REQUIRE_THROWS_INVALID_SESSION(cursor.begin());
        cursor = collection2.aggregate(s, {});
        REQUIRE_THROWS_INVALID_SESSION(cursor.begin());
        cursor = collection2.list_indexes(s);
        REQUIRE_THROWS_INVALID_SESSION(cursor.begin());
        cursor = db2.list_collections(s);
        REQUIRE_THROWS_INVALID_SESSION(cursor.begin());
        cursor = collection2.find(s, {});
        REQUIRE_THROWS_INVALID_SESSION(cursor.begin());
        auto stream = collection2.watch(s);
        REQUIRE_THROWS_INVALID_SESSION(stream.begin());
        auto indexes2 = collection2.indexes();
        cursor = indexes2.list(s);
        REQUIRE_THROWS_INVALID_SESSION(cursor.begin());

        // Test CRUD member functions.
        std::vector<model::write> writes;
        std::vector<bsoncxx::document::view> docs;
        auto bulk = collection2.create_bulk_write(s);

        REQUIRE_THROWS_INVALID_SESSION(bulk.execute());
        REQUIRE_THROWS_INVALID_SESSION(collection2.bulk_write(s, writes));
        REQUIRE_THROWS_INVALID_SESSION(collection2.delete_many(s, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.delete_one(s, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.find_one_and_delete(s, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.find_one_and_replace(s, {}, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.find_one_and_update(s, {}, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.insert_many(s, docs));
        REQUIRE_THROWS_INVALID_SESSION(collection2.insert_one(s, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.replace_one(s, {}, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.update_many(s, {}, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.update_one(s, {}, {}));
        REQUIRE_THROWS_INVALID_SESSION(collection2.write(s, model::insert_one{{}}));

        // Test index_view member functions.
        std::vector<index_model> models;

        REQUIRE_THROWS_INVALID_SESSION(indexes2.create_one(s, make_document(kvp("a", 1))));
        REQUIRE_THROWS_INVALID_SESSION(indexes2.create_many(s, models));
        REQUIRE_THROWS_INVALID_SESSION(indexes2.drop_one(s, "foo"));
        REQUIRE_THROWS_INVALID_SESSION(indexes2.drop_all(s));

        // Test gridfs::bucket.
        auto bucket2 = db2.gridfs_bucket();
        auto one = bsoncxx::types::value{bsoncxx::types::b_int32{1}};

        REQUIRE_THROWS_INVALID_SESSION(bucket2.open_upload_stream(s, "file"));
        REQUIRE_THROWS_INVALID_SESSION(bucket2.open_download_stream(s, one));

#undef REQUIRE_THROWS_INVALID_SESSION
    }
}

// Receive command-started events from libmongoc's APM to test session ids.
// TODO: Port to C++ Driver's APM once it's implemented, CXX-1562.
void command_started(const mongoc_apm_command_started_t* event);

class session_test {
   public:
    session_test() : client{uri{}} {
        auto client_t = static_cast<mongoc_client_t*>(client_t_from_client(client));
        auto callbacks = mongoc_apm_callbacks_new();
        mongoc_apm_set_command_started_cb(callbacks, command_started);
        mongoc_client_set_apm_callbacks(client_t, callbacks, this);
        mongoc_apm_callbacks_destroy(callbacks);
    }

    void test_method_with_session(const std::function<void(bool)>& f, const client_session& s) {
        using std::string;

        events.clear();

        // A method with an explicit session must send its logical session id or
        // "lsid".
        f(true);
        if (events.size() == 0) {
            throw std::logic_error{"no events after calling command with explicit session"};
        }

        for (auto& event : events) {
            if (!event.command["lsid"]) {
                throw std::logic_error{"no lsid in " + event.command_name +
                                       " with explicit session"};
            }
            if (event.command["lsid"].get_document().view() != s.id()) {
                throw std::logic_error{"wrong lsid in " + event.command_name +
                                       " with explicit session"};
            }
        }

        events.clear();

        // A method called with no session must send an implicit session id with the
        // command.
        f(false);
        if (events.size() == 0) {
            throw std::logic_error{"no events after calling command with implicit session"};
        }

        for (auto& event : events) {
            if (!event.command["lsid"]) {
                throw std::logic_error{"no lsid in " + event.command_name +
                                       " with implicit session"};
            }
            if (event.command["lsid"].get_document().view() == s.id()) {
                throw std::logic_error{"wrong lsid in " + event.command_name +
                                       " with implicit session"};
            }
        }
    }

    class apm_event {
       public:
        apm_event(const std::string& command_name_, const bsoncxx::document::value& document_)
            : command_name(command_name_), value(document_), command(value.view()) {}

        std::string command_name;
        bsoncxx::document::value value;
        bsoncxx::document::view command;
    };

    std::vector<apm_event> events;
    mongocxx::client client;
};

void command_started(const mongoc_apm_command_started_t* event) {
    using namespace bsoncxx::helpers;

    std::string command_name{mongoc_apm_command_started_get_command_name(event)};

    // Ignore auth commands like "saslStart", and handshakes with "isMaster".
    std::string sasl{"sasl"};
    if (command_name.substr(0, sasl.size()) == sasl || command_name == "isMaster") {
        return;
    }

    auto& listener =
        *(reinterpret_cast<session_test*>(mongoc_apm_command_started_get_context(event)));
    auto document = value_from_bson_t(mongoc_apm_command_started_get_command(event));

    listener.events.emplace_back(command_name, document);
}

TEST_CASE("lsid", "[session]") {
    instance::current();

    session_test test;

    if (!server_has_sessions(test.client)) {
        return;
    }

    auto s = test.client.start_session();
    auto db = test.client["lsid"];
    auto bucket = db.gridfs_bucket();
    auto collection = db["collection"];
    auto indexes = collection.indexes();
    collection.drop();

    // Ensure some data for aggregate() and find().
    for (int i = 0; i != 10; ++i) {
        collection.insert_one({});
    }

    SECTION("bucket") {
        auto f = [&s, &bucket, &db](bool use_session) {
            // Start clean.
            use_session ? db["fs.files"].drop(s) : db["fs.files"].drop();
            use_session ? db["fs.chunks"].drop(s) : db["fs.chunks"].drop();

            auto one = bsoncxx::types::value{bsoncxx::types::b_int32{1}};
            auto two = bsoncxx::types::value{bsoncxx::types::b_int32{2}};
            auto data = (uint8_t*)"foo";
            size_t len = 4;
            // Ensure multiple chunks.
            options::gridfs::upload opts;
            opts.chunk_size_bytes(1);

            auto up = use_session ? bucket.open_upload_stream(s, "file", opts)
                                  : bucket.open_upload_stream("file", opts);
            up.write(data, len);
            up.close();

            up = use_session ? bucket.open_upload_stream_with_id(s, one, "file", opts)
                             : bucket.open_upload_stream_with_id(one, "file", opts);
            up.write(data, len);
            up.close();

            up = use_session ? bucket.open_upload_stream_with_id(s, one, "file", opts)
                             : bucket.open_upload_stream_with_id(one, "file", opts);
            up.write(data, len);
            up.abort();

            std::stringstream stream("foo");
            use_session ? bucket.upload_from_stream(s, "file", &stream, opts)
                        : bucket.upload_from_stream("file", &stream, opts);
            use_session ? bucket.upload_from_stream_with_id(s, two, "file", &stream, opts)
                        : bucket.upload_from_stream_with_id(two, "file", &stream, opts);

            auto down = use_session ? bucket.open_download_stream(s, two)
                                    : bucket.open_download_stream(two);
            use_session ? bucket.download_to_stream(s, two, &stream)
                        : bucket.download_to_stream(two, &stream);

            int total = 0;
            auto cursor = use_session ? bucket.find(s, {}) : bucket.find({});
            for (auto&& result : cursor) {
                ++total;
            }

            REQUIRE(total == 4);
            use_session ? bucket.delete_file(s, one) : bucket.delete_file(one);
        };

        test.test_method_with_session(f, s);
    }

    SECTION("client::list_databases") {
        auto f = [&s, &test](bool use_session) {
            int total = 0;
            auto cursor =
                use_session ? test.client.list_databases(s) : test.client.list_databases();
            for (auto&& result : cursor) {
                ++total;
            }

            REQUIRE(total > 0);
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::aggregate") {
        auto f = [&s, &collection](bool use_session) {
            options::aggregate opts;
            opts.batch_size(2);
            int total = 0;
            auto cursor =
                use_session ? collection.aggregate(s, {}, opts) : collection.aggregate({}, opts);
            for (auto&& doc : cursor) {
                ++total;
            }

            REQUIRE(total == 10);
        };

        test.test_method_with_session(f, s);
        REQUIRE(test.events.size() >= 2);
    }

    SECTION("collection::bulk_write") {
        std::vector<model::write> vec;
        vec.emplace_back(model::insert_one{{}});

        auto bulk_write_vector = [&s, &collection, &vec](bool use_session) {
            use_session ? collection.bulk_write(s, vec) : collection.bulk_write(vec);
        };

        test.test_method_with_session(bulk_write_vector, s);

        auto bulk_write_iterator = [&s, &collection, &vec](bool use_session) {
            use_session ? collection.bulk_write(s, vec.begin(), vec.end())
                        : collection.bulk_write(vec.begin(), vec.end());
        };

        test.test_method_with_session(bulk_write_iterator, s);
    }

    SECTION("collection::count") {
        auto f = [&s, &collection](bool use_session) {
            auto total = use_session ? collection.count(s, {}) : collection.count({});
            REQUIRE(total == 10);
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::create_bulk_write") {
        auto f = [&s, &collection](bool use_session) {
            auto bulk =
                use_session ? collection.create_bulk_write(s) : collection.create_bulk_write();

            bulk.append(model::insert_one{{}});
            bulk.execute();
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::create_index") {
        auto f = [&s, &collection](bool use_session) {
            if (use_session) {
                collection.drop(s);
                collection.create_index(s, make_document(kvp("a", 1)));
            } else {
                collection.drop();
                collection.create_index(make_document(kvp("a", 1)));
            }
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::delete_many") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.delete_many(s, {}) : collection.delete_many({});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::delete_one") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.delete_one(s, {}) : collection.delete_one({});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::distinct") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.distinct(s, "a", {}) : collection.distinct("a", {});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::find") {
        auto f = [&s, &collection](bool use_session) {
            options::find opts;
            opts.batch_size(2);
            int total = 0;
            auto cursor = use_session ? collection.find(s, {}, opts) : collection.find({}, opts);

            for (auto&& doc : cursor) {
                ++total;
            }

            REQUIRE(total == 10);
        };

        test.test_method_with_session(f, s);
        // A find and a getMore.
        REQUIRE(test.events.size() >= 2);
    }

    SECTION("collection::find_one") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.find_one(s, {}) : collection.find_one({});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::find_one_and_delete") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.find_one_and_delete(s, {})
                        : collection.find_one_and_delete({});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::find_one_and_replace") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.find_one_and_replace(s, {}, {})
                        : collection.find_one_and_replace({}, {});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::find_one_and_update") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.find_one_and_update(s, {}, {})
                        : collection.find_one_and_update({}, {});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::insert_many") {
        bsoncxx::document::value doc({});
        std::vector<bsoncxx::document::view> docs{doc.view()};

        auto insert_vector = [&s, &collection, &docs](bool use_session) {
            return use_session ? collection.insert_many(s, docs) : collection.insert_many(docs);
        };

        test.test_method_with_session(insert_vector, s);

        auto insert_iter = [&s, &collection, &docs](bool use_session) {
            use_session ? collection.insert_many(s, docs.begin(), docs.end())
                        : collection.insert_many(docs.begin(), docs.end());
        };

        test.test_method_with_session(insert_iter, s);
    }

    SECTION("collection::insert_one") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.insert_one(s, {}) : collection.insert_one({});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::list_indexes") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.list_indexes(s) : collection.list_indexes();
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::rename") {
        auto f = [&s, &collection](bool use_session) {
            if (use_session) {
                collection.insert_one(s, {});
                auto c2 = collection;
                c2.rename(s, "collection2", true);
            } else {
                collection.insert_one({});
                auto c2 = collection;
                c2.rename("collection2", true);
            }
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::replace_one") {
        auto f = [&s, &collection](bool use_session) {
            use_session ? collection.replace_one(s, {}, {}) : collection.replace_one({}, {});
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::update_many") {
        auto f = [&s, &collection](bool use_session) {
            auto u = make_document(kvp("$set", make_document(kvp("a", 1))));
            use_session ? collection.update_many(s, {}, u.view())
                        : collection.update_many({}, u.view());
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::update_one") {
        auto f = [&s, &collection](bool use_session) {
            auto u = make_document(kvp("$set", make_document(kvp("a", 1))));
            use_session ? collection.update_one(s, {}, u.view())
                        : collection.update_one({}, u.view());
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::watch") {
        if (!test_util::is_replica_set(test.client)) {
            WARN("skip: watch() requires replica set");
            return;
        }

        auto f = [&s, &collection](bool use_session) {
            options::change_stream opts;
            opts.max_await_time(std::chrono::milliseconds(1));
            auto stream = use_session ? collection.watch(s, opts) : collection.watch(opts);
            for (auto&& event : stream) {
            }
        };

        test.test_method_with_session(f, s);
    }

    SECTION("collection::write") {
        auto f = [&s, &collection](bool use_session) {
            auto insert_op = model::insert_one{{}};
            use_session ? collection.write(s, insert_op) : collection.write(insert_op);
        };

        test.test_method_with_session(f, s);
    }

    SECTION("create and drop collection") {
        auto f = [&s, &db](bool use_session) {
            use_session ? db.create_collection(s, "foo") : db.create_collection("foo");
            use_session ? db["foo"].drop(s) : db["foo"].drop();
        };

        test.test_method_with_session(f, s);
    }

    SECTION("database::drop") {
        auto f = [&s, &db](bool use_session) { use_session ? db.drop(s) : db.drop(); };

        test.test_method_with_session(f, s);
    }

    SECTION("database::list_collections") {
        auto f = [&s, &db](bool use_session) {
            auto cursor = use_session ? db.list_collections(s) : db.list_collections();
            for (auto&& result : cursor) {
            }
        };

        test.test_method_with_session(f, s);
    }

    SECTION("database::run_command") {
        auto f = [&s, &db](bool use_session) {
            auto cmd = make_document(kvp("ping", 1));
            use_session ? db.run_command(s, cmd.view()) : db.run_command(cmd.view());
        };

        test.test_method_with_session(f, s);
    }

    SECTION("index_view::list") {
        auto f = [&s, &indexes](bool use_session) {
            auto cursor = use_session ? indexes.list(s) : indexes.list();
            for (auto&& i : cursor) {
            }
        };

        test.test_method_with_session(f, s);
    }

    SECTION("index_view::create_one") {
        auto create_with_keys = [&s, &indexes](bool use_session) {
            auto keys = make_document(kvp("a", 1));
            use_session ? indexes.create_one(s, keys.view()) : indexes.create_one(keys.view());
        };

        test.test_method_with_session(create_with_keys, s);

        auto create_with_model = [&s, &indexes](bool use_session) {
            auto model = index_model{make_document(kvp("a", 1))};
            use_session ? indexes.create_one(s, model) : indexes.create_one(model);
        };

        test.test_method_with_session(create_with_model, s);
    }

    SECTION("index_view::create_many") {
        auto f = [&s, &indexes](bool use_session) {
            auto models = std::vector<index_model>{index_model{make_document(kvp("a", 1))},
                                                   index_model{make_document(kvp("b", 1))}};

            use_session ? indexes.create_many(s, models) : indexes.create_many(models);
        };

        test.test_method_with_session(f, s);
    }

    SECTION("index_view::drop_one") {
        auto drop_by_name = [&s, &indexes](bool use_session) {
            auto keys = make_document(kvp("a", 1));
            auto name =
                use_session ? indexes.create_one(s, keys.view()) : indexes.create_one(keys.view());
            use_session ? indexes.drop_one(s, name.value()) : indexes.drop_one(name.value());
        };

        test.test_method_with_session(drop_by_name, s);

        auto drop_by_keys = [&s, &indexes](bool use_session) {
            auto keys = make_document(kvp("a", 1));
            use_session ? indexes.create_one(s, keys.view()) : indexes.create_one(keys.view());
            use_session ? indexes.drop_one(s, keys.view()) : indexes.drop_one(keys.view());
        };

        test.test_method_with_session(drop_by_keys, s);

        auto drop_by_model = [&s, &indexes](bool use_session) {
            auto keys = make_document(kvp("a", 1));
            use_session ? indexes.create_one(s, keys.view()) : indexes.create_one(keys.view());
            auto model = index_model{keys.view()};
            use_session ? indexes.drop_one(s, model) : indexes.drop_one(model);
        };

        test.test_method_with_session(drop_by_model, s);
    }

    SECTION("index_view::drop_all") {
        auto f = [&s, &indexes](bool use_session) {
            use_session ? indexes.drop_all(s) : indexes.drop_all();
        };

        test.test_method_with_session(f, s);
    }
}
}  // namespace
