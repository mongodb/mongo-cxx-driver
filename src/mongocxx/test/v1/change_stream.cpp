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

#include <mongocxx/v1/change_stream.hpp>

//

#include <mongocxx/v1/change_stream.hh>
#include <mongocxx/v1/server_error.hh>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/types/value.hh>

#include <chrono>
#include <utility>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mock.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

#include <bsoncxx/test/system_error.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("exceptions", "[mongocxx][v1][change_stream]") {
    struct test_data_type {
        scoped_bson doc;
        scoped_bson error_doc;
        int next_count = 0;
        int error_document_count = 0;
    } data;

    auto change_stream_destroy = libmongoc::change_stream_destroy.create_instance();
    change_stream_destroy
        ->interpose([&](mongoc_change_stream_t* p) -> void { CHECK(reinterpret_cast<test_data_type*>(p) == &data); })
        .forever();

    auto change_stream_next = libmongoc::change_stream_next.create_instance();
    change_stream_next
        ->interpose([&](mongoc_change_stream_t* stream, bson_t const** bson) -> bool {
            (void)bson;
            CHECK(reinterpret_cast<test_data_type*>(stream) == &data);
            ++data.next_count;
            return false;
        })
        .times(1);

    auto change_stream_error_document = libmongoc::change_stream_error_document.create_instance();
    change_stream_error_document
        ->interpose([&](mongoc_change_stream_t const* stream, bson_error_t* err, bson_t const** bson) -> bool {
            (void)(stream);
            (void)err;
            (void)bson;
            return false;
        })
        .forever();

    auto stream = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));

    SECTION("change_stream_error_document") {
        SECTION("v1::exception") {
            change_stream_error_document->interpose(
                [&](mongoc_change_stream_t const* stream, bson_error_t* err, bson_t const** bson) -> bool {
                    CHECK(reinterpret_cast<test_data_type const*>(stream) == &data);
                    REQUIRE(err);
                    REQUIRE(bson);

                    ++data.error_document_count;

                    bson_set_error(err, 0, 123, "advance failure");
                    err->reserved = 2u; // MONGOC_ERROR_CATEGORY

                    data.error_doc =
                        scoped_bson{BCON_NEW("error_document_count", BCON_INT32(data.error_document_count))};
                    *bson = data.error_doc.bson();

                    return true;
                });

            try {
                (void)stream.begin();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == 123);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("advance failure"));
            }

            CHECK_NOTHROW(stream.begin());

            CHECK(change_stream::internal::is_dead(stream));
            CHECK(change_stream::internal::doc(stream).empty());
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
        }

        SECTION("v1::server_error") {
            change_stream_error_document
                ->interpose([&](mongoc_change_stream_t const* stream, bson_error_t* err, bson_t const** bson) -> bool {
                    CHECK(reinterpret_cast<test_data_type const*>(stream) == &data);
                    REQUIRE(err);
                    REQUIRE(bson);

                    ++data.error_document_count;

                    bson_set_error(err, 0, 456, "advance failure");
                    err->reserved = 2u; // MONGOC_ERROR_CATEGORY

                    data.error_doc = scoped_bson{BCON_NEW(
                        "code", BCON_INT32(123), "error_document_count", BCON_INT32(data.error_document_count))};
                    *bson = data.error_doc.bson();

                    return true;
                })
                .times(1);

            try {
                (void)stream.begin();
                FAIL("should not reach this point");
            } catch (v1::server_error const& ex) {
                CHECK(ex.code() == v1::source_errc::server);
                CHECK(ex.code().value() == 123);
                CHECK(ex.client_code() == v1::source_errc::mongoc);
                CHECK(ex.client_code().value() == 456);
                CHECK(ex.raw()["error_document_count"].get_int32().value == 1);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("advance failure"));
            }

            CHECK_NOTHROW(stream.begin());

            CHECK(change_stream::internal::is_dead(stream));
            CHECK(change_stream::internal::doc(stream).empty());
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][change_stream]") {
    struct test_data_type {
        int destroyed_count = 0;
    } source_data, target_data;

    auto change_stream_destroy = libmongoc::change_stream_destroy.create_instance();
    change_stream_destroy
        ->interpose([&](mongoc_change_stream_t* stream) -> void {
            auto const data = reinterpret_cast<test_data_type*>(stream);

            if (data == &source_data || data == &target_data) {
                data->destroyed_count += 1;
            } else {
                CHECK(static_cast<void const*>(&source_data));
                CHECK(static_cast<void const*>(&target_data));
                CHECK(static_cast<void const*>(data));
                FAIL_CHECK("should not reach this point");
            }
        })
        .forever();

    auto source = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&source_data));
    bsoncxx::v1::stdx::optional<change_stream> target =
        change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&target_data));

    SECTION("move") {
        auto move = std::move(source);

        CHECK(source_data.destroyed_count == 0);
        CHECK(target_data.destroyed_count == 0);

        target = std::move(move);

        CHECK(source_data.destroyed_count == 0);
        CHECK(target_data.destroyed_count == 1);

        target.reset();

        CHECK(source_data.destroyed_count == 1);
        CHECK(target_data.destroyed_count == 1);
    }
}

TEST_CASE("end", "[mongocxx][v1][change_stream]") {
    struct test_data_type {
        int next_count = 0;
    } data;

    auto change_stream_destroy = libmongoc::change_stream_destroy.create_instance();
    change_stream_destroy
        ->interpose([&](mongoc_change_stream_t* p) -> void { CHECK(reinterpret_cast<test_data_type*>(p) == &data); })
        .forever();

    auto change_stream_next = libmongoc::change_stream_next.create_instance();
    change_stream_next
        ->interpose([&](mongoc_change_stream_t* stream, bson_t const** bson) -> bool {
            (void)stream;
            (void)bson;
            FAIL("should not reach this point");
            return false;
        })
        .forever();

    auto const s1 = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));
    auto const s2 = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));

    CHECK(s1.end() == s1.end());
    CHECK(s1.end() == s2.end());
    CHECK(s1.end() == change_stream::iterator{});
    CHECK(change_stream::iterator{} == change_stream::iterator{});

    CHECK((++s1.end()) == s1.end());
    CHECK((s1.end()++) == s1.end());

    CHECK((*s1.end()).empty()); // OK.
    // CHECK(s1.end()->empty()); // Undefined behavior.

    CHECK(data.next_count == 0);
}

TEST_CASE("begin", "[mongocxx][v1][change_stream]") {
    struct test_data_type {
        scoped_bson doc;
        scoped_bson error_doc;
        int next_count = 0;
        int error_document_count = 0;
    } data;

    static bsoncxx::v1::document::view const empty;

    auto change_stream_destroy = libmongoc::change_stream_destroy.create_instance();
    change_stream_destroy
        ->interpose([&](mongoc_change_stream_t* p) -> void { CHECK(reinterpret_cast<test_data_type*>(p) == &data); })
        .forever();

    auto change_stream_next = libmongoc::change_stream_next.create_instance();
    change_stream_next
        ->interpose([&](mongoc_change_stream_t* stream, bson_t const** bson) -> bool {
            (void)bson;
            CHECK(reinterpret_cast<test_data_type*>(stream) == &data);
            ++data.next_count;
            data.doc = scoped_bson{BCON_NEW("next", BCON_INT32(data.next_count))};
            *bson = data.doc.bson();
            return true;
        })
        .forever();

    auto change_stream_error_document = libmongoc::change_stream_error_document.create_instance();
    change_stream_error_document
        ->interpose([&](mongoc_change_stream_t const* stream, bson_error_t* err, bson_t const** bson) -> bool {
            (void)(stream);
            (void)err;
            (void)bson;
            return false;
        })
        .forever();

    SECTION("iteration") {
        auto stream = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));

        CHECK(change_stream::internal::can_get_more(stream));
        CHECK(data.next_count == 0);

        auto iter = stream.begin();
        REQUIRE(iter != stream.end());
        CHECK(change_stream::internal::has_doc(stream));
        CHECK(change_stream::internal::doc(stream) == *iter);
        CHECK(iter->find("next") != iter->end());
        CHECK((*iter)["next"].get_int32().value == 1);
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        CHECK_NOTHROW(++iter);
        REQUIRE(iter != stream.end());
        CHECK(change_stream::internal::has_doc(stream));
        CHECK(change_stream::internal::doc(stream) == *iter);
        CHECK(iter->find("next") != iter->end());
        CHECK((*iter)["next"].get_int32().value == 2);
        CHECK(data.next_count == 2);
        CHECK(data.error_document_count == 0);

        CHECK_NOTHROW(iter++);
        REQUIRE(iter != stream.end());
        CHECK(change_stream::internal::has_doc(stream));
        CHECK(change_stream::internal::doc(stream) == *iter);
        CHECK(iter->find("next") != iter->end());
        CHECK((*iter)["next"].get_int32().value == 3);
        CHECK(data.next_count == 3);
        CHECK(data.error_document_count == 0);
    }

    SECTION("equality") {
        auto s1 = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));
        auto s2 = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));

        CHECK(data.next_count == 0);
        CHECK(data.error_document_count == 0);

        auto iter1 = s1.begin();
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        auto iter2 = s2.begin();
        CHECK(data.next_count == 2);
        CHECK(data.error_document_count == 0);

        CHECK(iter1 == iter1);
        CHECK(iter2 == iter2);

        CHECK(iter1 != s1.end());
        CHECK(iter2 != s2.end());

        CHECK(iter1 != iter2);
        CHECK(iter1 != s2.end());
        CHECK(iter2 != s1.end());

        CHECK(data.next_count == 2);
        CHECK(data.error_document_count == 0);
    }

    SECTION("consecutive") {
        auto stream = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));

        CHECK(data.next_count == 0);
        CHECK(data.error_document_count == 0);

        auto const i1 = stream.begin();
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        auto const i2 = stream.begin();
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        CHECK(i1 == i2);
        CHECK(i1 != stream.end());
        CHECK(i2 != stream.end());

        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);
    }

    SECTION("resumable") {
        auto stream = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));

        change_stream_next
            ->interpose([&](mongoc_change_stream_t* stream, bson_t const** bson) -> bool {
                (void)bson;
                CHECK(reinterpret_cast<test_data_type*>(stream) == &data);
                ++data.next_count;
                return false;
            })
            .times(2);

        change_stream_error_document
            ->interpose([&](mongoc_change_stream_t const* stream, bson_error_t* err, bson_t const** bson) -> bool {
                CHECK(reinterpret_cast<test_data_type const*>(stream) == &data);
                REQUIRE(err);
                REQUIRE(bson);
                ++data.error_document_count;
                return false;
            })
            .times(2);

        CHECK(change_stream::internal::can_get_more(stream));
        CHECK(data.next_count == 0);
        CHECK(data.error_document_count == 0);

        {
            auto const iter = stream.begin();

            CHECK(change_stream::internal::can_get_more(stream));
            CHECK(change_stream::internal::doc(stream) == empty);
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
            CHECK(iter == stream.end());
            CHECK(iter->empty());
        }

        {
            auto const iter = stream.begin();

            CHECK(change_stream::internal::can_get_more(stream));
            CHECK(change_stream::internal::doc(stream) == empty);
            CHECK(data.next_count == 2);
            CHECK(data.error_document_count == 2);
            CHECK(iter == stream.end());
            CHECK(iter->empty());
        }

        {
            auto const iter = stream.begin();

            CHECK(change_stream::internal::has_doc(stream));
            CHECK(change_stream::internal::doc(stream) == data.doc.view());
            CHECK(data.next_count == 3);
            CHECK(data.error_document_count == 2);
            CHECK(iter != stream.end());
            CHECK(*iter == data.doc.view());
        }
    }
}

TEST_CASE("get_resume_token", "[mongocxx][v1][change_stream]") {
    struct test_data_type {
        scoped_bson doc;
        int next_count = 0;
        int get_resume_token_count = 0;
    } data;

    auto change_stream_destroy = libmongoc::change_stream_destroy.create_instance();
    change_stream_destroy
        ->interpose([&](mongoc_change_stream_t* p) -> void { CHECK(reinterpret_cast<test_data_type*>(p) == &data); })
        .forever();

    auto change_stream_get_resume_token = libmongoc::change_stream_get_resume_token.create_instance();
    change_stream_get_resume_token
        ->interpose([&](mongoc_change_stream_t* stream) -> bson_t const* {
            CHECK(reinterpret_cast<test_data_type*>(stream) == &data);
            ++data.get_resume_token_count;
            return nullptr;
        })
        .forever();

    auto const stream = change_stream::internal::make(reinterpret_cast<mongoc_change_stream_t*>(&data));
    CHECK(data.get_resume_token_count == 0);

    SECTION("empty") {
        CHECK_FALSE(stream.get_resume_token().has_value());
        CHECK(data.get_resume_token_count == 1);
    }

    SECTION("value") {
        change_stream_get_resume_token->interpose([&](mongoc_change_stream_t* stream) -> bson_t const* {
            CHECK(reinterpret_cast<test_data_type*>(stream) == &data);
            ++data.get_resume_token_count;
            data.doc = scoped_bson{BCON_NEW("get_resume_token", BCON_INT32(data.get_resume_token_count))};
            return data.doc.bson();
        });
        CHECK(stream.get_resume_token() == scoped_bson{R"({"get_resume_token": 1})"}.view());
        CHECK(data.get_resume_token_count == 1);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][change_stream][options]") {
    change_stream::options source;
    change_stream::options target;

    source.resume_after(scoped_bson{R"({"source": 123})"}.value());
    target.resume_after(scoped_bson{R"({"target": 456})"}.value());

    REQUIRE(source.resume_after().has_value());
    REQUIRE(target.resume_after().has_value());

    bsoncxx::v1::document::value const doc{*source.resume_after()};
    auto const data = source.resume_after()->data();

    SECTION("move") {
        auto move = std::move(source);

        // `source` is in an assign-or-destroy-only state.

        REQUIRE(move.resume_after() == doc);
        CHECK(move.resume_after()->data() == data);

        target = std::move(move);

        // `move` is in an assign-or-destroy-only state.

        REQUIRE(target.resume_after() == doc);
        CHECK(target.resume_after()->data() == data);
    }

    SECTION("copy") {
        auto copy = source;

        REQUIRE(source.resume_after() == doc);
        CHECK(source.resume_after()->data() == data);

        REQUIRE(copy.resume_after() == doc);
        CHECK(copy.resume_after()->data() != data);
        CHECK(copy.resume_after() == source.resume_after());

        auto const copy_data = copy.resume_after()->data();

        target = copy;

        REQUIRE(copy.resume_after() == doc);
        CHECK(copy.resume_after()->data() == copy_data);

        REQUIRE(target.resume_after() == doc);
        CHECK(target.resume_after()->data() != copy_data);
        CHECK(target.resume_after() == copy.resume_after());
    }
}

TEST_CASE("fields", "[mongocxx][v1][change_stream][options]") {
    change_stream::options opts;

    SECTION("default") {
        CHECK_FALSE(opts.batch_size().has_value());
        CHECK_FALSE(opts.collation().has_value());
        CHECK_FALSE(opts.comment().has_value());
        CHECK_FALSE(opts.full_document().has_value());
        CHECK_FALSE(opts.full_document_before_change().has_value());
        CHECK_FALSE(opts.max_await_time().has_value());
        CHECK_FALSE(opts.resume_after().has_value());
        CHECK_FALSE(opts.start_after().has_value());
        CHECK_FALSE(opts.start_at_operation_time().has_value());
    }

    SECTION("set") {
#undef SET_AND_CHECK
#define SET_AND_CHECK(_opts, _name, _v)           \
    if (1) {                                      \
        CHECK((_opts)._name(_v)._name() == (_v)); \
    } else                                        \
        ((void)0)

        SET_AND_CHECK(opts, batch_size, 123);
        SET_AND_CHECK(opts, collation, scoped_bson{R"({"collation": 1})"}.value());
        SET_AND_CHECK(opts, comment, bsoncxx::v1::types::value{"comment"});
        SET_AND_CHECK(opts, full_document, "full_document");
        SET_AND_CHECK(opts, full_document_before_change, "full_document_before_change");
        SET_AND_CHECK(opts, max_await_time, std::chrono::milliseconds{123});
        SET_AND_CHECK(opts, resume_after, scoped_bson{R"({"resume_after": 1})"}.value());
        SET_AND_CHECK(opts, start_after, scoped_bson{R"({"start_after": 1})"}.value());
        SET_AND_CHECK(opts, start_at_operation_time, (bsoncxx::v1::types::b_timestamp{123, 456}));

#undef SET_AND_CHECK
    }
}

} // namespace v1
} // namespace mongocxx
