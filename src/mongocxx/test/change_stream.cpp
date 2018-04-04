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

#include <atomic>
#include <chrono>
#include <iostream>
#include <list>
#include <thread>
#include <vector>

#include <mongocxx/private/libbson.hh>
#include <bson.h>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/write_exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/read_concern.hpp>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/write_concern.hpp>
#include <third_party/catch/include/helpers.hpp>
#include <queue>

namespace {

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

using namespace mongocxx;
/*
 * Test-cases:

TODO Remaining:
    call .begin() to resume
    add range-based for usage
    should we be using cbegin etc?
    assert behavior of http://en.cppreference.com/w/cpp/concept/Iterator (mostly done?)
    assert behavior of http://en.cppreference.com/w/cpp/concept/InputIterator
    Do ^^ for cursor::iterator too or cut a CXX ticket for the same

Done:
    no pipeline
    copy-construct
    move-construct
    no more data
    multiple calls to begin()
    .end() == .end()
    user-constructed iterator == .end()

error cases (TODO how to simulate error?)
    mal-formed pipeline (can't)
    error response -  with self.coll.watch([{'$project': {'_id': 0}}]) as change_stream
    after error we don't hold onto last doc
    calling .begin() after error doesn't crash
    accessing the documenting with operator* and operator-> after an error doesn't crash
*/

///
/// Create a single-item document
/// E.g. doc("foo", 123) creates {"foo":123}
///
template <typename T>
inline bsoncxx::document::value doc(std::string key, T val) {
    bsoncxx::builder::basic::document out{};
    out.append(kvp(key, val));
    return std::move(out.extract());
}

class response {
public:
    using ptr = std::unique_ptr<bson_t, void(*)(bson_t*)>;

    static ptr as_doc(bsoncxx::document::view_or_value&& doc) {
        return std::move(ptr {
            bson_new_from_data(doc.view().data(), doc.view().length()),
            bson_destroy
        });
    }

    response(bool next, bool error, bsoncxx::document::view_or_value&& doc)
    : next_{next}, error_{error}, doc_{std::move(as_doc(std::move(doc)))}
    {}

    response(response&& other)
    : next_{other.next_}, error_{other.error_}, doc_{std::move(other.doc_)}
    {}

    response(const response& other) = delete;

    bool error() const {
        return error_;
    }
    bson_t* bson() {
        return doc_.get();
    }

private:
    const bool next_;
    const bool error_;
    ptr doc_;
};

struct mock_stream_state {
    std::vector<response> responses;
    unsigned long position;

    bool destroyed = false;

//    mock_stream_state(std::initializer_list<response> args)
//    : position{0}, responses{args} {}
//    : mock_stream_state{std::move(std::vector<response>{args})} {}

    mock_stream_state()
    : position{0}, responses{} {}

    template<typename... Args>
    mock_stream_state& then(Args&&... args) {
        responses.emplace_back(std::forward<Args>(args)...);
        return *this;
    }

//    template<typename... Args>
//    explicit mock_stream_state(Args&&... args)
//    : responses{std::forward<Args>(args)...}, position{0} {}

//    explicit mock_stream_state()
//    : mock_stream_state{{}} {}

    template <typename F>  // uref
    void next_op(F&& f) {
        f->interpose([&](mongoc_change_stream_t* stream, const bson_t** bson) -> bool {
            return this->next(stream, bson);
        }).forever();
    }

    template <typename F>
    void watch_op(F&& f) {
        // way to DRY this up?
        f->interpose([&](const mongoc_collection_t* coll,
                         const bson_t* pipeline,
                         const bson_t* opts) -> mongoc_change_stream_t* {
            return this->watch(coll, pipeline, opts);
        }).forever();
    }

    template <typename F>
    void destroy_op(F&& f) {
        f->interpose([&](mongoc_change_stream_t* stream) -> void {
            return this->destroy(stream);
        }).forever();
    }

    template <typename F>
    void error_op(F&& f) {
        f->interpose([&](const mongoc_change_stream_t* stream,
                         bson_error_t* err,
                         const bson_t** bson) -> bool {
            return this->error(stream, err, bson);
        }).forever();
    }

    bool next(mongoc_change_stream_t* stream, const bson_t** bson) {
        response& current = responses.at(position);
        *bson = current.bson();
        ++position;
        return true;
    }

    bool error(const mongoc_change_stream_t* stream, bson_error_t* err, const bson_t** bson) {
        return false;
    }

    void destroy(mongoc_change_stream_t* stream) {
        destroyed = true;
    }

    mongoc_change_stream_t* watch(const mongoc_collection_t* coll,
                                  const bson_t* pipeline,
                                  const bson_t* opts) {
        return nullptr;
    }
};

template <typename T>
class TD;

SCENARIO("We have errors") {
    MOCK_CHANGE_STREAM

    instance::current();
    client mongodb_client{uri{}};
    options::change_stream options{};

    database db = mongodb_client["streams"];
    collection events = db["events"];

    using namespace std;

    mock_stream_state state;
    state.then(true, false, make_document(kvp("a","b")));
    state.then(true, false, make_document(kvp("b","c")));
    state.then(true, false, make_document(kvp("d","e")));

    state.watch_op(collection_watch);
    state.destroy_op(change_stream_destroy);
    state.next_op(change_stream_next);
    state.error_op(change_stream_error_document);

    WHEN("We watch") {
        THEN("There is an error") {
            auto stream = events.watch();
            change_stream::iterator it = stream.begin();

            string json = bsoncxx::to_json(*it);
            CAPTURE(json);
            REQUIRE(it != stream.end());

            change_stream::iterator it2 = stream.begin();
            string json2 = bsoncxx::to_json(*it);
            CAPTURE(json2);

            it++;

            CAPTURE(bsoncxx::to_json(*it));
            CAPTURE(bsoncxx::to_json(*it2));

            REQUIRE(false);
        }
    }
}

SCENARIO("We project data") {
    instance::current();
    client mongodb_client{uri{}};
    options::change_stream options{};

    database db = mongodb_client["streams"];
    collection events = db["events"];

    using namespace std;

    GIVEN("We don't project the ID") {
        WHEN("We try to create a change stream") {
            auto pipe = pipeline{};
            //            TODO: why does the server not barf on this?
            //            pipe.unwind(doc("invalid","[]"));
            //            options.full_document(bsoncxx::string::view_or_value{"default"});
            pipe.project(doc("_id", 0));
            auto stream = events.watch(pipe, options);
            REQUIRE(events.insert_one(doc("a", "b")));

            THEN("We get an error") {
                // TODO: doesn't actually throw ....
                REQUIRE_THROWS(stream.begin());
            }
        }
    }
}

SCENARIO("A collection is watched") {
    instance::current();
    client mongodb_client{uri{}};
    options::change_stream options{};

    database db = mongodb_client["streams"];
    collection events = db["events"];

    GIVEN("We have a default change stream") {
        THEN("We can move-assign it") {
            change_stream stream = events.watch();
            change_stream move_copy = std::move(stream);
        }
        THEN("We can move-construct it") {
            change_stream stream = events.watch();
            change_stream move_constructed = change_stream{std::move(stream)};
        }
        THEN(".end == .end") {
            change_stream x = events.watch();
            REQUIRE(x.end() == x.end());
        }
        THEN("We don't have any events") {
            change_stream x = events.watch();
            REQUIRE(x.begin() == x.end());
        }
        THEN("Empty iterator is equivalent to user-constructed iterator") {
            change_stream x = events.watch();
            REQUIRE(x.begin() == change_stream::iterator{});
            REQUIRE(x.end() == change_stream::iterator{});
        }
    }

    GIVEN("We have a single event") {
        change_stream x = events.watch();
        REQUIRE(events.insert_one(doc("a", "b")));

        THEN("We can receive an event") {
            auto it = *(x.begin());
            REQUIRE(it["fullDocument"]["a"].get_utf8().value == stdx::string_view("b"));
        }

        THEN("We can deref iterator with value multiple times") {
            auto it = x.begin();
            auto a = *it;
            auto b = *it;
            REQUIRE(a["fullDocument"]["a"].get_utf8().value == stdx::string_view("b"));
            REQUIRE(b["fullDocument"]["a"].get_utf8().value == stdx::string_view("b"));
        }

        THEN("Calling .begin multiple times doesn't advance state") {
            auto a = *(x.begin());
            auto b = *(x.begin());
            REQUIRE(a == b);
        }

        THEN("We have no more events after the first one") {
            auto it = x.begin();
            it++;
            REQUIRE(it == x.end());
            REQUIRE(x.begin() == x.end());
        }

        THEN("Past end is empty document") {
            auto it = x.begin();
            it++;
            REQUIRE(*it == bsoncxx::builder::basic::document{});
        }

        THEN("Can dereference end()") {
            auto it = x.begin();
            it++;
            REQUIRE(*it == *it);
        }
    }
}

TEST_CASE("Change Streams") {
    instance::current();

    client mongodb_client{uri{}};
    database db = mongodb_client["streams"];
    collection events = db["events"];

    options::change_stream options{};
    options.max_await_time(std::chrono::milliseconds{50000});

    change_stream stream = events.watch(options);

    change_stream::iterator it = stream.begin();
    change_stream::iterator it2 = it;
    change_stream::iterator it3 = {std::move(it2)};

    for (const auto& it : stream) {
        printf("Got:  %s\n", bsoncxx::to_json(it).c_str());
        std::cout << bsoncxx::to_json(it) << std::endl;
    }

    for (auto it = stream.begin(); it != stream.end(); ++it) {
        printf("Got:  %s\n", bsoncxx::to_json(*it).c_str());
        std::cout << bsoncxx::to_json(*it) << std::endl;
    }

    REQUIRE(events);
}

}  // namepsace
