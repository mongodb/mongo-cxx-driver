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

#include <mongocxx/v1/cursor.hpp>

//

#include <mongocxx/v1/cursor.hh>
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
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("exceptions", "[mongocxx][v1][cursor]") {
    struct test_data_type {
        scoped_bson doc;
        scoped_bson error_doc;
        int next_count = 0;
        int error_document_count = 0;
    } data;

    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    cursor_destroy->interpose([&](mongoc_cursor_t* p) -> void { CHECK(reinterpret_cast<test_data_type*>(p) == &data); })
        .forever();

    auto cursor_next = libmongoc::cursor_next.create_instance();
    cursor_next
        ->interpose([&](mongoc_cursor_t* cursor, bson_t const** bson) -> bool {
            (void)bson;
            CHECK(reinterpret_cast<test_data_type*>(cursor) == &data);
            ++data.next_count;
            return false;
        })
        .times(1);

    auto cursor_error_document = libmongoc::cursor_error_document.create_instance();
    cursor_error_document
        ->interpose([&](mongoc_cursor_t const* cursor, bson_error_t* err, bson_t const** bson) -> bool {
            (void)(cursor);
            (void)err;
            (void)bson;
            return false;
        })
        .forever();

    auto cursor = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));

    SECTION("cursor_error_document") {
        SECTION("v1::exception") {
            cursor_error_document->interpose(
                [&](mongoc_cursor_t const* cursor, bson_error_t* err, bson_t const** bson) -> bool {
                    CHECK(reinterpret_cast<test_data_type const*>(cursor) == &data);
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
                (void)cursor.begin();
                FAIL("should not reach this point");
            } catch (v1::exception const& ex) {
                CHECK(ex.code() == v1::source_errc::mongoc);
                CHECK(ex.code().value() == 123);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("advance failure"));
            }

            CHECK_NOTHROW(cursor.begin());

            CHECK(cursor::internal::is_dead(cursor));
            CHECK(cursor::internal::doc(cursor).empty());
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
        }

        SECTION("v1::server_error") {
            cursor_error_document
                ->interpose([&](mongoc_cursor_t const* cursor, bson_error_t* err, bson_t const** bson) -> bool {
                    CHECK(reinterpret_cast<test_data_type const*>(cursor) == &data);
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
                (void)cursor.begin();
                FAIL("should not reach this point");
            } catch (v1::server_error const& ex) {
                CHECK(ex.code() == v1::source_errc::server);
                CHECK(ex.code().value() == 123);
                CHECK(ex.client_code() == v1::source_errc::mongoc);
                CHECK(ex.client_code().value() == 456);
                CHECK(ex.raw()["error_document_count"].get_int32().value == 1);
                CHECK_THAT(ex.what(), Catch::Matchers::ContainsSubstring("advance failure"));
            }

            CHECK_NOTHROW(cursor.begin());

            CHECK(cursor::internal::is_dead(cursor));
            CHECK(cursor::internal::doc(cursor).empty());
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
        }
    }
}

TEST_CASE("ownership", "[mongocxx][v1][cursor]") {
    struct test_data_type {
        int destroyed_count = 0;
    } source_data, target_data;

    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    cursor_destroy
        ->interpose([&](mongoc_cursor_t* cursor) -> void {
            auto const data = reinterpret_cast<test_data_type*>(cursor);

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

    auto source = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&source_data));
    bsoncxx::v1::stdx::optional<cursor> target =
        cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&target_data));

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

TEST_CASE("end", "[mongocxx][v1][cursor]") {
    struct test_data_type {
        int next_count = 0;
    } data;

    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    cursor_destroy->interpose([&](mongoc_cursor_t* p) -> void { CHECK(reinterpret_cast<test_data_type*>(p) == &data); })
        .forever();

    auto cursor_next = libmongoc::cursor_next.create_instance();
    cursor_next
        ->interpose([&](mongoc_cursor_t* cursor, bson_t const** bson) -> bool {
            (void)cursor;
            (void)bson;
            FAIL("should not reach this point");
            return false;
        })
        .forever();

    auto const c1 = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));
    auto const c2 = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));

    CHECK(c1.end() == c1.end());
    CHECK(c1.end() == c2.end());
    CHECK(c1.end() == cursor::iterator{});
    CHECK(cursor::iterator{} == cursor::iterator{});

    CHECK((++c1.end()) == c1.end());
    CHECK((c1.end()++) == c1.end());

    CHECK((*c1.end()).empty()); // OK.
    // CHECK(c1.end()->empty()); // Undefined behavior.

    CHECK(data.next_count == 0);
}

TEST_CASE("begin", "[mongocxx][v1][cursor]") {
    struct test_data_type {
        scoped_bson doc;
        scoped_bson error_doc;
        int next_count = 0;
        int error_document_count = 0;
    } data;

    static bsoncxx::v1::document::view const empty;

    auto cursor_destroy = libmongoc::cursor_destroy.create_instance();
    cursor_destroy->interpose([&](mongoc_cursor_t* p) -> void { CHECK(reinterpret_cast<test_data_type*>(p) == &data); })
        .forever();

    auto cursor_next = libmongoc::cursor_next.create_instance();
    cursor_next
        ->interpose([&](mongoc_cursor_t* cursor, bson_t const** bson) -> bool {
            (void)bson;
            CHECK(reinterpret_cast<test_data_type*>(cursor) == &data);
            ++data.next_count;
            data.doc = scoped_bson{BCON_NEW("next", BCON_INT32(data.next_count))};
            *bson = data.doc.bson();
            return true;
        })
        .forever();

    auto cursor_error_document = libmongoc::cursor_error_document.create_instance();
    cursor_error_document
        ->interpose([&](mongoc_cursor_t const* cursor, bson_error_t* err, bson_t const** bson) -> bool {
            (void)(cursor);
            (void)err;
            (void)bson;
            return false;
        })
        .forever();

    SECTION("iteration") {
        auto cursor = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));

        CHECK(cursor::internal::can_get_more(cursor));
        CHECK(data.next_count == 0);

        auto iter = cursor.begin();
        REQUIRE(iter != cursor.end());
        CHECK(cursor::internal::has_doc(cursor));
        CHECK(cursor::internal::doc(cursor) == *iter);
        CHECK(iter->find("next") != iter->end());
        CHECK((*iter)["next"].get_int32().value == 1);
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        CHECK_NOTHROW(++iter);
        REQUIRE(iter != cursor.end());
        CHECK(cursor::internal::has_doc(cursor));
        CHECK(cursor::internal::doc(cursor) == *iter);
        CHECK(iter->find("next") != iter->end());
        CHECK((*iter)["next"].get_int32().value == 2);
        CHECK(data.next_count == 2);
        CHECK(data.error_document_count == 0);

        CHECK_NOTHROW(iter++);
        REQUIRE(iter != cursor.end());
        CHECK(cursor::internal::has_doc(cursor));
        CHECK(cursor::internal::doc(cursor) == *iter);
        CHECK(iter->find("next") != iter->end());
        CHECK((*iter)["next"].get_int32().value == 3);
        CHECK(data.next_count == 3);
        CHECK(data.error_document_count == 0);
    }

    SECTION("equality") {
        auto c1 = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));
        auto c2 = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));

        CHECK(data.next_count == 0);
        CHECK(data.error_document_count == 0);

        auto iter1 = c1.begin();
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        auto iter2 = c2.begin();
        CHECK(data.next_count == 2);
        CHECK(data.error_document_count == 0);

        CHECK(iter1 == iter1);
        CHECK(iter2 == iter2);

        CHECK(iter1 != c1.end());
        CHECK(iter2 != c2.end());

        CHECK(iter1 != iter2);
        CHECK(iter1 != c2.end());
        CHECK(iter2 != c1.end());

        CHECK(data.next_count == 2);
        CHECK(data.error_document_count == 0);
    }

    SECTION("consecutive") {
        auto cursor = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));

        CHECK(data.next_count == 0);
        CHECK(data.error_document_count == 0);

        auto const i1 = cursor.begin();
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        auto const i2 = cursor.begin();
        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);

        CHECK(i1 == i2);
        CHECK(i1 != cursor.end());
        CHECK(i2 != cursor.end());

        CHECK(data.next_count == 1);
        CHECK(data.error_document_count == 0);
    }

    SECTION("non-tailable") {
        auto cursor = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data));

        cursor_next
            ->interpose([&](mongoc_cursor_t* cursor, bson_t const** bson) -> bool {
                (void)bson;
                CHECK(reinterpret_cast<test_data_type*>(cursor) == &data);
                ++data.next_count;
                return false;
            })
            .times(1);

        cursor_error_document
            ->interpose([&](mongoc_cursor_t const* cursor, bson_error_t* err, bson_t const** bson) -> bool {
                CHECK(reinterpret_cast<test_data_type const*>(cursor) == &data);
                REQUIRE(err);
                REQUIRE(bson);
                ++data.error_document_count;
                return false;
            })
            .times(2);

        CHECK(cursor::internal::can_get_more(cursor));
        CHECK(data.next_count == 0);
        CHECK(data.error_document_count == 0);

        {
            auto const iter = cursor.begin();

            CHECK(cursor::internal::is_dead(cursor));
            CHECK(cursor::internal::doc(cursor) == empty);
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
            CHECK(iter == cursor.end());
            CHECK(v1::cursor::iterator::internal::with(iter) == &cursor);
        }

        {
            auto const iter = cursor.begin();

            CHECK(cursor::internal::is_dead(cursor));
            CHECK(cursor::internal::doc(cursor) == empty);
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
            CHECK(iter == cursor.end());
            CHECK(v1::cursor::iterator::internal::with(iter) == nullptr);
        }
    }

    SECTION("tailable") {
        auto const type = GENERATE(cursor::type::k_tailable, cursor::type::k_tailable_await);

        auto cursor = cursor::internal::make(reinterpret_cast<mongoc_cursor_t*>(&data), type);

        cursor_next
            ->interpose([&](mongoc_cursor_t* cursor, bson_t const** bson) -> bool {
                (void)bson;
                CHECK(reinterpret_cast<test_data_type*>(cursor) == &data);
                ++data.next_count;
                return false;
            })
            .times(2);

        cursor_error_document
            ->interpose([&](mongoc_cursor_t const* cursor, bson_error_t* err, bson_t const** bson) -> bool {
                CHECK(reinterpret_cast<test_data_type const*>(cursor) == &data);
                REQUIRE(err);
                REQUIRE(bson);
                ++data.error_document_count;
                return false;
            })
            .times(2);

        CHECK(cursor::internal::can_get_more(cursor));
        CHECK(data.next_count == 0);
        CHECK(data.error_document_count == 0);

        {
            auto const iter = cursor.begin();

            CHECK(cursor::internal::can_get_more(cursor));
            CHECK(cursor::internal::doc(cursor) == empty);
            CHECK(data.next_count == 1);
            CHECK(data.error_document_count == 1);
            CHECK(iter == cursor.end());
            CHECK(iter->empty());
        }

        {
            auto const iter = cursor.begin();

            CHECK(cursor::internal::can_get_more(cursor));
            CHECK(cursor::internal::doc(cursor) == empty);
            CHECK(data.next_count == 2);
            CHECK(data.error_document_count == 2);
            CHECK(iter == cursor.end());
            CHECK(iter->empty());
        }

        {
            auto const iter = cursor.begin();

            CHECK(cursor::internal::has_doc(cursor));
            CHECK(cursor::internal::doc(cursor) == data.doc.view());
            CHECK(data.next_count == 3);
            CHECK(data.error_document_count == 2);
            CHECK(iter != cursor.end());
            CHECK(*iter == data.doc.view());
        }
    }
}

} // namespace v1
} // namespace mongocxx
