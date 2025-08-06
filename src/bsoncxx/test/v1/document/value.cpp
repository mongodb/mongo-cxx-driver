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

#include <bsoncxx/test/v1/document/value.hh>

//

#include <bsoncxx/v1/detail/type_traits.hpp>

#include <bsoncxx/test/v1/document/view.hh>
#include <bsoncxx/test/v1/types/view.hh>

#include <cstdint>
#include <cstring>
#include <functional>
#include <sstream>
#include <utility>

#include <bsoncxx/test/catch.hh>
#include <bsoncxx/test/stringify.hh>
#include <bsoncxx/test/system_error.hh>

#include <catch2/matchers/catch_matchers_string.hpp>

namespace {

using bsoncxx::v1::document::value;
using bsoncxx::v1::document::view;

template <typename Deleter, typename T>
bsoncxx::v1::stdx::optional<Deleter> get_deleter(T const& v) {
    if (auto const deleter_ptr = v.get_deleter().template target<Deleter>()) {
        return *deleter_ptr;
    }
    return {};
}

TEST_CASE("exceptions", "[bsoncxx][v1][document][value]") {
    using code = bsoncxx::v1::document::view::errc;

    std::uint8_t const data[] = {5, 0, 0, 0, 0}; // {}

    REQUIRE(data[0] == sizeof(data));

    auto const make_ptr = [&data] {
        // Reserve an extra byte to support the `sizeof(data) + 1` test cases.
        auto res = std::unique_ptr<std::uint8_t[]>(new std::uint8_t[sizeof(data) + 1u]);
        std::memcpy(res.get(), data, sizeof(data)); // Leave extra byte uninitialized.
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

TEST_CASE("ownership", "[bsoncxx][v1][document][value]") {
    using default_deleter_type = value::default_deleter_type;
    using noop_deleter_type = value::noop_deleter_type;

    bsoncxx::v1::document::view const empty;

    auto const noop_deleter = &value::noop_deleter;

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

TEST_CASE("basic", "[bsoncxx][v1][document][value]") {
    auto const invalid = value{nullptr};
    auto const empty_key = bsoncxx::v1::stdx::string_view{};

    SECTION("invalid") {
        REQUIRE_FALSE(invalid);

        CHECK(invalid.data() == nullptr);
        CHECK(invalid.size() == 0u);
        CHECK_FALSE(invalid.empty());

        CHECK(invalid.begin() == invalid.begin());
        CHECK(invalid.end() == invalid.end());
        CHECK(invalid.begin() == invalid.end());

        CHECK(invalid.find(empty_key) == invalid.end());
        CHECK_FALSE(invalid[empty_key]);

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

        CHECK(v.find(empty_key) == v.end());
        CHECK_FALSE(v[empty_key]);

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

        CHECK(v.find("x") != v.end());
        CHECK(v.find("x")->get_int32().value == 1);
        CHECK(v.find("y") == v.end());

        CHECK(v["x"].get_int32().value == 1);
        CHECK_FALSE(v["y"]);

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

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunused-member-function"));

struct Q {
    // void to_bson(Q const&, value&) {}
    void to_bson(Q const&, value&&) {}
    void to_bson(Q&, value&&) {}
    void to_bson(Q&&, value&) {}

    // void from_bson(Q&, value const&) {}
    void from_bson(Q const&, value const&) {}
    void from_bson(Q&, value&) {}
    void from_bson(Q&, value&&) {}
};

struct S {
    std::uint8_t bytes[12] = {}; // { 's': i }
    bool should_throw = false;

    S() = default;

    explicit S(std::uint8_t i, bool should_throw)
        : bytes{12, 0, 0, 0, 16, 's', '\0', i, 0, 0, 0, 0}, should_throw{should_throw} {
        REQUIRE(view{bytes});
    }

    explicit S(std::uint8_t i) : S{i, false} {}

    friend bool operator==(S const& lhs, S const& rhs) {
        return std::memcmp(lhs.bytes, rhs.bytes, sizeof(bytes)) == 0;
    }

    friend bool operator!=(S const& lhs, S const& rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, S const& s) {
        return os << 'S' << static_cast<int>(s.bytes[7]);
    }

    friend void to_bson(S const& s, value& v) {
        if (s.should_throw) {
            std::ostringstream oss;
            oss << s;
            throw std::runtime_error(std::move(oss).str());
        }

        REQUIRE(v.empty());
        v.reset(view{s.bytes});
        REQUIRE(v.size() == sizeof(s.bytes));
    }

    friend void from_bson(S& s, view v) {
        if (s.should_throw) {
            std::ostringstream oss;
            oss << s;
            throw std::runtime_error(std::move(oss).str());
        }

        REQUIRE(v.size() == sizeof(s.bytes));
        std::memcpy(s.bytes, v.data(), sizeof(s.bytes));
    }
};

BSONCXX_PRIVATE_WARNINGS_POP();

TEST_CASE("user-defined types", "[bsoncxx][v1][document][value]") {
    SECTION("valid") {
        S const s1{1};
        S const s2{2};

        REQUIRE(s1 == s1);
        REQUIRE(s2 == s2);
        REQUIRE(s1 != s2);

        value v{s1}; // to_bson
        auto const& cv = v;

        CHECK(cv.data() != static_cast<void const*>(s1.bytes));
        CHECK(cv == view{s1.bytes});

        S s = cv.get<S>(); // from_bson
        CHECK(s == s1);

        v = s2; // to_bson

        cv.get(s); // from_bson
        CHECK(s == s2);
    }

    SECTION("exception safety") {
        S s1{1, false};
        S s2{2, true};

        view const vs1{s1.bytes};

        value v{vs1};

        REQUIRE(v == vs1);

        CHECK_THROWS_WITH(value{s2}, Catch::Matchers::Equals("S2")); // to_bson

        CHECK(v == vs1);

        CHECK_THROWS_WITH(v = s2, Catch::Matchers::Equals("S2")); // to_bson
        CHECK(v == vs1);

        CHECK_THROWS_WITH(v.get(s2), Catch::Matchers::Equals("S2")); // from_bson
        CHECK(v == vs1);
    }
}

TEST_CASE("StringMaker", "[bsoncxx][test][v1][document][value]") {
    SECTION("invalid") {
        value v{nullptr};

        REQUIRE_FALSE(static_cast<bool>(v));

        CHECK(bsoncxx::test::stringify(v) == "invalid");
    }

    SECTION("empty") {
        value v;

        REQUIRE(static_cast<bool>(v));

        CHECK(bsoncxx::test::stringify(v) == "{}");
    }

    SECTION("one") {
        // {"x": 1}
        std::uint8_t const bytes[] = {
            // clang-format off
            12, 0, 0, 0,
            16, 'x', '\0', 1, 0, 0, 0, // "x": 1
            0,
            // clang-format on
        };
        auto ptr = new std::uint8_t[sizeof(bytes)];
        std::memcpy(ptr, bytes, sizeof(bytes));

        value v{ptr};

        REQUIRE(static_cast<bool>(v));

        CHECK(bsoncxx::test::stringify(v) == R"({"x": 1})");
    }

    SECTION("two") {
        // {"x": 1, "y": 2}
        std::uint8_t const bytes[] = {
            // clang-format off
            19, 0, 0, 0,
            16, 'x', '\0', 1, 0, 0, 0, // "x": 1
            16, 'y', '\0', 2, 0, 0, 0, // "y": 2
            0,
            // clang-format on
        };
        auto ptr = new std::uint8_t[sizeof(bytes)];
        std::memcpy(ptr, bytes, sizeof(bytes));

        value v{ptr};

        REQUIRE(static_cast<bool>(v));

        CHECK(bsoncxx::test::stringify(v) == R"({"x": 1, "y": 2})");
    }

    SECTION("three") {
        // {"x": 1, "y": 2, "z": 3}
        std::uint8_t bytes[] = {
            // clang-format off
            26, 0, 0, 0,
            16, 'x', '\0', 1, 0, 0, 0, // "x": 1
            16, 'y', '\0', 2, 0, 0, 0, // "y": 2
            16, 'z', '\0', 3, 0, 0, 0, // "z": 3
            0,
            // clang-format on
        };
        auto ptr = new std::uint8_t[sizeof(bytes)];
        std::memcpy(ptr, bytes, sizeof(bytes));

        value v{ptr};

        REQUIRE(static_cast<bool>(v));

        CHECK(bsoncxx::test::stringify(v) == R"({"x": 1, "y": 2, "z": 3})");
    }
}

namespace static_assertions {

BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wunused-template"));

template <typename T>
void copy_list_init_fn(T) {}

template <typename V, typename T>
using get_ns_expr = decltype(std::declval<V>().template get<T>());

static_assert(!std::is_constructible<value, Q>::value, "Q does not have a valid to_bson() overload");
static_assert(
    !bsoncxx::detail::is_detected<get_ns_expr, value, Q>::value,
    "Q does not have a valid from_bson() overload");

static_assert(!std::is_convertible<S, value>::value, "to_bson() ctor must be explicit");
static_assert(std::is_constructible<value, S>::value, "S::to_bson() must be found via ADL");
static_assert(std::is_assignable<value, S>::value, "S::to_bson() must be found via ADL");

template <typename T>
using empty_direct_list_init_expr = decltype(new (nullptr) T({}));
template <typename T>
using empty_copy_list_init_expr = decltype(copy_list_init_fn<T>({}));
static_assert(
    !bsoncxx::detail::is_detected<empty_copy_list_init_expr>::value,
    "bsoncxx::v1::document::value does not support empty list initialization");
static_assert(
    !bsoncxx::detail::is_detected<empty_direct_list_init_expr>::value,
    "bsoncxx::v1::document::value does not support empty list initialization");

template <typename T>
using list_init_nullptr = decltype(new (nullptr) T(nullptr)); // value(std::uint8_t*)
static_assert(
    bsoncxx::detail::is_detected<list_init_nullptr, value>::value,
    "value(nullptr) must unambiguously select the std::uint8_t* ctor");

template <typename T>
using list_init_nullptr_list = decltype(new (nullptr) T({{}, {}})); // value(unique_ptr_type)
static_assert(
    bsoncxx::detail::is_detected<list_init_nullptr, value>::value,
    "value({nullptr, {}}) must unambiguously select the unique_ptr_type ctor");

} // namespace static_assertions

} // namespace
