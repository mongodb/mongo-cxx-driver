// Copyright 2014 MongoDB Inc.
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

#include "catch.hpp"

#include <mongocxx/exception/exception.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/write_concern.hpp>

using namespace mongocxx;

TEST_CASE("a default write_concern", "[write_concern]") {
    instance::current();

    write_concern wc{};

    SECTION("doesn't require the server to journal") {
        REQUIRE(wc.journal() == false);
    }

    SECTION("will not timeout") {
        auto timeout = wc.timeout();
        REQUIRE(timeout == decltype(timeout)::zero());
    }

    SECTION("has either acknowledge_level or nodes") {
        bool has_ack_level = wc.acknowledge_level() ? true : false;
        bool has_nodes = wc.nodes() ? true : false;
        REQUIRE(has_ack_level != has_nodes);
    }

    SECTION("has default acknowledge_level") {
        REQUIRE(wc.acknowledge_level());
        REQUIRE(write_concern::level::k_default == *wc.acknowledge_level());
    }

    SECTION("has empty tag set") {
        REQUIRE(!wc.tag());
    }
}

TEST_CASE("write_concern fields may be set and retrieved", "[write_concern]") {
    instance::current();

    write_concern wc{};

    SECTION("journal may be configured") {
        wc.journal(true);
        REQUIRE(wc.journal() == true);
    }

    SECTION("timeout may be configured") {
        wc.timeout(std::chrono::seconds(10));
        REQUIRE(wc.timeout() == std::chrono::seconds(10));
        wc.timeout(std::chrono::milliseconds(250));
        REQUIRE(wc.timeout() == std::chrono::milliseconds(250));
    }

    SECTION("a tag may be set") {
        const std::string tag{"MultipleDC"};
        wc.tag(tag);
        auto maybe_tag = wc.tag();
        REQUIRE(maybe_tag);
        REQUIRE(tag == *maybe_tag);
    }

    SECTION("the number of nodes requiring confirmation may be set to a number") {
        wc.nodes(10);
        REQUIRE(wc.nodes());
        REQUIRE(*wc.nodes() == 10);
    }

    SECTION("can set acknowledge_level") {
        wc.acknowledge_level(write_concern::level::k_unacknowledged);
        REQUIRE(!wc.nodes());
        REQUIRE(write_concern::level::k_unacknowledged == *wc.acknowledge_level());
    }

    SECTION("setting invalid acknowledge levels throws") {
        REQUIRE_THROWS_AS(wc.acknowledge_level(write_concern::level::k_unknown), exception);
        REQUIRE_THROWS_AS(wc.acknowledge_level(write_concern::level::k_tag), exception);
        wc.tag("AnyTag");
        REQUIRE_NOTHROW(wc.acknowledge_level(write_concern::level::k_tag));
    }

    SECTION("the number of nodes requiring confirmation may be set to the majority") {
        wc.majority(std::chrono::milliseconds(100));
        REQUIRE(wc.majority());
        REQUIRE(100 == wc.timeout().count());
    }
}

TEST_CASE("confirmation from tags, a repl-member count, and majority are mutually exclusive",
          "[write_concern]") {
    instance::current();

    SECTION("setting the confirmation number unsets the confirmation tag") {
        write_concern wc{};
        wc.tag("MultipleDC");
        wc.nodes(10);
        REQUIRE(!wc.tag());
    }

    SECTION("setting the confirmation number unsets majority") {
        write_concern wc{};
        wc.majority(std::chrono::milliseconds(100));
        wc.nodes(20);
        REQUIRE(!wc.majority());
    }

    SECTION("setting the tag unsets the confirmation number") {
        write_concern wc{};
        wc.nodes(10);
        wc.tag("MultipleDC");
        REQUIRE(!wc.nodes());
    }

    SECTION("setting the tag sets the acknowledge level") {
        write_concern wc{};
        wc.tag("MultipleDC");
        REQUIRE(wc.acknowledge_level());
        REQUIRE(write_concern::level::k_tag == *wc.acknowledge_level());
    }

    SECTION("setting the tag unsets majority") {
        write_concern wc{};
        wc.majority(std::chrono::milliseconds(100));
        wc.tag("MultipleDC");
        REQUIRE(!wc.majority());
    }

    SECTION("setting the majority unsets the confirmation number") {
        write_concern wc{};
        wc.nodes(10);
        wc.majority(std::chrono::milliseconds(100));
        REQUIRE(write_concern::level::k_majority == *wc.acknowledge_level());
    }

    SECTION("setting majority unsets the tag") {
        write_concern wc{};
        wc.tag("MultipleDC");
        wc.majority(std::chrono::milliseconds(100));
        REQUIRE(!wc.tag());
    }
}
