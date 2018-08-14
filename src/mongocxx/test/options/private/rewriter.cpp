// Copyright 2016 MongoDB Inc.
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

#include "helpers.hpp"

#include <chrono>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/private/suppress_deprecation_warnings.hh>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/private/rewriter.hh>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $comment", "[find][option]") {
    instance::current();

    SECTION("$comment with k_utf8 type is translated") {
        auto find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$comment", "test"))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.comment());
        REQUIRE(*find_opts.comment() == stdx::string_view("test"));
    }

    SECTION("$comment with other types is rejected") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$comment", 1)))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $explain", "[find][option]") {
    instance::current();

    SECTION("$explain isn't supported") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$explain", true)))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $hint", "[find][option]") {
    instance::current();
    options::find find_opts;

    SECTION("$hint with k_utf8 type is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$hint", "index"))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.hint());
        REQUIRE(*find_opts.hint() == "index");
    }

    SECTION("$hint with k_document is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(options::find{}.modifiers_deprecated(
            make_document(kvp("$hint", make_document(kvp("a", 1))))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.hint());
        REQUIRE(*find_opts.hint() == make_document(kvp("a", 1)));
    }

    SECTION("$hint with other types is rejected") {
        REQUIRE_THROWS_AS(options::rewriter::rewrite_find_modifiers(
                              options::find{}.modifiers_deprecated(make_document(kvp("$hint", 1)))),
                          logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $max", "[find][option]") {
    instance::current();

    SECTION("$max with k_document type is translated") {
        auto find_opts =
            options::rewriter::rewrite_find_modifiers(options::find{}.modifiers_deprecated(
                make_document(kvp("$max", make_document(kvp("a", 1))))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.max());
        REQUIRE(*find_opts.max() == make_document(kvp("a", 1)));
    }

    SECTION("$max with other types is rejected") {
        REQUIRE_THROWS_AS(options::rewriter::rewrite_find_modifiers(
                              options::find{}.modifiers_deprecated(make_document(kvp("$max", 1)))),
                          logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $maxScan", "[find][option]") {
    instance::current();
    options::find find_opts;

    SECTION("$maxScan with k_int32 type is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$maxScan", 1))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.max_scan_deprecated());
        REQUIRE(*find_opts.max_scan_deprecated() == 1);
    }

    SECTION("$maxScan with k_int64 type is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$maxScan", std::int64_t{1}))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.max_scan_deprecated());
        REQUIRE(*find_opts.max_scan_deprecated() == 1);
    }

    SECTION("$maxScan with k_double type is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$maxScan", 1.0))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.max_scan_deprecated());
        REQUIRE(*find_opts.max_scan_deprecated() == 1);
    }

    SECTION("$maxScan with other types is rejected") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$maxScan", "foo")))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $maxTimeMS", "[find][option]") {
    instance::current();
    options::find find_opts;

    SECTION("$maxTimeMS with k_int32 type is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$maxTimeMS", 1))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.max_time());
        REQUIRE(find_opts.max_time()->count() == 1);
    }

    SECTION("$maxTimeMS with k_int64 type is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(options::find{}.modifiers_deprecated(
            make_document(kvp("$maxTimeMS", std::int64_t{1}))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.max_time());
        REQUIRE(find_opts.max_time()->count() == 1);
    }

    SECTION("$maxTimeMS with k_double type is translated") {
        find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$maxTimeMS", 1.0))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.max_time());
        REQUIRE(find_opts.max_time()->count() == 1);
    }

    SECTION("$maxTimeMS with other types is rejected") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$maxTimeMS", "foo")))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $min", "[find][option]") {
    instance::current();

    SECTION("$min with k_document type is translated") {
        auto find_opts =
            options::rewriter::rewrite_find_modifiers(options::find{}.modifiers_deprecated(
                make_document(kvp("$min", make_document(kvp("a", 1))))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.min());
        REQUIRE(*find_opts.min() == make_document(kvp("a", 1)));
    }

    SECTION("$min with other types is rejected") {
        REQUIRE_THROWS_AS(options::rewriter::rewrite_find_modifiers(
                              options::find{}.modifiers_deprecated(make_document(kvp("$min", 1)))),
                          logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $orderby", "[find][option]") {
    instance::current();

    SECTION("$orderby with k_document type is translated") {
        auto find_opts =
            options::rewriter::rewrite_find_modifiers(options::find{}.modifiers_deprecated(
                make_document(kvp("$orderby", make_document(kvp("a", 1))))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.sort());
        REQUIRE(*find_opts.sort() == make_document(kvp("a", 1)));
    }

    SECTION("$orderby with other types is rejected") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$orderby", 1)))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $query", "[find][option]") {
    instance::current();

    SECTION("$query isn't supported") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(options::find{}.modifiers_deprecated(
                make_document(kvp("$query", make_document(kvp("a", 1)))))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $returnKey", "[find][option]") {
    instance::current();

    SECTION("$returnKey with k_bool type is translated") {
        auto find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$returnKey", true))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.return_key());
        REQUIRE(*find_opts.return_key() == true);
    }

    SECTION("$returnKey with other types is rejected") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$returnKey", 1)))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $showDiskLoc", "[find][option]") {
    instance::current();

    SECTION("$showDiskLoc with k_bool type is translated") {
        auto find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$showDiskLoc", true))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.show_record_id());
        REQUIRE(*find_opts.show_record_id() == true);
    }

    SECTION("$showDiskLoc with other types is rejected") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$showDiskLoc", 1)))),
            logic_error);
    }
}

TEST_CASE("options::rewriter::rewrite_find_modifiers() with $snapshot", "[find][option]") {
    instance::current();

    SECTION("$snapshot with k_bool type is translated") {
        auto find_opts = options::rewriter::rewrite_find_modifiers(
            options::find{}.modifiers_deprecated(make_document(kvp("$snapshot", true))));
        REQUIRE(!find_opts.modifiers_deprecated());
        REQUIRE(find_opts.snapshot_deprecated());
        REQUIRE(*find_opts.snapshot_deprecated() == true);
    }

    SECTION("$snapshot with other types is rejected") {
        REQUIRE_THROWS_AS(
            options::rewriter::rewrite_find_modifiers(
                options::find{}.modifiers_deprecated(make_document(kvp("$snapshot", 1)))),
            logic_error);
    }
}

}  // namespace
