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

#include <chrono>

#include "catch.hpp"
#include "helpers.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/document/view.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>

using namespace bsoncxx::builder::stream;
using namespace mongocxx;

TEST_CASE("find", "[find][option]") {
    instance::current();

    options::find find_opts{};

    auto collation = document{} << "locale"
                                << "en_US" << finalize;
    auto max = document{} << "a" << 6 << finalize;
    auto min = document{} << "a" << 3 << finalize;
    auto modifiers = document{} << "$comment"
                                << "comment" << finalize;
    auto projection = document{} << "_id" << false << finalize;
    auto sort = document{} << "x" << -1 << finalize;

    CHECK_OPTIONAL_ARGUMENT(find_opts, allow_partial_results, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, batch_size, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, comment, "comment");
    CHECK_OPTIONAL_ARGUMENT(find_opts, cursor_type, cursor::type::k_non_tailable);
    CHECK_OPTIONAL_ARGUMENT(find_opts, limit, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, max, max.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_await_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_scan, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, max_time, std::chrono::milliseconds{300});
    CHECK_OPTIONAL_ARGUMENT(find_opts, min, min.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, modifiers, modifiers.view());
    CHECK_OPTIONAL_ARGUMENT(find_opts, no_cursor_timeout, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, projection, projection.view());
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(find_opts, read_preference, read_preference{});
    CHECK_OPTIONAL_ARGUMENT(find_opts, return_key, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, show_record_id, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, skip, 3);
    CHECK_OPTIONAL_ARGUMENT(find_opts, snapshot, true);
    CHECK_OPTIONAL_ARGUMENT(find_opts, sort, sort.view());
}

TEST_CASE("options::find::convert_all_modifiers() with $comment", "[find][option]") {
    instance::current();

    SECTION("$comment with k_utf8 type is translated") {
        auto find_opts = options::find{}
                             .modifiers(document{} << "$comment"
                                                   << "test" << finalize)
                             .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.comment());
        REQUIRE(*find_opts.comment() == stdx::string_view("test"));
    }

    SECTION("$comment with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$comment" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $explain", "[find][option]") {
    instance::current();

    SECTION("$explain isn't supported") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$explain" << true << finalize)
                              .convert_all_modifiers());
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $hint", "[find][option]") {
    instance::current();
    options::find find_opts;

    SECTION("$hint with k_utf8 type is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$hint"
                                              << "index" << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.hint());
        REQUIRE(*find_opts.hint() == "index");
    }

    SECTION("$hint with k_document is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$hint" << open_document << "a" << 1
                                              << close_document << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.hint());
        REQUIRE(*find_opts.hint() == document{} << "a" << 1 << finalize);
    }

    SECTION("$hint with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$hint" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $max", "[find][option]") {
    instance::current();

    SECTION("$max with k_document type is translated") {
        auto find_opts = options::find{}
                             .modifiers(document{} << "$max" << open_document << "a" << 1
                                                   << close_document << finalize)
                             .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.max());
        REQUIRE(*find_opts.max() == document{} << "a" << 1 << finalize);
    }

    SECTION("$max with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$max" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $maxScan", "[find][option]") {
    instance::current();
    options::find find_opts;

    SECTION("$maxScan with k_int32 type is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$maxScan" << 1 << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.max_scan());
        REQUIRE(*find_opts.max_scan() == 1);
    }

    SECTION("$maxScan with k_int64 type is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$maxScan" << std::int64_t{1} << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.max_scan());
        REQUIRE(*find_opts.max_scan() == 1);
    }

    SECTION("$maxScan with k_double type is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$maxScan" << 1.0 << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.max_scan());
        REQUIRE(*find_opts.max_scan() == 1);
    }

    SECTION("$maxScan with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$maxScan"
                                                    << "foo" << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $maxTimeMS", "[find][option]") {
    instance::current();
    options::find find_opts;

    SECTION("$maxTimeMS with k_int32 type is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$maxTimeMS" << 1 << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.max_time());
        REQUIRE(find_opts.max_time()->count() == 1);
    }

    SECTION("$maxTimeMS with k_int64 type is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$maxTimeMS" << std::int64_t{1} << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.max_time());
        REQUIRE(find_opts.max_time()->count() == 1);
    }

    SECTION("$maxTimeMS with k_double type is translated") {
        find_opts = options::find{}
                        .modifiers(document{} << "$maxTimeMS" << 1.0 << finalize)
                        .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.max_time());
        REQUIRE(find_opts.max_time()->count() == 1);
    }

    SECTION("$maxTimeMS with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$maxTimeMS"
                                                    << "foo" << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $min", "[find][option]") {
    instance::current();

    SECTION("$min with k_document type is translated") {
        auto find_opts = options::find{}
                             .modifiers(document{} << "$min" << open_document << "a" << 1
                                                   << close_document << finalize)
                             .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.min());
        REQUIRE(*find_opts.min() == document{} << "a" << 1 << finalize);
    }

    SECTION("$min with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$min" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $orderby", "[find][option]") {
    instance::current();

    SECTION("$orderby with k_document type is translated") {
        auto find_opts = options::find{}
                             .modifiers(document{} << "$orderby" << open_document << "a" << 1
                                                   << close_document << finalize)
                             .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.sort());
        REQUIRE(*find_opts.sort() == document{} << "a" << 1 << finalize);
    }

    SECTION("$orderby with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$orderby" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $query", "[find][option]") {
    instance::current();

    SECTION("$query isn't supported") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$query" << open_document << "a" << 1
                                                    << close_document << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $returnKey", "[find][option]") {
    instance::current();

    SECTION("$returnKey with k_bool type is translated") {
        auto find_opts = options::find{}
                             .modifiers(document{} << "$returnKey" << true << finalize)
                             .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.return_key());
        REQUIRE(*find_opts.return_key() == true);
    }

    SECTION("$returnKey with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$returnKey" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $showDiskLoc", "[find][option]") {
    instance::current();

    SECTION("$showDiskLoc with k_bool type is translated") {
        auto find_opts = options::find{}
                             .modifiers(document{} << "$showDiskLoc" << true << finalize)
                             .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.show_record_id());
        REQUIRE(*find_opts.show_record_id() == true);
    }

    SECTION("$showDiskLoc with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$showDiskLoc" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}

TEST_CASE("options::find::convert_all_modifiers() with $snapshot", "[find][option]") {
    instance::current();

    SECTION("$snapshot with k_bool type is translated") {
        auto find_opts = options::find{}
                             .modifiers(document{} << "$snapshot" << true << finalize)
                             .convert_all_modifiers();
        REQUIRE(!find_opts.modifiers());
        REQUIRE(find_opts.snapshot());
        REQUIRE(*find_opts.snapshot() == true);
    }

    SECTION("$snapshot with other types is rejected") {
        REQUIRE_THROWS_AS(options::find{}
                              .modifiers(document{} << "$snapshot" << 1 << finalize)
                              .convert_all_modifiers(),
                          logic_error);
    }
}
