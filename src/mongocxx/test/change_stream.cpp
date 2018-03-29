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
#include <thread>
#include <vector>

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
#include <mongocxx/read_concern.hpp>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/write_concern.hpp>
#include <mongocxx/private/libbson.hh>
#include <bson.h>
#include <mongocxx/private/libbson.hh>

namespace {

using bsoncxx::builder::basic::document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

using namespace mongocxx;
// TODO: move to different file (?)
// this is just sandbox/exploratory-testing for now
/*
 * Test-cases:
 *      mal-formed pipeline (can't)
 *      no pipeline (done)
 *      copy-construct (done)
 *      move-construct (done)
 *      no more data (done)
 *      error response -  with self.coll.watch([{'$project': {'_id': 0}}]) as change_stream
 *      multiple calls to begin() (done)
 *      .end() == .end() (done)
 *      call .begin() to resume
 *      after error we don't hold onto last doc
 *      calling .begin() after error doesn't crash
 *      accessing the documenting with operator* and operator-> after an error doesn't crash
 *      user-constructed iterator == .end()
 *      tests that iterator meets criteria specified here:
 *          The requirements for iterator equality tests are very weak for input iterators.
 *          http://en.cppreference.com/w/cpp/concept/Iterator
 *          http://en.cppreference.com/w/cpp/concept/InputIterator
 *     And follow the links to things like:
 *      http://en.cppreference.com/w/cpp/concept/EqualityComparable
 *      http://en.cppreference.com/w/cpp/concept/CopyAssignable
 *     It would be great to do that for cursor::iterator too.
 */

template<typename T>
bsoncxx::document::value doc(std::string key, T val) {
    bsoncxx::builder::basic::document out {};
    out.append(kvp(key, val));
    return std::move(out.extract());
}

std::ostream& operator<<(std::ostream&out, const bsoncxx::document::view_or_value& document) {
    out << bsoncxx::to_json(document);
    return out;
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
            // TODO: why does the server not barf on this?
            // TODO: how to simulate an error?
//            pipe.unwind(doc("invalid","[]"));
//            options.full_document(bsoncxx::string::view_or_value{"default"});
            pipe.project(doc("_id", 0));
            auto stream = events.watch(pipe, options);
            REQUIRE(events.insert_one(doc("a","b")));
//            REQUIRE(events.insert_one(doc("a","b")));
//            REQUIRE(events.insert_one(doc("a","b")));

            THEN("We get an error") {
                auto it = stream.begin();
                auto val = *it;
                CAPTURE(val);
                auto operationType = val.operator[]("_id").get_utf8().value;
                CAPTURE(operationType);
//                std::cout << "operationType=" << operationType.get_utf8().value << std::endl;
//                INFO("We got value " << *it << endl); it++;
//                INFO("We got value " << *it << endl); it++;
//                INFO("We got value " << *it << endl); it++;
//                INFO("We got value " << *it << endl); it++;
                CHECK(false);
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
        REQUIRE(events.insert_one(doc("a","b")));

        THEN("We can receive an event") {
            auto it = *(x.begin());
            REQUIRE(it["fullDocument"]["a"].get_utf8().value == "b");
        }

        THEN("We can deref iterator with value multiple times") {
            auto it = x.begin();
            auto a = *it;
            auto b = *it;
            REQUIRE(a["fullDocument"]["a"].get_utf8().value == "b");
            REQUIRE(b["fullDocument"]["a"].get_utf8().value == "b");
        }

        THEN("Calling .begin multiple times doesn't advance state") {
            auto a = *(x.begin());
            auto b = *(x.begin());
            REQUIRE( a == b );
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

    for (auto&& it : stream) {
        printf("Got:  %s\n", bsoncxx::to_json(it).c_str());
        std::cout << bsoncxx::to_json(it) << std::endl;
    }

    for (auto it = stream.begin(); it != stream.end(); ++it) {
        printf("Got:  %s\n", bsoncxx::to_json(*it).c_str());
        std::cout << bsoncxx::to_json(*it) << std::endl;
    }

    REQUIRE(events);
}

} // namepsace
