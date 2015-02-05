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

#include <mongo/driver/write_concern.hpp>

using namespace mongo;
using namespace mongo::driver;

TEST_CASE("a default write_concern", "[write_concern]") {
    write_concern wc{};

    SECTION("doesn't require the server to fsync") {
        REQUIRE(wc.fsync() == false);
    }

    SECTION("doesn't require the server to journal") {
        REQUIRE(wc.journal() == false);
    }

    SECTION("will not timeout") {
        auto timeout = wc.timeout();
        REQUIRE(timeout == decltype(timeout)::zero());
    }

    SECTION("will require confirmation from just the primary or standalone mongod") {
        // TODO: fix me
        REQUIRE(-2 == wc.nodes());
    }

    SECTION("has empty tag set") {
        REQUIRE(wc.tag().empty());
    }
}

TEST_CASE("write_concern fields may be set and retrieved", "[write_concern]") {
    write_concern wc{};

    SECTION("fsync may be configured") {
        wc.fsync(true);
        REQUIRE(wc.fsync() == true);
    }

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
        REQUIRE(tag == wc.tag());
    }

    SECTION("the number of nodes requiring confirmation may be set to a number") {
        wc.nodes(10);
        REQUIRE(wc.nodes() == 10);
    }

    SECTION("the number of nodes requiring confirmation may be set to the majority") {
        wc.majority(std::chrono::milliseconds(100));
        REQUIRE(wc.majority());
        REQUIRE(100 == wc.timeout().count());
    }
}

TEST_CASE("confirmation from tags, a repl-member count, and majority are mutually exclusive",
          "[write_concern]") {
    SECTION("setting the confirmation number unsets the confirmation tag") {
        write_concern wc{};
        wc.tag("MultipleDC");
        wc.nodes(10);
        REQUIRE(wc.tag().empty());
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
        REQUIRE(-4 == wc.nodes());
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
        REQUIRE(-3 == wc.nodes());
    }

    SECTION("setting majority unsets the tag") {
        write_concern wc{};
        wc.tag("MultipleDC");
        wc.majority(std::chrono::milliseconds(100));
        REQUIRE(wc.tag().empty());
    }
}
