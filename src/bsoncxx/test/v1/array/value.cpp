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

#include <bsoncxx/test/v1/array/value.hh>

//

#include <cstdint>

#include <bsoncxx/test/catch.hh>
#include <bsoncxx/test/stringify.hh>

namespace {

using bsoncxx::v1::array::value;
using bsoncxx::v1::array::view;

template <typename Deleter, typename T>
bsoncxx::v1::stdx::optional<Deleter> get_deleter(T const& v) {
    if (auto const deleter_ptr = v.get_deleter().template target<Deleter>()) {
        return *deleter_ptr;
    }
    return {};
}

TEST_CASE("exceptions", "[bsoncxx][v1][array][value]") {
    using code = bsoncxx::v1::document::view::errc;

    std::uint8_t const data[] = {5, 0, 0, 0, 0}; // {}

    REQUIRE(data[0] == sizeof(data));

    auto const make_ptr = [&data] {
        auto res = std::unique_ptr<std::uint8_t[]>(new std::uint8_t[sizeof(data)]);
        std::memcpy(res.get(), data, sizeof(data));
        return res;
    };

    SECTION("invalid_length") {
        SECTION("value(std::uint8_t*, std::size_t, Deleter)") {
            auto const deleter = [](std::uint8_t* p) { delete[] p; };
            using deleter_type = std::reference_wrapper<decltype(deleter)>;

            auto const expr = [&](std::size_t length) {
                return value{make_ptr().release(), length, std::ref(deleter)};
            };

            CHECK_THROWS_WITH_CODE(expr(0u), code::invalid_length);
            CHECK_THROWS_WITH_CODE(expr(sizeof(data) - 1u), code::invalid_length);

            CHECK(get_deleter<deleter_type>(expr(sizeof(data))).has_value());
            CHECK(get_deleter<deleter_type>(expr(sizeof(data) + 1u)).has_value());

            auto ptr = make_ptr();
            ++ptr[0];
            CHECK_THROWS_WITH_CODE((value{ptr.release(), sizeof(data), std::ref(deleter)}), code::invalid_length);
        }

        SECTION("value(std::uint8_t*, std::size_t)") {
            auto const expr = [&](std::size_t length) { return value{make_ptr().release(), length}; };

            CHECK_THROWS_WITH_CODE(expr(0), code::invalid_length);
            CHECK_THROWS_WITH_CODE(expr(sizeof(data) - 1u), code::invalid_length);

            CHECK(get_deleter<value::default_deleter_type>(expr(sizeof(data))).has_value());
            CHECK(get_deleter<value::default_deleter_type>(expr(sizeof(data) + 1u)).has_value());

            auto ptr = make_ptr();
            ++ptr[0];
            CHECK_THROWS_WITH_CODE((value{ptr.release(), sizeof(data)}), code::invalid_length);
        }

        SECTION("value(unique_ptr_type, std::size_t)") {
            auto const expr = [&](std::size_t length) { return value{make_ptr(), length}; };

            CHECK_THROWS_WITH_CODE(expr(0), code::invalid_length);
            CHECK_THROWS_WITH_CODE(expr(sizeof(data) - 1u), code::invalid_length);

            CHECK(get_deleter<value::default_deleter_type>(expr(sizeof(data))).has_value());
            CHECK(get_deleter<value::default_deleter_type>(expr(sizeof(data) + 1u)).has_value());

            auto ptr = make_ptr();
            ++ptr[0];
            CHECK_THROWS_WITH_CODE((value{std::move(ptr), sizeof(data)}), code::invalid_length);
        }
    }
}

TEST_CASE("ownership", "[bsoncxx][v1][array][value]") {
    using default_deleter_type = value::default_deleter_type;
    using noop_deleter_type = bsoncxx::v1::document::value::noop_deleter_type;

    bsoncxx::v1::document::view const empty;

    auto const noop_deleter = &bsoncxx::v1::document::value::noop_deleter;

    std::uint8_t const xdoc[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // { 'x': 1 }

    auto xdoc_owner = std::unique_ptr<std::uint8_t[]>(new std::uint8_t[sizeof(xdoc)]);
    std::memcpy(xdoc_owner.get(), xdoc, sizeof(xdoc));

    SECTION("invalid") {
        value v{nullptr};

        REQUIRE(v.data() == nullptr);
        REQUIRE(get_deleter<default_deleter_type>(v).has_value());

        SECTION("move") {
            auto const move = std::move(v);

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(move.data() == nullptr);
            CHECK(get_deleter<default_deleter_type>(move).has_value());
        }

        SECTION("copy") {
            auto const copy = v;

            CHECK(v.data() == nullptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(copy.data() == nullptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());
        }

        SECTION("release") {
            auto const ptr = v.release();

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(ptr.get() == nullptr);
            REQUIRE(get_deleter<default_deleter_type>(ptr).has_value());
        }

        SECTION("reset with value") {
            auto const data_ptr = xdoc_owner.get();

            v.reset(value{std::move(xdoc_owner)});

            CHECK(v.data() == data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());
        }

        SECTION("reset with view") {
            auto const data_ptr = xdoc;

            v.reset(view{xdoc});

            CHECK(v.data() != nullptr);
            CHECK(v.data() != data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(v == view{xdoc});
        }
    }

    SECTION("default") {
        value v;

        REQUIRE(v.data() == empty.data());
        REQUIRE(get_deleter<noop_deleter_type>(v) == noop_deleter);

        SECTION("release") {
            auto const ptr = v.release();

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(ptr.get() == empty.data());
            CHECK(get_deleter<noop_deleter_type>(ptr) == noop_deleter);
        }

        SECTION("move") {
            auto const move = std::move(v);

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(move.data() == empty.data());
            CHECK(get_deleter<noop_deleter_type>(move) == noop_deleter);
        }

        SECTION("copy") {
            auto const copy = v;

            CHECK(v.data() == empty.data());
            CHECK(get_deleter<noop_deleter_type>(v) == noop_deleter);

            CHECK(copy.data() == empty.data());
            CHECK(get_deleter<noop_deleter_type>(copy) == noop_deleter);
        }

        SECTION("reset with value") {
            auto const data_ptr = xdoc_owner.get();

            v.reset(value{std::move(xdoc_owner)});

            CHECK(v.data() == data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());
        }

        SECTION("reset with view") {
            auto const data_ptr = xdoc;

            v = view{xdoc};

            CHECK(v.data() != data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());
        }
    }

    SECTION("owning") {
        auto const owner_ptr = xdoc_owner.get();

        value v{std::move(xdoc_owner)};

        REQUIRE(v.data() == owner_ptr);
        REQUIRE(get_deleter<default_deleter_type>(v).has_value());

        SECTION("release") {
            auto const ptr = v.release();

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(ptr.get() == owner_ptr);
            CHECK(get_deleter<default_deleter_type>(ptr).has_value());
        }

        SECTION("move") {
            auto move = std::move(v);

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(move.data() == owner_ptr);
            CHECK(get_deleter<default_deleter_type>(move).has_value());

            v = std::move(move);

            CHECK(move.data() == nullptr);
            CHECK_NOFAIL(!move.get_deleter()); // Valid but unspecified state.

            CHECK(v.data() == owner_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());
        }

        SECTION("copy") {
            auto const copy = v;

            CHECK(v.data() == owner_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(copy.data() != nullptr);
            CHECK(copy.data() != owner_ptr);
            CHECK(get_deleter<default_deleter_type>(copy).has_value());

            CHECK(copy == view{xdoc});

            auto const copy_ptr = copy.data();

            v = copy;

            CHECK(copy.data() == copy_ptr);
            CHECK(get_deleter<default_deleter_type>(copy).has_value());

            CHECK(v.data() != nullptr);
            CHECK(v.data() != copy_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(v == copy);
        }

        SECTION("reset with value") {
            auto copy = v;
            auto const data_ptr = copy.data();

            REQUIRE(v.data() != data_ptr);

            v.reset(std::move(copy));

            CHECK(v.data() != owner_ptr);
            CHECK(v.data() == data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(v == view{xdoc});
        }

        SECTION("reset with view") {
            auto const data_ptr = xdoc;

            REQUIRE(v.data() != data_ptr);

            v.reset(view{xdoc});

            CHECK(v.data() != owner_ptr);
            CHECK(v.data() != data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(v == view{xdoc});
        }
    }

    SECTION("custom deleter") {
        std::uint8_t bytes[] = {5, 0, 0, 0, 0}; // {}
        auto const bytes_ptr = bytes;
        auto const deleter = [bytes_ptr](std::uint8_t* p) { CHECK(p == bytes_ptr); };
        using deleter_type = std::reference_wrapper<decltype(deleter)>;

        value v{bytes, std::ref(deleter)};
        auto const data_ptr = v.data();

        REQUIRE(data_ptr == bytes_ptr);
        REQUIRE(get_deleter<deleter_type>(v).has_value());

        SECTION("release") {
            auto const ptr = v.release();

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(ptr.get() == data_ptr);
            CHECK(get_deleter<deleter_type>(ptr).has_value());
        }

        SECTION("move") {
            auto move = std::move(v);

            CHECK(v.data() == nullptr);
            CHECK_NOFAIL(!v.get_deleter()); // Valid but unspecified state.

            CHECK(move.data() == data_ptr);
            CHECK(get_deleter<deleter_type>(move).has_value());

            v = std::move(move);

            CHECK(move.data() == nullptr);
            CHECK_NOFAIL(!move.get_deleter()); // Valid but unspecified state.

            CHECK(v.data() == data_ptr);
            CHECK(get_deleter<deleter_type>(v).has_value());
        }

        SECTION("copy") {
            auto const copy = v;

            CHECK(v.data() == data_ptr);
            CHECK(get_deleter<deleter_type>(v).has_value());

            CHECK(copy.data() != nullptr);
            CHECK(copy.data() != data_ptr);
            CHECK(get_deleter<default_deleter_type>(copy).has_value());

            CHECK(copy == view{bytes});

            auto const copy_ptr = copy.data();

            v = copy;

            CHECK(copy.data() == copy_ptr);
            CHECK(get_deleter<default_deleter_type>(copy).has_value());

            CHECK(v.data() != nullptr);
            CHECK(v.data() != copy_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(v == copy);
        }

        SECTION("reset with value") {
            auto copy = v;
            auto const data_ptr = copy.data();

            REQUIRE(v.data() != data_ptr);

            v.reset(std::move(copy));

            CHECK(v.data() != bytes_ptr);
            CHECK(v.data() == data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(v == view{bytes});
        }

        SECTION("reset with view") {
            auto const data_ptr = xdoc;

            REQUIRE(v.data() != data_ptr);

            v.reset(view{xdoc});

            CHECK(v.data() != bytes_ptr);
            CHECK(v.data() != data_ptr);
            CHECK(get_deleter<default_deleter_type>(v).has_value());

            CHECK(v == view{xdoc});
        }
    }
}

TEST_CASE("basic", "[bsoncxx][v1][array][value]") {
    auto const invalid = value{nullptr};

    SECTION("invalid") {
        REQUIRE_FALSE(invalid);

        CHECK(invalid.data() == nullptr);
        CHECK(invalid.size() == 0u);
        CHECK_FALSE(invalid.empty());

        CHECK(invalid.begin() == invalid.begin());
        CHECK(invalid.end() == invalid.end());
        CHECK(invalid.begin() == invalid.end());

        CHECK(invalid.find(0) == invalid.end());
        CHECK_FALSE(invalid[0]);

        CHECK(invalid == invalid);
        CHECK_FALSE(invalid != invalid);
        CHECK(invalid != view{});
        CHECK(invalid == value{invalid});
    }

    SECTION("default") {
        value const v;

        REQUIRE(v);

        CHECK(v.data() == value{}.data());
        CHECK(v.size() == 5u);
        CHECK(v.empty());

        CHECK(v.begin() == v.begin());
        CHECK(v.end() == v.end());
        CHECK(v.begin() == v.end());

        CHECK(v.find(0) == v.end());
        CHECK_FALSE(v[0]);

        CHECK(v == v);
        CHECK_FALSE(v != v);
        CHECK(v == view{});
        CHECK(v == value{v});

        CHECK_FALSE(v == invalid);
        CHECK(v != invalid);
        CHECK(v.begin() == invalid.begin());
        CHECK(v.end() == invalid.end());
    }

    SECTION("valid") {
        std::uint8_t const xdoc[] = {12, 0, 0, 0, 16, 'x', '\0', 1, 0, 0, 0, 0}; // { 'x': 1 }

        auto xdoc_owner = std::unique_ptr<std::uint8_t[]>(new std::uint8_t[sizeof(xdoc)]);
        std::memcpy(xdoc_owner.get(), xdoc, sizeof(xdoc));
        auto const data_ptr = xdoc_owner.get();

        value const v{std::move(xdoc_owner)};

        REQUIRE(v);

        CHECK(v != value{});

        CHECK(v.data() == data_ptr);
        CHECK(v.size() == sizeof(xdoc));
        CHECK(v.size() == v.length());
        CHECK_FALSE(v.empty());

        CHECK(v.begin() == v.begin());
        CHECK(v.begin() == v.cbegin());
        CHECK(v.end() == v.end());
        CHECK(v.end() == v.cend());
        CHECK(v.begin() != v.end());

        CHECK(v == v);
        CHECK_FALSE(v != v);
        CHECK(v != view{});
        CHECK(v == value{v});

        CHECK_FALSE(v == invalid);
        CHECK(v != invalid);
        CHECK(v.begin() != invalid.begin());
        CHECK(v.end() == invalid.end());
    }
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][array][value]") {
    SECTION("invalid") {
        value doc{nullptr};

        REQUIRE_FALSE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == "invalid");
    }

    SECTION("empty") {
        value doc;

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == "[]");
    }

    SECTION("one") {
        // {"0": 1}
        std::uint8_t const bytes[] = {
            // clang-format off
            12, 0, 0, 0,
            16, '0', '\0', 1, 0, 0, 0, // "0": 1
            0,
            // clang-format on
        };
        auto ptr = new std::uint8_t[sizeof(bytes)];
        std::memcpy(ptr, bytes, sizeof(bytes));

        value doc{ptr};

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == R"([1])");
    }

    SECTION("two") {
        // {"x": 1, "y": 2}
        std::uint8_t const bytes[] = {
            // clang-format off
            19, 0, 0, 0,
            16, '0', '\0', 1, 0, 0, 0, // "0": 1
            16, '1', '\0', 2, 0, 0, 0, // "1": 2
            0,
            // clang-format on
        };
        auto ptr = new std::uint8_t[sizeof(bytes)];
        std::memcpy(ptr, bytes, sizeof(bytes));

        value doc{ptr};

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == R"([1, 2])");
    }

    SECTION("three") {
        // {"x": 1, "y": 2, "z": 3}
        std::uint8_t bytes[] = {
            // clang-format off
            26, 0, 0, 0,
            16, '0', '\0', 1, 0, 0, 0, // "0": 1
            16, '1', '\0', 2, 0, 0, 0, // "1": 2
            16, '2', '\0', 3, 0, 0, 0, // "2": 3
            0,
            // clang-format on
        };
        auto ptr = new std::uint8_t[sizeof(bytes)];
        std::memcpy(ptr, bytes, sizeof(bytes));

        value doc{ptr};

        REQUIRE(static_cast<bool>(doc));

        CHECK(bsoncxx::test::stringify(doc) == R"([1, 2, 3])");
    }
}

} // namespace
