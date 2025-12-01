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

#include <mongocxx/test/private/scoped_bson.hh>

//

#include <bsoncxx/test/v1/document/value.hh>

#include <cstdint>
#include <stdexcept>
#include <utility>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/make_unique.hh>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

namespace mongocxx {

TEST_CASE("ownership", "[mongocxx][private][scoped_bson_view]") {
    scoped_bson source_owner{R"({"x": 1})"};
    scoped_bson target_owner{R"({"y": 2})"};

    REQUIRE(source_owner.data() != target_owner.data());

    auto source = source_owner.bson_view();
    auto target = target_owner.bson_view();

    REQUIRE(target.data() != source.data());
    REQUIRE(target.bson() != source.bson());
    REQUIRE_FALSE(bson_empty0(source.bson()));
    REQUIRE_FALSE(bson_empty0(target.bson()));
    REQUIRE(source.view() == source_owner.view());
    REQUIRE(target.view() == target_owner.view());

    auto const source_data = source.data();

    SECTION("copy") {
        auto copy = source;

        REQUIRE(source.data() == source_data);
        REQUIRE(source.bson() != nullptr);
        REQUIRE(bson_get_data(source.bson()) == source.data());

        REQUIRE(copy.data() == source_data);
        REQUIRE_FALSE(bson_empty0(copy.bson()));
        REQUIRE(bson_get_data(copy.bson()) == copy.data());
        REQUIRE(bson_compare(copy.bson(), source.bson()) == 0);

        target = copy;

        REQUIRE(copy.data() == source_data);
        REQUIRE(copy.bson() != nullptr);
        REQUIRE(bson_get_data(copy.bson()) == copy.data());

        REQUIRE(target.data() == source_data);
        REQUIRE_FALSE(bson_empty0(target.bson()));
        REQUIRE(bson_get_data(target.bson()) == target.data());
        REQUIRE(bson_compare(target.bson(), source.bson()) == 0);
    }
}

TEST_CASE("basic", "[mongocxx][private][scoped_bson_view]") {
    SECTION("null") {
        scoped_bson_view const v{nullptr};

        CHECK(v.data() == nullptr);
        CHECK(v.bson() == nullptr);
        CHECK_FALSE(v.view());

        CHECK(scoped_bson{v.copy()}.data() == nullptr);

        CHECK(v.value().data() == nullptr);
    }

    SECTION("empty") {
        bsoncxx::v1::document::view const empty;

        scoped_bson_view const v{empty};

        CHECK(v.data() == empty.data());
        REQUIRE(v.bson() != nullptr);
        CHECK(bson_get_data(v.bson()) == v.data());
        CHECK(v.view() == empty);

        CHECK(v.value().data() == empty.data());
    }

    SECTION("default") {
        bsoncxx::v1::document::view const empty;

        scoped_bson_view const v;

        CHECK(v.data() == empty.data());
        REQUIRE(v.bson() != nullptr);
        CHECK(bson_get_data(v.bson()) == v.data());
        CHECK(v.view() == empty);

        CHECK(v.value().data() == empty.data());
    }

    SECTION("value") {
        scoped_bson const owner{BCON_NEW("x", "1")};
        auto const v = owner.bson_view();

        CHECK(v.data() == owner.data());
        REQUIRE_FALSE(v.bson() == nullptr);
        CHECK(bson_get_data(v.bson()) == owner.data());
        CHECK(v.view() == owner.view());

        scoped_bson_view const copy{v.view()};

        CHECK(copy.data() == v.data());
        REQUIRE_FALSE(copy.bson() == nullptr);
        CHECK(bson_get_data(copy.bson()) == copy.data());
        CHECK(copy.view() == v.view());

        auto const value = copy.value();

        CHECK(value.data() != v.data());
        CHECK(value.view() == v.view());
    }

    SECTION("bson_t") {
        SECTION("null") {
            scoped_bson_view const v{nullptr};

            CHECK(v.data() == nullptr);
            CHECK(v.bson() == nullptr);
            CHECK_FALSE(v.view());
        }

        SECTION("empty") {
            std::uint8_t const data[] = {5, 0, 0, 0, 0}; // {}
            scoped_bson_view const x1{bsoncxx::v1::document::view{data, sizeof(data)}};

            scoped_bson_view const v{x1.bson()}; // bson_t const*

            CHECK(v.data() == x1.data());
            REQUIRE(v.bson() != nullptr);
            CHECK(bson_get_data(v.bson()) == v.data());
            CHECK(v.view() == x1.view());
        }

        SECTION("value") {
            std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // {"x": 1}
            scoped_bson_view const x1{bsoncxx::v1::document::view{data, sizeof(data)}};

            scoped_bson_view const v{x1.bson()}; // bson_t const*

            CHECK(v.data() == x1.data());
            REQUIRE(v.bson() != nullptr);
            CHECK(bson_get_data(v.bson()) == v.data());
            CHECK(v.view() == x1.view());

            scoped_bson_view const copy{v.bson()}; // bson_t const*

            CHECK(copy.data() == v.data());
            REQUIRE_FALSE(copy.bson() == nullptr);
            CHECK(bson_get_data(copy.bson()) == copy.data());
            CHECK(copy.view() == v.view());
        }
    }
}

TEST_CASE("out_ptr", "[mongocxx][private][scoped_bson_view]") {
    scoped_bson const x1_owner{R"({"x": 1})"};
    scoped_bson const x2_owner{R"({"x": 2})"};

    auto const x1 = x1_owner.bson_view();
    auto const x2 = x2_owner.bson_view();

    SECTION("null") {
        scoped_bson_view doc = x1;

        [&](bson_t const** bson_ptr) {
            REQUIRE(bson_ptr != nullptr);
            CHECK(*bson_ptr == nullptr);

            *bson_ptr = nullptr;
        }(doc.out_ptr());

        CHECK(doc.data() == nullptr);
        CHECK(doc.bson() == nullptr);
        CHECK_FALSE(doc.view());
    }

    SECTION("empty") {
        scoped_bson_view const empty;

        scoped_bson_view doc = x1;

        [&](bson_t const** bson_ptr) {
            REQUIRE(bson_ptr != nullptr);
            CHECK(*bson_ptr == nullptr);

            *bson_ptr = empty.bson();
        }(doc.out_ptr());

        CHECK(doc.data() == empty.data());
        REQUIRE(doc.bson() != nullptr);
        CHECK(bson_get_data(doc.bson()) == doc.data());
        CHECK(doc.view() == empty.view());
    }

    SECTION("value") {
        scoped_bson_view doc = x1;

        [&](bson_t const** bson_ptr) {
            REQUIRE(bson_ptr != nullptr);
            CHECK(*bson_ptr == nullptr);

            *bson_ptr = x2.bson();
        }(doc.out_ptr());

        CHECK(doc.data() == x2.data());
        REQUIRE(doc.bson() != nullptr);
        CHECK(bson_get_data(doc.bson()) == doc.data());
        CHECK(doc.view() == x2.view());
    }
}

TEST_CASE("ownership", "[mongocxx][private][scoped_bson]") {
    scoped_bson source{BCON_NEW("x", "1")};
    scoped_bson target{BCON_NEW("y", "2")};

    CHECK(target.data() != source.data());

    auto const original = source;
    auto const source_data = source.data();

    SECTION("move") {
        auto move = std::move(source);

        CHECK(source.data() == nullptr);
        CHECK(source.bson() == nullptr);

        CHECK(move.data() == source_data);
        REQUIRE(move.bson() != nullptr);
        CHECK(bson_get_data(move.bson()) == move.data());
        CHECK(bson_compare(move.bson(), original.bson()) == 0);

        target = std::move(move);

        CHECK(move.data() == nullptr);
        CHECK(move.bson() == nullptr);
        CHECK(target.data() == source_data);

        CHECK(target.data() == source_data);
        REQUIRE(target.bson() != nullptr);
        CHECK(bson_get_data(target.bson()) == target.data());
        CHECK(bson_compare(target.bson(), original.bson()) == 0);

        auto const doc = std::move(target).value();

        CHECK(target.data() == nullptr);
        CHECK(target.bson() == nullptr); // Assign-or-destroy-only.

        CHECK(doc.data() == source_data);
        CHECK(doc.view() == original.view());
    }

    SECTION("copy") {
        auto copy = source;
        auto const copy_data = copy.data();

        CHECK(source.data() == source_data);
        REQUIRE(source.bson() != nullptr);
        CHECK(bson_get_data(source.bson()) == source.data());

        CHECK(copy.data() != source_data);
        REQUIRE(copy.bson() != nullptr);
        CHECK(bson_get_data(copy.bson()) == copy.data());
        CHECK(bson_compare(copy.bson(), source.bson()) == 0);

        target = copy;

        CHECK(copy.data() == copy_data);
        REQUIRE(copy.bson() != nullptr);
        CHECK(bson_get_data(copy.bson()) == copy.data());

        CHECK(target.data() != copy_data);
        REQUIRE(target.bson() != nullptr);
        CHECK(bson_get_data(target.bson()) == target.data());
        CHECK(bson_compare(target.bson(), source.bson()) == 0);
    }
}

TEST_CASE("basic", "[mongocxx][private][scoped_bson]") {
    SECTION("null") {
        scoped_bson const v{bsoncxx::v1::document::view{nullptr}};

        CHECK(v.data() == nullptr);
        CHECK(v.bson() == nullptr);
        CHECK_FALSE(v.view());

        CHECK(scoped_bson{v.copy()}.data() == nullptr);
    }

    SECTION("empty") {
        bsoncxx::v1::document::view const empty;

        scoped_bson const v{empty};

        CHECK(v.data() == empty.data());
        REQUIRE(v.bson() != nullptr);
        CHECK(bson_get_data(v.bson()) == v.data());
        CHECK(v.view() == empty);
        CHECK(v.array_view() == bsoncxx::v1::array::view{});
    }

    SECTION("default") {
        bsoncxx::v1::document::view const empty;

        scoped_bson const v;

        CHECK(v.data() == empty.data());
        REQUIRE(v.bson() != nullptr);
        CHECK(bson_get_data(v.bson()) == v.data());
        CHECK(v.view() == empty);
        CHECK(v.array_view() == bsoncxx::v1::array::view{});
    }

    SECTION("value") {
        std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // {"x": 1}
        bsoncxx::v1::document::view const x1{data, sizeof(data)};

        scoped_bson const v{x1};

        CHECK(v.data() != x1.data());
        REQUIRE(v.bson() != nullptr);
        CHECK(bson_get_data(v.bson()) == v.data());
        CHECK(v.view() == x1);

        scoped_bson const copy{v.value()};

        CHECK(copy.data() != v.data());
        REQUIRE_FALSE(copy.bson() == nullptr);
        CHECK(bson_get_data(copy.bson()) == copy.data());
        CHECK(copy.view() == v.view());
    }

    SECTION("bson_t") {
        SECTION("null") {
            scoped_bson v{nullptr};

            CHECK(v.data() == nullptr);
            CHECK(v.bson() == nullptr);
            CHECK_FALSE(v.view());
        }

        SECTION("empty") {
            std::uint8_t const data[] = {5, 0, 0, 0, 0}; // {}
            scoped_bson_view x1{bsoncxx::v1::document::view{data, sizeof(data)}};

            scoped_bson v{x1.copy()}; // bson_t*

            CHECK(v.data() != x1.data());
            REQUIRE(v.bson() != nullptr);
            CHECK(bson_get_data(v.bson()) == v.data());
            CHECK(v.view() == x1.view());
        }

        SECTION("value") {
            std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // {"x": 1}
            scoped_bson_view x1{bsoncxx::v1::document::view{data, sizeof(data)}};

            scoped_bson v{x1.bson()}; // bson_t const*

            CHECK(v.data() != x1.data());
            REQUIRE(v.bson() != nullptr);
            CHECK(bson_get_data(v.bson()) == v.data());
            CHECK(v.view() == x1.view());

            scoped_bson const copy{v.copy()}; // bson_t const*

            CHECK(copy.data() != v.data());
            REQUIRE_FALSE(copy.bson() == nullptr);
            CHECK(bson_get_data(copy.bson()) == copy.data());
            CHECK(copy.view() == v.view());
        }
    }

    SECTION("json") {
        std::uint8_t const data[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // {"x": 1}
        bsoncxx::v1::document::view const x1{data, sizeof(data)};

        CHECK(scoped_bson{R"({"x": 1})"}.view() == x1);
    }
}

TEST_CASE("concat", "[mongocxx][private][scoped_bson]") {
    SECTION("invalid") {
        SECTION("lhs") {
            scoped_bson lhs{nullptr};
            scoped_bson rhs;

            CHECK_THROWS_MATCHES(
                lhs += rhs,
                std::logic_error,
                Catch::Matchers::Message("mongocxx::scoped_bson::operator+=: this->data() == nullptr"));
        }

        SECTION("rhs") {
            scoped_bson lhs;
            scoped_bson rhs{nullptr};

            CHECK_THROWS_MATCHES(
                lhs += rhs,
                std::logic_error,
                Catch::Matchers::Message("mongocxx::scoped_bson::operator+=: other.data() == nullptr"));
        }

        SECTION("concat") {
            auto const size = (std::size_t{BSON_MAX_SIZE} / 2u) + 1u;
            auto const data = bsoncxx::make_unique_for_overwrite<char[]>(size);
            auto const big_string = bsoncxx::v1::stdx::string_view{data.get(), size};

            REQUIRE(data);

            std::memset(data.get(), 'x', size - 1u);
            data[size - 1u] = '\0';

            scoped_bson lhs{BCON_NEW("x", BCON_UTF8(big_string.data()))};
            scoped_bson rhs{BCON_NEW("x", BCON_UTF8(big_string.data()))};

            REQUIRE(lhs.data() != nullptr);
            REQUIRE(rhs.data() != nullptr);

            CHECK_THROWS_MATCHES(
                lhs += rhs,
                std::logic_error,
                Catch::Matchers::Message("mongocxx::scoped_bson::operator+=: bson_concat failed"));
        }
    }

    SECTION("strong exception safety") {
        scoped_bson x1{BCON_NEW("x", "1")};
        scoped_bson x2{BCON_NEW("x", "2")};

        auto const x1_data = const_cast<std::uint8_t*>(x1.data());
        auto const x2_data = const_cast<std::uint8_t*>(x2.data());

        auto const first_byte = x1_data[0];

        auto const x1_doc = x1.value();
        auto const x2_doc = x2.value();

        x1_data[0] = 0;                              // Corrupt embedded length field.
        CHECK_THROWS_AS(x1 += x2, std::logic_error); // Self-assignment.
        x1_data[0] = first_byte;                     // Restore embedded length field.

        CHECK(x1.data() == x1_data); // No reallocation.
        CHECK(x1.view() == x1_doc);

        CHECK(x2.data() == x2_data); // No reallocation.
        CHECK(x2.view() == x2_doc);
    }

    SECTION("self") {
        bsoncxx::v1::document::view const empty;

        SECTION("empty") {
            scoped_bson v;

            v += v;

            CHECK(v.data() == empty.data());
            REQUIRE(v.bson() != nullptr);
            CHECK(bson_compare(v.bson(), scoped_bson_view{empty}.bson()) == 0);
        }

        SECTION("value") {
            scoped_bson const x1{R"({"x": 1})"};
            scoped_bson const x1x1{R"({"x": 1, "x": 1})"};

            auto v = x1;

            v += v;

            CHECK(v.data() != x1.data());
            REQUIRE(v.bson() != nullptr);
            CHECK(bson_compare(v.bson(), scoped_bson_view{x1x1}.bson()) == 0);
        }
    }

    SECTION("basic") {
        scoped_bson const x1{BCON_NEW("x", "1")};
        scoped_bson const x2{BCON_NEW("x", "2")};
        scoped_bson const x1x2{BCON_NEW("x", "1", "x", "2")};

        scoped_bson doc;

        CHECK(doc.view().empty());

        doc += x1;
        CHECK(doc.view() == x1.view());

        doc += x2;
        CHECK(doc.view() == x1x2.view());
    }

    SECTION("deleter") {
        auto& noop_deleter = bsoncxx::v1::document::value::noop_deleter;
        auto& bson_free = ::bson_free;

        scoped_bson const x1{R"({"x": 1})"};
        scoped_bson const x2{R"({"x": 2})"};
        scoped_bson const x1x2{R"({"x": 1, "x": 2})"};

        std::uint8_t data[] = {5, 0, 0, 0, 0}; // {}
        scoped_bson doc{bsoncxx::v1::document::value{data, &noop_deleter}};

        doc += x1; // noop_deleter -> bson_free

        CHECK(doc.view() == x1.view());

#if defined(__MINGW32__)
        {
            // Linking with mingw-w64 does not guarantee a unique address for `bson_free`.
            // Negative-test that the deleter is *no longer* `noop_deleter` instead.
            (void)bson_free;
            auto const deleter_ptr = doc.value().get_deleter().target<decltype(&noop_deleter)>();
            CHECK_FALSE(deleter_ptr);
        }
#else
        {
            auto const deleter_ptr = doc.value().get_deleter().target<decltype(&bson_free)>();
            REQUIRE(deleter_ptr);
            CHECK(*deleter_ptr == &bson_free);
        }
#endif

        doc += x2; // bson_free -> bson_free

        CHECK(doc.view() == x1x2.view());

#if defined(__MINGW32__)
        {
            // Linking with mingw-w64 does not guarantee a unique address for `bson_free`.
            // Negative-test that the deleter is *no longer* `noop_deleter` instead.
            (void)bson_free;
            auto const deleter_ptr = doc.value().get_deleter().target<decltype(&noop_deleter)>();
            CHECK_FALSE(deleter_ptr);
        }
#else
        {
            auto const deleter_ptr = doc.value().get_deleter().target<decltype(&bson_free)>();
            REQUIRE(deleter_ptr);
            CHECK(*deleter_ptr == &bson_free);
        }
#endif
    }
}

TEST_CASE("out_ptr", "[mongocxx][private][scoped_bson]") {
    scoped_bson const x1{R"({"x": 1})"};
    scoped_bson const x2{R"({"x": 2})"};

    SECTION("empty") {
        scoped_bson const empty;

        scoped_bson doc = x1;

        [&](bson_t* bson) {
            REQUIRE(bson != nullptr);
            REQUIRE(bson_empty(bson));

            bson_copy_to(empty.bson(), bson);
        }(doc.out_ptr());

        CHECK(doc.data() == empty.data());
        REQUIRE(doc.bson() != nullptr);
        CHECK(bson_get_data(doc.bson()) == doc.data());
        CHECK(doc.view() == empty.view());
    }

    SECTION("value") {
        scoped_bson doc = x1;

        [&](bson_t* bson) {
            REQUIRE(bson != nullptr);
            REQUIRE(bson_empty(bson));

            bson_copy_to(x2.bson(), bson);
        }(doc.out_ptr());

        CHECK(doc.data() != x2.data());
        REQUIRE(doc.bson() != nullptr);
        CHECK(bson_get_data(doc.bson()) == doc.data());
        CHECK(doc.view() == x2.view());
    }
}

TEST_CASE("inout_ptr", "[mongocxx][private][scoped_bson]") {
    scoped_bson const x1{R"({"x": 1})"};
    scoped_bson const x2{R"({"x": 2})"};

    SECTION("empty") {
        scoped_bson const empty;

        scoped_bson doc = x1;

        [&](bson_t* bson) {
            REQUIRE(bson != nullptr);
            CHECK(bson_compare(bson, x1.bson()) == 0);

            bson_destroy(bson);
            bson_copy_to(empty.bson(), bson);
        }(doc.inout_ptr());

        CHECK(doc.data() == empty.data());
        REQUIRE(doc.bson() != nullptr);
        CHECK(bson_get_data(doc.bson()) == doc.data());
        CHECK(doc.view() == empty.view());
    }

    SECTION("value") {
        scoped_bson doc = x1;

        [&](bson_t* bson) {
            REQUIRE(bson != nullptr);
            CHECK(bson_compare(bson, x1.bson()) == 0);

            bson_destroy(bson);
            bson_copy_to(x2.bson(), bson);
        }(doc.inout_ptr());

        CHECK(doc.data() != x2.data());
        REQUIRE(doc.bson() != nullptr);
        CHECK(bson_get_data(doc.bson()) == doc.data());
        CHECK(doc.view() == x2.view());
    }

    SECTION("stolen") {
        scoped_bson doc = x1;

        [&](bson_t* bson) {
            REQUIRE(bson != nullptr);
            CHECK(bson_compare(bson, x1.bson()) == 0);

            bson_t tmp;
            bson_steal(&tmp, bson);
            bson_destroy(&tmp);
        }(doc.inout_ptr());

        CHECK(doc.data() == nullptr);
        REQUIRE(doc.bson() == nullptr);
        CHECK_FALSE(doc.view());
    }
}

TEST_CASE("StringMaker", "[mongocxx][test][private][scoped_bson]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({})"},
        scoped_bson{R"([])"},
        scoped_bson{R"({"x": 1, "y": 2.0, "z": "3"})"},
        scoped_bson{R"([{"x": 1}, {"y": 2.0}, {"z": "3"}])"},
    }));

    CHECK(bsoncxx::test::stringify(v) == bsoncxx::test::stringify(v.view()));
}

} // namespace mongocxx
