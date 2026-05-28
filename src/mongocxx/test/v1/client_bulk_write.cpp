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

#include <mongocxx/v1/client_bulk_write.hh>

//

#include <mongocxx/v1/hint.hpp>
#include <mongocxx/v1/write_concern.hpp>

#include <bsoncxx/test/v1/array/value.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <utility>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

} // namespace

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][options]") {
    client_bulk_write::options source;
    client_bulk_write::options target;

    auto const source_value = bsoncxx::v1::types::value{"source"};
    auto const target_value = bsoncxx::v1::types::value{"target"};

    source.comment(source_value);
    target.comment(target_value);

    REQUIRE(source.comment() == source_value);
    REQUIRE(target.comment() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.comment() == source_value);

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(target.comment() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.comment() == source_value);
        CHECK(copy.comment() == source_value);

        target = copy;

        CHECK(copy.comment() == source_value);
        CHECK(target.comment() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][client_bulk_write][options]") {
    client_bulk_write::options const opts;

    CHECK_FALSE(opts.bypass_document_validation().has_value());
    CHECK_FALSE(opts.comment().has_value());
    CHECK_FALSE(opts.let().has_value());
    CHECK_FALSE(opts.ordered().has_value());
    CHECK_FALSE(opts.verbose_results().has_value());
    CHECK_FALSE(opts.write_concern().has_value());
}

TEST_CASE("bypass_document_validation", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::options{}.bypass_document_validation(v).bypass_document_validation() == v);
}

TEST_CASE("comment", "[mongocxx][v1][client_bulk_write][options]") {
    using T = bsoncxx::v1::types::value;

    auto const v = GENERATE(values({
        T{},
        T{std::int32_t{123}},
        T{std::int64_t{456}},
        T{123.456},
        T{"abc"},
    }));

    CHECK(client_bulk_write::options{}.comment(v).comment() == v);
}

TEST_CASE("let", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(client_bulk_write::options{}.let(v.value()).let() == v.view());
}

TEST_CASE("ordered", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::options{}.ordered(v).ordered() == v);
}

TEST_CASE("verbose_results", "[mongocxx][v1][client_bulk_write][options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::options{}.verbose_results(v).verbose_results() == v);
}

TEST_CASE("write_concern", "[mongocxx][v1][client_bulk_write][options]") {
    using T = v1::write_concern;

    auto const v = GENERATE(values({
        T{},
        T{}.acknowledge_level(T::level::k_majority),
        T{}.tag("abc"),
    }));

    CHECK(client_bulk_write::options{}.write_concern(v).write_concern() == v);
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][result]") {
    auto source = client_bulk_write::result::internal::make();
    auto target = client_bulk_write::result::internal::make();

    auto const source_doc = scoped_bson{R"({"0": {"insertedId": "src"}})"};
    auto const target_doc = scoped_bson{R"({"0": {"insertedId": "tgt"}})"};

    client_bulk_write::result::internal::insert_results(source) = bsoncxx::v1::document::value{source_doc.view()};
    client_bulk_write::result::internal::insert_results(target) = bsoncxx::v1::document::value{target_doc.view()};

    REQUIRE(source.insert_results());
    REQUIRE(target.insert_results());
    REQUIRE(*source.insert_results() == source_doc.view());
    REQUIRE(*target.insert_results() == target_doc.view());

    auto const source_data = source.insert_results()->data();

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.insert_results()->data() == source_data);

        target = std::move(move);

        // move is in an assign-or-move-only state.

        CHECK(target.insert_results()->data() == source_data);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.insert_results()->data() == source_data);
        CHECK(*source.insert_results() == source_doc.view());

        CHECK(copy.insert_results()->data() != source_data);
        CHECK(*copy.insert_results() == source_doc.view());

        auto const copy_data = copy.insert_results()->data();

        target = copy;

        CHECK(copy.insert_results()->data() == copy_data);
        CHECK(*copy.insert_results() == source_doc.view());

        CHECK(target.insert_results()->data() != source_data);
        CHECK(target.insert_results()->data() != copy_data);
        CHECK(*target.insert_results() == source_doc.view());
    }
}

namespace {

struct result_mocks_type {
    using destroy_type = decltype(libmongoc::bulkwriteresult_destroy.create_instance());
    using inserted_type = decltype(libmongoc::bulkwriteresult_insertedcount.create_instance());
    using upserted_type = decltype(libmongoc::bulkwriteresult_upsertedcount.create_instance());
    using matched_type = decltype(libmongoc::bulkwriteresult_matchedcount.create_instance());
    using modified_type = decltype(libmongoc::bulkwriteresult_modifiedcount.create_instance());
    using deleted_type = decltype(libmongoc::bulkwriteresult_deletedcount.create_instance());
    using insert_results_type = decltype(libmongoc::bulkwriteresult_insertresults.create_instance());
    using update_results_type = decltype(libmongoc::bulkwriteresult_updateresults.create_instance());
    using delete_results_type = decltype(libmongoc::bulkwriteresult_deleteresults.create_instance());

    identity_type result_identity;

    mongoc_bulkwriteresult_t* result_id = reinterpret_cast<mongoc_bulkwriteresult_t*>(&result_identity);

    destroy_type destroy = libmongoc::bulkwriteresult_destroy.create_instance();
    inserted_type inserted = libmongoc::bulkwriteresult_insertedcount.create_instance();
    upserted_type upserted = libmongoc::bulkwriteresult_upsertedcount.create_instance();
    matched_type matched = libmongoc::bulkwriteresult_matchedcount.create_instance();
    modified_type modified = libmongoc::bulkwriteresult_modifiedcount.create_instance();
    deleted_type deleted = libmongoc::bulkwriteresult_deletedcount.create_instance();
    insert_results_type insert_results = libmongoc::bulkwriteresult_insertresults.create_instance();
    update_results_type update_results = libmongoc::bulkwriteresult_updateresults.create_instance();
    delete_results_type delete_results = libmongoc::bulkwriteresult_deleteresults.create_instance();

    ~result_mocks_type() = default;
    result_mocks_type(result_mocks_type&& other) noexcept = delete;
    result_mocks_type& operator=(result_mocks_type&& other) noexcept = delete;
    result_mocks_type(result_mocks_type const& other) = delete;
    result_mocks_type& operator=(result_mocks_type const& other) = delete;

    result_mocks_type() {
        destroy
            ->interpose([&](mongoc_bulkwriteresult_t* ptr) {
                if (ptr) {
                    CHECK(ptr == result_id);
                }
            })
            .forever();

        inserted
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
                CHECK(ptr == result_id);
                return 0;
            })
            .forever();

        upserted
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
                CHECK(ptr == result_id);
                return 0;
            })
            .forever();

        matched
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
                CHECK(ptr == result_id);
                return 0;
            })
            .forever();

        modified
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
                CHECK(ptr == result_id);
                return 0;
            })
            .forever();

        deleted
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
                CHECK(ptr == result_id);
                return 0;
            })
            .forever();

        insert_results
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> bson_t const* {
                CHECK(ptr == result_id);
                return nullptr;
            })
            .forever();

        update_results
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> bson_t const* {
                CHECK(ptr == result_id);
                return nullptr;
            })
            .forever();

        delete_results
            ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> bson_t const* {
                CHECK(ptr == result_id);
                return nullptr;
            })
            .forever();
    }

    client_bulk_write::result make() {
        return client_bulk_write::result::internal::make(result_id);
    }
};

} // namespace

TEST_CASE("inserted_count", "[mongocxx][v1][client_bulk_write][result]") {
    using T = std::int64_t;

    auto const v = GENERATE(values({
        T{INT64_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT64_MAX},
    }));

    result_mocks_type mocks;
    mocks.inserted
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
            CHECK(ptr == mocks.result_id);
            return v;
        })
        .forever();

    auto const r = mocks.make();

    CHECK(r.inserted_count() == v);
}

TEST_CASE("upserted_count", "[mongocxx][v1][client_bulk_write][result]") {
    using T = std::int64_t;

    auto const v = GENERATE(values({
        T{INT64_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT64_MAX},
    }));

    result_mocks_type mocks;
    mocks.upserted
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
            CHECK(ptr == mocks.result_id);
            return v;
        })
        .forever();

    auto const r = mocks.make();

    CHECK(r.upserted_count() == v);
}

TEST_CASE("matched_count", "[mongocxx][v1][client_bulk_write][result]") {
    using T = std::int64_t;

    auto const v = GENERATE(values({
        T{INT64_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT64_MAX},
    }));

    result_mocks_type mocks;
    mocks.matched
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
            CHECK(ptr == mocks.result_id);
            return v;
        })
        .forever();

    auto const r = mocks.make();

    CHECK(r.matched_count() == v);
}

TEST_CASE("modified_count", "[mongocxx][v1][client_bulk_write][result]") {
    using T = std::int64_t;

    auto const v = GENERATE(values({
        T{INT64_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT64_MAX},
    }));

    result_mocks_type mocks;
    mocks.modified
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
            CHECK(ptr == mocks.result_id);
            return v;
        })
        .forever();

    auto const r = mocks.make();

    CHECK(r.modified_count() == v);
}

TEST_CASE("deleted_count", "[mongocxx][v1][client_bulk_write][result]") {
    using T = std::int64_t;

    auto const v = GENERATE(values({
        T{INT64_MIN},
        T{-1},
        T{0},
        T{1},
        T{INT64_MAX},
    }));

    result_mocks_type mocks;
    mocks.deleted
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> std::int64_t {
            CHECK(ptr == mocks.result_id);
            return v;
        })
        .forever();

    auto const r = mocks.make();

    CHECK(r.deleted_count() == v);
}

TEST_CASE("insert_results", "[mongocxx][v1][client_bulk_write][result]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"0": {"insertedId": 1}})"},
    }));

    result_mocks_type mocks;
    mocks.insert_results
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.result_id);
            return v.bson();
        })
        .forever();

    auto const r = mocks.make();

    REQUIRE(r.insert_results());
    CHECK(*r.insert_results() == v.view());
}

TEST_CASE("update_results", "[mongocxx][v1][client_bulk_write][result]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"0": {"matchedCount": 1, "modifiedCount": 1}})"},
    }));

    result_mocks_type mocks;
    mocks.update_results
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.result_id);
            return v.bson();
        })
        .forever();

    auto const r = mocks.make();
    REQUIRE(r.update_results());
    CHECK(*r.update_results() == v.view());
}

TEST_CASE("delete_results", "[mongocxx][v1][client_bulk_write][result]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"0": {"deletedCount": 1}})"},
    }));

    result_mocks_type mocks;
    mocks.delete_results
        ->interpose([&](mongoc_bulkwriteresult_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.result_id);
            return v.bson();
        })
        .forever();

    auto const r = mocks.make();

    REQUIRE(r.delete_results());
    CHECK(*r.delete_results() == v.view());
}

TEST_CASE("verbose_results unset", "[mongocxx][v1][client_bulk_write][result]") {
    result_mocks_type mocks;

    auto const r = mocks.make();

    CHECK_FALSE(r.insert_results().has_value());
    CHECK_FALSE(r.update_results().has_value());
    CHECK_FALSE(r.delete_results().has_value());
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][exception]") {
    auto source = client_bulk_write::exception::internal::make();
    auto target = client_bulk_write::exception::internal::make();

    auto const source_doc = scoped_bson{R"({"0": {"code": 1, "message": "src", "details": {}}})"};
    auto const target_doc = scoped_bson{R"({"0": {"code": 2, "message": "tgt", "details": {}}})"};

    client_bulk_write::exception::internal::write_errors(source) = source_doc.value();
    client_bulk_write::exception::internal::write_errors(target) = target_doc.value();

    REQUIRE(source.write_errors() == source_doc.view());
    REQUIRE(target.write_errors() == target_doc.view());

    SECTION("copy") {
        auto copy = source;

        CHECK(source.write_errors() == source_doc.view());
        CHECK(copy.write_errors() == source_doc.view());

        target = copy;

        CHECK(copy.write_errors() == source_doc.view());
        CHECK(target.write_errors() == source_doc.view());
    }
}

namespace {

struct exception_mocks_type {
    using destroy_type = decltype(libmongoc::bulkwriteexception_destroy.create_instance());
    using error_type = decltype(libmongoc::bulkwriteexception_error.create_instance());
    using write_errors_type = decltype(libmongoc::bulkwriteexception_writeerrors.create_instance());
    using write_concern_errors_type = decltype(libmongoc::bulkwriteexception_writeconcernerrors.create_instance());
    using error_reply_type = decltype(libmongoc::bulkwriteexception_errorreply.create_instance());

    identity_type exc_identity;

    mongoc_bulkwriteexception_t* exc_id = reinterpret_cast<mongoc_bulkwriteexception_t*>(&exc_identity);

    destroy_type destroy = libmongoc::bulkwriteexception_destroy.create_instance();
    error_type error = libmongoc::bulkwriteexception_error.create_instance();
    write_errors_type write_errors = libmongoc::bulkwriteexception_writeerrors.create_instance();
    write_concern_errors_type write_concern_errors = libmongoc::bulkwriteexception_writeconcernerrors.create_instance();
    error_reply_type error_reply = libmongoc::bulkwriteexception_errorreply.create_instance();

    ~exception_mocks_type() = default;
    exception_mocks_type(exception_mocks_type&& other) noexcept = delete;
    exception_mocks_type& operator=(exception_mocks_type&& other) noexcept = delete;
    exception_mocks_type(exception_mocks_type const& other) = delete;
    exception_mocks_type& operator=(exception_mocks_type const& other) = delete;

    exception_mocks_type() {
        destroy
            ->interpose([&](mongoc_bulkwriteexception_t* ptr) {
                if (ptr) {
                    CHECK(ptr == exc_id);
                }
            })
            .forever();

        error
            ->interpose([&](mongoc_bulkwriteexception_t const* ptr, bson_error_t* /*err*/) -> bool {
                CHECK(ptr == exc_id);
                return false;
            })
            .forever();

        write_errors
            ->interpose([&](mongoc_bulkwriteexception_t const* ptr) -> bson_t const* {
                CHECK(ptr == exc_id);
                return nullptr;
            })
            .forever();

        write_concern_errors
            ->interpose([&](mongoc_bulkwriteexception_t const* ptr) -> bson_t const* {
                CHECK(ptr == exc_id);
                return nullptr;
            })
            .forever();

        error_reply
            ->interpose([&](mongoc_bulkwriteexception_t const* ptr) -> bson_t const* {
                CHECK(ptr == exc_id);
                return nullptr;
            })
            .forever();
    }

    client_bulk_write::exception make(
        bsoncxx::v1::stdx::optional<client_bulk_write::result> partial_result = bsoncxx::v1::stdx::nullopt) {
        return client_bulk_write::exception::internal::make(exc_id, std::move(partial_result));
    }
};

} // namespace

TEST_CASE("write_errors", "[mongocxx][v1][client_bulk_write][exception]") {
    auto const v = scoped_bson{R"({"0": {"code": 123, "message": "err", "details": {}}})"};

    exception_mocks_type mocks;
    mocks.write_errors
        ->interpose([&](mongoc_bulkwriteexception_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.exc_id);
            return v.bson();
        })
        .forever();

    auto const ex = mocks.make();

    CHECK(ex.write_errors() == v.view());
}

TEST_CASE("write_concern_errors", "[mongocxx][v1][client_bulk_write][exception]") {
    auto const v = scoped_bson{R"([{"code": 123, "message": "err", "details": {}}])"};

    exception_mocks_type mocks;
    mocks.write_concern_errors
        ->interpose([&](mongoc_bulkwriteexception_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.exc_id);
            return v.bson();
        })
        .forever();

    auto const ex = mocks.make();

    CHECK(ex.write_concern_errors() == v.array_view());
}

TEST_CASE("error_reply", "[mongocxx][v1][client_bulk_write][exception]") {
    auto const v = scoped_bson{R"({"ok": 0, "code": 123, "errmsg": "err"})"};

    exception_mocks_type mocks;
    mocks.error_reply
        ->interpose([&](mongoc_bulkwriteexception_t const* ptr) -> bson_t const* {
            CHECK(ptr == mocks.exc_id);
            return v.bson();
        })
        .forever();

    auto const ex = mocks.make();

    CHECK(ex.error_reply() == v.view());
}

TEST_CASE("partial_result", "[mongocxx][v1][client_bulk_write][exception]") {
    auto const partial_result = [] {
        auto r = client_bulk_write::result::internal::make();
        client_bulk_write::result::internal::inserted_count(r) = 2;
        return r;
    }();

    exception_mocks_type mocks;

    auto const ex = mocks.make(partial_result);

    CHECK(ex.partial_result()->inserted_count() == partial_result.inserted_count());
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][update_one_options]") {
    client_bulk_write::update_one_options source;
    client_bulk_write::update_one_options target;

    source.upsert(true);
    target.upsert(false);

    REQUIRE(source.upsert() == true);
    REQUIRE(target.upsert() == false);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.upsert() == true);

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.upsert() == true);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.upsert() == true);
        CHECK(copy.upsert() == true);

        target = copy;

        CHECK(copy.upsert() == true);
        CHECK(target.upsert() == true);
    }
}

TEST_CASE("default", "[mongocxx][v1][client_bulk_write][update_one_options]") {
    client_bulk_write::update_one_options const opts;

    CHECK_FALSE(opts.array_filters().has_value());
    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.hint().has_value());
    CHECK_FALSE(opts.sort().has_value());
    CHECK_FALSE(opts.upsert().has_value());
}

TEST_CASE("array_filters", "[mongocxx][v1][client_bulk_write][update_one_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"([1, 2.0, "three"])"},
    }));

    CHECK(
        client_bulk_write::update_one_options{}
            .array_filters(bsoncxx::v1::array::value{v.array_view()})
            .array_filters() == v.array_view());
}

TEST_CASE("collation", "[mongocxx][v1][client_bulk_write][update_one_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"locale": "en"})"},
    }));

    CHECK(client_bulk_write::update_one_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("hint", "[mongocxx][v1][client_bulk_write][update_one_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(client_bulk_write::update_one_options{}.hint(v).hint() == v);
}

TEST_CASE("sort", "[mongocxx][v1][client_bulk_write][update_one_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(client_bulk_write::update_one_options{}.sort(v.value()).sort() == v.view());
}

TEST_CASE("upsert", "[mongocxx][v1][client_bulk_write][update_one_options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::update_one_options{}.upsert(v).upsert() == v);
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][update_many_options]") {
    client_bulk_write::update_many_options source;
    client_bulk_write::update_many_options target;

    source.upsert(true);
    target.upsert(false);

    REQUIRE(source.upsert() == true);
    REQUIRE(target.upsert() == false);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.upsert() == true);

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.upsert() == true);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.upsert() == true);
        CHECK(copy.upsert() == true);

        target = copy;

        CHECK(copy.upsert() == true);
        CHECK(target.upsert() == true);
    }
}

TEST_CASE("default", "[mongocxx][v1][client_bulk_write][update_many_options]") {
    client_bulk_write::update_many_options const opts;

    CHECK_FALSE(opts.array_filters().has_value());
    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.hint().has_value());
    CHECK_FALSE(opts.upsert().has_value());
}

TEST_CASE("array_filters", "[mongocxx][v1][client_bulk_write][update_many_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"([1, 2.0, "three"])"},
    }));

    CHECK(
        client_bulk_write::update_many_options{}
            .array_filters(bsoncxx::v1::array::value{v.array_view()})
            .array_filters() == v.array_view());
}

TEST_CASE("collation", "[mongocxx][v1][client_bulk_write][update_many_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"locale": "en"})"},
    }));

    CHECK(client_bulk_write::update_many_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("hint", "[mongocxx][v1][client_bulk_write][update_many_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(client_bulk_write::update_many_options{}.hint(v).hint() == v);
}

TEST_CASE("upsert", "[mongocxx][v1][client_bulk_write][update_many_options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::update_many_options{}.upsert(v).upsert() == v);
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][replace_one_options]") {
    client_bulk_write::replace_one_options source;
    client_bulk_write::replace_one_options target;

    source.upsert(true);
    target.upsert(false);

    REQUIRE(source.upsert() == true);
    REQUIRE(target.upsert() == false);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.upsert() == true);

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.upsert() == true);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.upsert() == true);
        CHECK(copy.upsert() == true);

        target = copy;

        CHECK(copy.upsert() == true);
        CHECK(target.upsert() == true);
    }
}

TEST_CASE("default", "[mongocxx][v1][client_bulk_write][replace_one_options]") {
    client_bulk_write::replace_one_options const opts;

    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.hint().has_value());
    CHECK_FALSE(opts.sort().has_value());
    CHECK_FALSE(opts.upsert().has_value());
}

TEST_CASE("collation", "[mongocxx][v1][client_bulk_write][replace_one_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"locale": "en"})"},
    }));

    CHECK(client_bulk_write::replace_one_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("hint", "[mongocxx][v1][client_bulk_write][replace_one_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(client_bulk_write::replace_one_options{}.hint(v).hint() == v);
}

TEST_CASE("sort", "[mongocxx][v1][client_bulk_write][replace_one_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"x": 1})"},
    }));

    CHECK(client_bulk_write::replace_one_options{}.sort(v.value()).sort() == v.view());
}

TEST_CASE("upsert", "[mongocxx][v1][client_bulk_write][replace_one_options]") {
    auto const v = GENERATE(false, true);

    CHECK(client_bulk_write::replace_one_options{}.upsert(v).upsert() == v);
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][delete_one_options]") {
    client_bulk_write::delete_one_options source;
    client_bulk_write::delete_one_options target;

    source.hint(v1::hint{"src"});
    target.hint(v1::hint{"tgt"});

    REQUIRE(source.hint() == v1::hint{"src"});
    REQUIRE(target.hint() == v1::hint{"tgt"});

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.hint() == v1::hint{"src"});

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.hint() == v1::hint{"src"});
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.hint() == v1::hint{"src"});
        CHECK(copy.hint() == v1::hint{"src"});

        target = copy;

        CHECK(copy.hint() == v1::hint{"src"});
        CHECK(target.hint() == v1::hint{"src"});
    }
}

TEST_CASE("default", "[mongocxx][v1][client_bulk_write][delete_one_options]") {
    client_bulk_write::delete_one_options const opts;

    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.hint().has_value());
}

TEST_CASE("collation", "[mongocxx][v1][client_bulk_write][delete_one_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"locale": "en"})"},
    }));

    CHECK(client_bulk_write::delete_one_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("hint", "[mongocxx][v1][client_bulk_write][delete_one_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(client_bulk_write::delete_one_options{}.hint(v).hint() == v);
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write][delete_many_options]") {
    client_bulk_write::delete_many_options source;
    client_bulk_write::delete_many_options target;

    source.hint(v1::hint{"src"});
    target.hint(v1::hint{"tgt"});

    REQUIRE(source.hint() == v1::hint{"src"});
    REQUIRE(target.hint() == v1::hint{"tgt"});

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.hint() == v1::hint{"src"});

        target = std::move(move);

        // move is in an assign-or-destroy-only state.

        CHECK(target.hint() == v1::hint{"src"});
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.hint() == v1::hint{"src"});
        CHECK(copy.hint() == v1::hint{"src"});

        target = copy;

        CHECK(copy.hint() == v1::hint{"src"});
        CHECK(target.hint() == v1::hint{"src"});
    }
}

TEST_CASE("default", "[mongocxx][v1][client_bulk_write][delete_many_options]") {
    client_bulk_write::delete_many_options const opts;

    CHECK_FALSE(opts.collation().has_value());
    CHECK_FALSE(opts.hint().has_value());
}

TEST_CASE("collation", "[mongocxx][v1][client_bulk_write][delete_many_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"locale": "en"})"},
    }));

    CHECK(client_bulk_write::delete_many_options{}.collation(v.value()).collation() == v.view());
}

TEST_CASE("hint", "[mongocxx][v1][client_bulk_write][delete_many_options]") {
    auto const v = GENERATE(values({
        v1::hint{"abc"},
        v1::hint{scoped_bson{R"({"x": 1})"}.value()},
    }));

    CHECK(client_bulk_write::delete_many_options{}.hint(v).hint() == v);
}

TEST_CASE("ownership", "[mongocxx][v1][client_bulk_write]") {
    identity_type id1;
    identity_type id2;

    auto const bw1 = reinterpret_cast<mongoc_bulkwrite_t*>(&id1);
    auto const bw2 = reinterpret_cast<mongoc_bulkwrite_t*>(&id2);

    int destroy_count = 0;

    auto destroy = libmongoc::bulkwrite_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulkwrite_t* bw) -> void {
            if (bw) {
                if (bw != bw1 && bw != bw2) {
                    FAIL("unexpected mongoc_bulkwrite_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    auto source = client_bulk_write::internal::make(bw1);
    auto target = client_bulk_write::internal::make(bw2);

    REQUIRE(client_bulk_write::internal::as_mongoc(source) == bw1);
    REQUIRE(client_bulk_write::internal::as_mongoc(target) == bw2);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-destroy-only state.

            CHECK(client_bulk_write::internal::as_mongoc(move) == bw1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-destroy-only state.

            CHECK(client_bulk_write::internal::as_mongoc(target) == bw1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }
}

TEST_CASE("exceptions", "[mongocxx][v1][client_bulk_write]") {
    identity_type id;
    auto const identity = reinterpret_cast<mongoc_bulkwrite_t*>(&id);

    auto destroy = libmongoc::bulkwrite_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulkwrite_t* bw) -> void {
            if (bw != identity) {
                FAIL("unexpected mongoc_bulkwrite_t");
            }
        })
        .forever();

    auto cbw = client_bulk_write::internal::make(identity);

    bsoncxx::v1::document::value const empty;

    SECTION("append") {
        auto const set_error = [](bson_error_t* error) {
            REQUIRE(error != nullptr);
            bson_set_error(error, MONGOC_ERROR_COMMAND, 123, "append failure");
            error->reserved = 2; // MONGOC_ERROR_CATEGORY
        };

        auto const check_exception = [](v1::exception const& ex) {
            CHECK(ex.code() == v1::source_errc::mongoc);
            CHECK(ex.code().value() == 123);
            CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("append failure"));
        };

        SECTION("insert_one") {
            auto mock = libmongoc::bulkwrite_append_insertone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t*,
                    char const*,
                    bson_t const*,
                    mongoc_bulkwrite_insertoneopts_t const*,
                    bson_error_t* error) -> bool {
                    set_error(error);
                    return false;
                });

            try {
                cbw.append("db.coll", empty, client_bulk_write::insert_one_options{});
                FAIL("should not reach");
            } catch (v1::exception const& ex) {
                check_exception(ex);
            }
        }

        SECTION("update_one") {
            auto mock = libmongoc::bulkwrite_append_updateone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t*,
                    char const*,
                    bson_t const*,
                    bson_t const*,
                    mongoc_bulkwrite_updateoneopts_t const*,
                    bson_error_t* error) -> bool {
                    set_error(error);
                    return false;
                });

            try {
                cbw.append("db.coll", empty, empty, client_bulk_write::update_one_options{});
                FAIL("should not reach");
            } catch (v1::exception const& ex) {
                check_exception(ex);
            }
        }

        SECTION("update_many") {
            auto mock = libmongoc::bulkwrite_append_updatemany.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t*,
                    char const*,
                    bson_t const*,
                    bson_t const*,
                    mongoc_bulkwrite_updatemanyopts_t const*,
                    bson_error_t* error) -> bool {
                    set_error(error);
                    return false;
                });

            try {
                cbw.append("db.coll", empty, empty, client_bulk_write::update_many_options{});
                FAIL("should not reach");
            } catch (v1::exception const& ex) {
                check_exception(ex);
            }
        }

        SECTION("replace_one") {
            auto mock = libmongoc::bulkwrite_append_replaceone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t*,
                    char const*,
                    bson_t const*,
                    bson_t const*,
                    mongoc_bulkwrite_replaceoneopts_t const*,
                    bson_error_t* error) -> bool {
                    set_error(error);
                    return false;
                });

            try {
                cbw.append("db.coll", empty, empty, client_bulk_write::replace_one_options{});
                FAIL("should not reach");
            } catch (v1::exception const& ex) {
                check_exception(ex);
            }
        }

        SECTION("delete_one") {
            auto mock = libmongoc::bulkwrite_append_deleteone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t*,
                    char const*,
                    bson_t const*,
                    mongoc_bulkwrite_deleteoneopts_t const*,
                    bson_error_t* error) -> bool {
                    set_error(error);
                    return false;
                });

            try {
                cbw.append("db.coll", empty, client_bulk_write::delete_one_options{});
                FAIL("should not reach");
            } catch (v1::exception const& ex) {
                check_exception(ex);
            }
        }

        SECTION("delete_many") {
            auto mock = libmongoc::bulkwrite_append_deletemany.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t*,
                    char const*,
                    bson_t const*,
                    mongoc_bulkwrite_deletemanyopts_t const*,
                    bson_error_t* error) -> bool {
                    set_error(error);
                    return false;
                });

            try {
                cbw.append("db.coll", empty, client_bulk_write::delete_many_options{});
                FAIL("should not reach");
            } catch (v1::exception const& ex) {
                check_exception(ex);
            }
        }
    }

    SECTION("execute") {
        identity_type opts_id;
        auto const opts_identity = reinterpret_cast<mongoc_bulkwriteopts_t*>(&opts_id);

        auto opts_new = libmongoc::bulkwriteopts_new.create_instance();
        opts_new->interpose([&]() -> mongoc_bulkwriteopts_t* { return opts_identity; });

        auto opts_destroy = libmongoc::bulkwriteopts_destroy.create_instance();
        opts_destroy->interpose([&](mongoc_bulkwriteopts_t* opts) { CHECK(opts == opts_identity); });

        SECTION("client_bulk_write::exception") {
            exception_mocks_type exc_mocks;

            auto execute = libmongoc::bulkwrite_execute.create_instance();
            execute->interpose([&](mongoc_bulkwrite_t* bw, mongoc_bulkwriteopts_t const*) -> mongoc_bulkwritereturn_t {
                CHECK(bw == identity);
                return {nullptr, exc_mocks.exc_id};
            });

            try {
                cbw.execute(client_bulk_write::options{});
                FAIL("should not reach");
            } catch (client_bulk_write::exception const&) {
            }
        }
    }
}

TEST_CASE("append", "[mongocxx][v1][client_bulk_write]") {
    identity_type id;
    auto const identity = reinterpret_cast<mongoc_bulkwrite_t*>(&id);

    auto destroy = libmongoc::bulkwrite_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulkwrite_t* bw) -> void {
            if (bw != identity) {
                FAIL("unexpected mongoc_bulkwrite_t");
            }
        })
        .forever();

    auto cbw = client_bulk_write::internal::make(identity);

    SECTION("required fields") {
        scoped_bson one{R"({"x": 1})"};
        scoped_bson two{R"({"y": 2})"};

        auto const one_data = one.data();
        auto const two_data = two.data();

        bsoncxx::v1::stdx::string_view const ns = "db.coll";

        SECTION("insert_one") {
            auto mock = libmongoc::bulkwrite_append_insertone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t* bw,
                    char const* ns_arg,
                    bson_t const* doc,
                    mongoc_bulkwrite_insertoneopts_t const* opts,
                    bson_error_t*) -> bool {
                    CHECK(bw == identity);
                    CHECK(bsoncxx::v1::stdx::string_view{ns_arg} == ns);
                    CHECK(scoped_bson_view{doc}.data() == one_data);
                    CHECK(opts == nullptr);
                    return true;
                });

            CHECK_NOTHROW(cbw.append(ns, std::move(one).value(), client_bulk_write::insert_one_options{}));
        }

        SECTION("update_one") {
            auto mock = libmongoc::bulkwrite_append_updateone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t* bw,
                    char const* ns_arg,
                    bson_t const* filter,
                    bson_t const* update,
                    mongoc_bulkwrite_updateoneopts_t const* opts,
                    bson_error_t*) -> bool {
                    CHECK(bw == identity);
                    CHECK(bsoncxx::v1::stdx::string_view{ns_arg} == ns);
                    CHECK(scoped_bson_view{filter}.data() == one_data);
                    CHECK(scoped_bson_view{update}.data() == two_data);
                    CHECK(opts == nullptr);
                    return true;
                });

            CHECK_NOTHROW(cbw.append(
                ns, std::move(one).value(), std::move(two).value(), client_bulk_write::update_one_options{}));
        }

        SECTION("update_many") {
            auto mock = libmongoc::bulkwrite_append_updatemany.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t* bw,
                    char const* ns_arg,
                    bson_t const* filter,
                    bson_t const* update,
                    mongoc_bulkwrite_updatemanyopts_t const* opts,
                    bson_error_t*) -> bool {
                    CHECK(bw == identity);
                    CHECK(bsoncxx::v1::stdx::string_view{ns_arg} == ns);
                    CHECK(scoped_bson_view{filter}.data() == one_data);
                    CHECK(scoped_bson_view{update}.data() == two_data);
                    CHECK(opts == nullptr);
                    return true;
                });

            CHECK_NOTHROW(cbw.append(
                ns, std::move(one).value(), std::move(two).value(), client_bulk_write::update_many_options{}));
        }

        SECTION("replace_one") {
            auto mock = libmongoc::bulkwrite_append_replaceone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t* bw,
                    char const* ns_arg,
                    bson_t const* filter,
                    bson_t const* replacement,
                    mongoc_bulkwrite_replaceoneopts_t const* opts,
                    bson_error_t*) -> bool {
                    CHECK(bw == identity);
                    CHECK(bsoncxx::v1::stdx::string_view{ns_arg} == ns);
                    CHECK(scoped_bson_view{filter}.data() == one_data);
                    CHECK(scoped_bson_view{replacement}.data() == two_data);
                    CHECK(opts == nullptr);
                    return true;
                });

            CHECK_NOTHROW(cbw.append(
                ns, std::move(one).value(), std::move(two).value(), client_bulk_write::replace_one_options{}));
        }

        SECTION("delete_one") {
            auto mock = libmongoc::bulkwrite_append_deleteone.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t* bw,
                    char const* ns_arg,
                    bson_t const* filter,
                    mongoc_bulkwrite_deleteoneopts_t const* opts,
                    bson_error_t*) -> bool {
                    CHECK(bw == identity);
                    CHECK(bsoncxx::v1::stdx::string_view{ns_arg} == ns);
                    CHECK(scoped_bson_view{filter}.data() == one_data);
                    CHECK(opts == nullptr);
                    return true;
                });

            CHECK_NOTHROW(cbw.append(ns, std::move(one).value(), client_bulk_write::delete_one_options{}));
        }

        SECTION("delete_many") {
            auto mock = libmongoc::bulkwrite_append_deletemany.create_instance();
            mock->interpose(
                [&](mongoc_bulkwrite_t* bw,
                    char const* ns_arg,
                    bson_t const* filter,
                    mongoc_bulkwrite_deletemanyopts_t const* opts,
                    bson_error_t*) -> bool {
                    CHECK(bw == identity);
                    CHECK(bsoncxx::v1::stdx::string_view{ns_arg} == ns);
                    CHECK(scoped_bson_view{filter}.data() == one_data);
                    CHECK(opts == nullptr);
                    return true;
                });

            CHECK_NOTHROW(cbw.append(ns, std::move(one).value(), client_bulk_write::delete_many_options{}));
        }
    }
}

TEST_CASE("execute", "[mongocxx][v1][client_bulk_write]") {
    identity_type id;
    auto const identity = reinterpret_cast<mongoc_bulkwrite_t*>(&id);

    auto destroy = libmongoc::bulkwrite_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_bulkwrite_t* bw) -> void {
            if (bw != identity) {
                FAIL("unexpected mongoc_bulkwrite_t");
            }
        })
        .forever();

    auto cbw = client_bulk_write::internal::make(identity);

    identity_type opts_id;
    auto const opts_identity = reinterpret_cast<mongoc_bulkwriteopts_t*>(&opts_id);

    auto opts_new = libmongoc::bulkwriteopts_new.create_instance();
    opts_new->interpose([&]() -> mongoc_bulkwriteopts_t* { return opts_identity; });

    auto opts_destroy = libmongoc::bulkwriteopts_destroy.create_instance();
    opts_destroy->interpose([&](mongoc_bulkwriteopts_t* opts) { CHECK(opts == opts_identity); });

    auto execute = libmongoc::bulkwrite_execute.create_instance();

    SECTION("acknowledged") {
        result_mocks_type res_mocks;

        execute->interpose([&](mongoc_bulkwrite_t* bw, mongoc_bulkwriteopts_t const* opts) -> mongoc_bulkwritereturn_t {
            CHECK(bw == identity);
            CHECK(opts == opts_identity);
            return {res_mocks.result_id, nullptr};
        });

        auto const ret = cbw.execute(client_bulk_write::options{});

        REQUIRE(ret.has_value());
    }

    SECTION("unacknowledged") {
        execute->interpose([&](mongoc_bulkwrite_t* bw, mongoc_bulkwriteopts_t const* opts) -> mongoc_bulkwritereturn_t {
            CHECK(bw == identity);
            CHECK(opts == opts_identity);
            return {nullptr, nullptr};
        });

        CHECK_FALSE(cbw.execute(client_bulk_write::options{}).has_value());
    }
}

} // namespace v1
} // namespace mongocxx
