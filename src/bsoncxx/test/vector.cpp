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

#include <bsoncxx/v1/detail/macros.hpp>

#include <array>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/sub_binary.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/vector/formats.hpp>
#include <bsoncxx/vector/view.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#define ALL_VECTOR_FORMATS vector::formats::f_float32, vector::formats::f_int8, vector::formats::f_packed_bit

namespace {

using namespace bsoncxx;

template <typename Format>
struct format_specific;

template <>
struct format_specific<vector::formats::f_float32> {
    using value_type = float;
    static constexpr std::array<uint8_t, 2> bytes_empty() {
        return {0x27, 0x00};
    }
    static constexpr std::array<uint8_t, 6> bytes_unit() {
        return {0x27, 0x00, 0x00, 0x00, 0x80, 0x3f};
    }
    static constexpr value_type element_unit() {
        return 1.f;
    }
};

template <>
struct format_specific<vector::formats::f_int8> {
    using value_type = int8_t;
    static constexpr std::array<uint8_t, 2> bytes_empty() {
        return {0x03, 0x00};
    }
    static constexpr std::array<uint8_t, 3> bytes_unit() {
        return {0x03, 0x00, 0x01};
    }
    static constexpr value_type element_unit() {
        return 1;
    }
};

template <>
struct format_specific<vector::formats::f_packed_bit> {
    using value_type = bool;
    static constexpr std::array<uint8_t, 2> bytes_empty() {
        return {0x10, 0x00};
    }
    static constexpr std::array<uint8_t, 3> bytes_unit() {
        return {0x10, 0x07, 0x80};
    }
    static constexpr value_type element_unit() {
        return true;
    }
};

template <typename Sequence>
void binary_eq_bytes(types::b_binary const& binary, Sequence const& bytes) {
    REQUIRE(binary.size == bytes.size());
    REQUIRE(std::memcmp(binary.bytes, bytes.data(), bytes.size()) == 0);
}

template <typename Iterator, typename Element>
void iterator_operations(
    Iterator const& begin,
    Iterator const& end,
    std::ptrdiff_t expected_size,
    Element element_unit) {
    REQUIRE(end - begin == expected_size);

    for (std::ptrdiff_t outer_index = 0; outer_index < 50; outer_index++) {
        Iterator outer_front = begin + outer_index;
        Iterator outer_back = end - outer_index;

        REQUIRE(outer_front - begin == outer_index);
        REQUIRE(begin - outer_front == -outer_index);

        REQUIRE(end - outer_back == outer_index);
        REQUIRE(outer_back - end == -outer_index);

        for (std::ptrdiff_t inner_index = 0; inner_index < 20; inner_index++) {
            Iterator inner_front = outer_front + inner_index;
            Iterator inner_back = outer_back - inner_index;

            REQUIRE(inner_front - outer_front == inner_index);
            REQUIRE(outer_front - inner_front == -inner_index);

            REQUIRE(outer_back - inner_back == inner_index);
            REQUIRE(inner_back - outer_back == -inner_index);
        }
    }

    Iterator iter_copy = begin;
    REQUIRE(iter_copy == begin);
    REQUIRE(iter_copy >= begin);
    REQUIRE(iter_copy <= begin);
    REQUIRE_FALSE(iter_copy != begin);
    REQUIRE_FALSE(iter_copy < begin);

    REQUIRE(++iter_copy - begin == 1);
    REQUIRE(iter_copy > begin);
    REQUIRE(iter_copy >= begin);
    REQUIRE(iter_copy != begin);
    REQUIRE_FALSE(iter_copy == begin);
    REQUIRE_FALSE(iter_copy <= begin);
    REQUIRE_FALSE(iter_copy < begin);

    REQUIRE(--iter_copy - begin == 0);
    REQUIRE(iter_copy == begin);

    REQUIRE(iter_copy++ - begin == 0);
    REQUIRE(iter_copy-- - begin == 1);
    REQUIRE(iter_copy == begin);

    std::generate(begin, end, [&] { return element_unit; });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value == element_unit); });

    std::copy(begin, begin + (expected_size / 2), begin + (expected_size / 2));
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value == element_unit); });

    std::for_each(begin, end, [&](auto&& value) { value = element_unit; });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value == element_unit); });

    std::fill(begin, end, element_unit);
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value == element_unit); });

    std::sort(begin, end);
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value == element_unit); });

    *(end - 1) = Element{0};
    std::sort(begin, end);
    REQUIRE(*begin == Element{0});
    std::for_each(begin + 1, end, [&](auto const& value) { REQUIRE(value == element_unit); });

    std::for_each(begin, end, [&](auto&& value) { value = Element{0}; });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value != element_unit); });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value < element_unit); });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE(value <= element_unit); });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE_FALSE(value == element_unit); });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE_FALSE(value > element_unit); });
    std::for_each(begin, end, [&](auto const& value) { REQUIRE_FALSE(value >= element_unit); });
}

TEMPLATE_TEST_CASE("all vector view formats", "[bsoncxx::vector::view]", ALL_VECTOR_FORMATS) {
    using test_format_specific = format_specific<TestType>;
    using value_type = typename test_format_specific::value_type;

    SECTION("accept a valid vector with no elements") {
        auto bytes = test_format_specific::bytes_empty();
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        vector::view<TestType const> view{binary};
        REQUIRE(view.empty());
        REQUIRE(view.size() == 0);
        REQUIRE(view.byte_size() == 0);
        CHECK_THROWS_AS(view.at(0), std::out_of_range);
        CHECK_THROWS_AS(view.byte_at(0), std::out_of_range);
    }

    SECTION("decode a valid vector with a single element") {
        auto bytes = test_format_specific::bytes_unit();
        auto element = test_format_specific::element_unit();
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        vector::view<TestType const> view{binary};
        REQUIRE_FALSE(view.empty());
        REQUIRE(view.size() == 1u);
        REQUIRE(view.byte_size() == bytes.size() - 2u);
        REQUIRE(view.at(0) == element);
        REQUIRE(view[0] == element);
        REQUIRE(view.byte_at(0) == bytes[2]);
        REQUIRE(view.byte_at(bytes.size() - 3u) == bytes[bytes.size() - 1u]);
        CHECK_THROWS_AS(view.at(1), std::out_of_range);
        CHECK_THROWS_AS(view.byte_at(bytes.size() - 2u), std::out_of_range);
    }

    SECTION("reject binary data of the wrong sub_type") {
        auto bytes = test_format_specific::bytes_empty();
        auto invalid_type = GENERATE(
            binary_sub_type::k_binary, binary_sub_type::k_encrypted, binary_sub_type::k_uuid, binary_sub_type::k_user);
        types::b_binary const binary{invalid_type, bytes.size(), bytes.data()};
        REQUIRE_THROWS_WITH(
            vector::view<TestType const>(binary), Catch::Matchers::ContainsSubstring("invalid BSON vector"));
    }

    SECTION("reject binary data that's too short to include a header") {
        auto bytes = test_format_specific::bytes_empty();
        auto bytes_to_remove = GENERATE(1u, 2u);
        REQUIRE(bytes.size() >= bytes_to_remove);
        types::b_binary const binary{binary_sub_type::k_vector, uint32_t(bytes.size() - bytes_to_remove), bytes.data()};
        REQUIRE_THROWS_WITH(
            vector::view<TestType const>(binary), Catch::Matchers::ContainsSubstring("invalid BSON vector"));
    }

    SECTION("reject empty vectors with any modified header bits") {
        for (unsigned bit_index = 0; bit_index < 16; bit_index++) {
            auto bytes = test_format_specific::bytes_empty();
            bytes[bit_index >> 3u] ^= std::uint8_t(1u << (bit_index & 7u));
            types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
            REQUIRE_THROWS_WITH(
                vector::view<TestType const>(binary), Catch::Matchers::ContainsSubstring("invalid BSON vector"));
        }
    }

    SECTION("encode a single element as expected") {
        using namespace builder::basic;
        auto expected_bytes = test_format_specific::bytes_unit();
        auto element = test_format_specific::element_unit();
        bsoncxx::document::value doc =
            make_document(kvp("vector", [&](sub_binary sbin) { sbin.allocate(TestType{}, 1u)[0u] = element; }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        REQUIRE(binary.sub_type == binary_sub_type::k_vector);
        binary_eq_bytes(binary, expected_bytes);
        vector::view<TestType const> validate_encoded{binary};
    }

    SECTION("support algorithms and operators on element iterators") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            // Avoid multiples of 8, to cover nonzero packed_bit 'padding'.
            auto view = sbin.allocate(TestType{}, 8007u);
            iterator_operations(
                view.begin(), view.end(), std::ptrdiff_t(view.size()), test_format_specific::element_unit());
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::view<TestType const> validate_encoded{binary};
        REQUIRE(binary.size > 1000u);
    }

    SECTION("support algorithms and operators on byte iterators") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            // Choose a multiple of 8, to avoid the effects of masking unused bits.
            auto view = sbin.allocate(TestType{}, 8000u);
            iterator_operations(view.byte_begin(), view.byte_end(), std::ptrdiff_t(view.byte_size()), uint8_t(1));
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::view<TestType const> validate_encoded{binary};
        REQUIRE(binary.size > 1000u);
    }

    SECTION("support assignment between referenced elements") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto view = sbin.allocate(TestType{}, 2u);
            view[0] = test_format_specific::element_unit();
            view[1] = value_type{0};
            REQUIRE(view.at(0) != view.at(1));
            REQUIRE_FALSE(view.at(0) == view.at(1));
            view[1] = view[0];
            REQUIRE(view.at(0) == view.at(1));
            REQUIRE_FALSE(view.at(0) != view.at(1));
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::view<TestType const> validate_encoded{binary};
    }

    SECTION("support assignment between referenced bytes") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto view = sbin.allocate(TestType{}, 16u);
            std::fill(view.begin(), view.end(), test_format_specific::element_unit());
            *(view.end() - 2) = value_type{0};
            REQUIRE(view.byte_at(view.byte_size() - 2) != view.byte_at(view.byte_size() - 1));
            REQUIRE_FALSE(view.byte_at(view.byte_size() - 2) == view.byte_at(view.byte_size() - 1));
            view.byte_at(view.byte_size() - 2) = view.byte_at(view.byte_size() - 1);
            REQUIRE(view.byte_at(view.byte_size() - 2) == view.byte_at(view.byte_size() - 1));
            REQUIRE_FALSE(view.byte_at(view.byte_size() - 2) != view.byte_at(view.byte_size() - 1));
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::view<TestType const> validate_encoded{binary};
    }

    SECTION("fail to allocate unrepresentably large vectors") {
        using namespace builder::basic;
        // This checks that we can detect overlarge sizes and throw an exception.
        // Detailed checks for the size limit are delegated to Libbson (via libbson_length_for_append)
        REQUIRE_THROWS_WITH(
            make_document(kvp("vector", [&](sub_binary sbin) { sbin.allocate(TestType{}, SIZE_MAX); })),
            Catch::Matchers::ContainsSubstring("BSON vector too large"));
    }

    SECTION("support front and back element references") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto view = sbin.allocate(TestType{}, 2u);
            std::fill(view.begin(), view.end(), test_format_specific::element_unit());
            *(view.end() - 1) = value_type{0};
            REQUIRE(view.back() == value_type{0});
            REQUIRE(view.back() == view[view.size() - 1u]);
            REQUIRE(view.front() != view.back());
            REQUIRE_FALSE(view.front() == view.back());
            view.front() = view.back();
            REQUIRE(view[0] == value_type{0});
            REQUIRE(view.front() == view.back());
            REQUIRE_FALSE(view.front() != view.back());
            REQUIRE(view[view.size() - 1u] == value_type{0});
            view.back() = test_format_specific::element_unit();
            REQUIRE(view[0] == value_type{0});
            REQUIRE(view[view.size() - 1u] != value_type{0});
            REQUIRE(view.front() != view.back());
            REQUIRE_FALSE(view.front() == view.back());
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::view<TestType const> validate_encoded{binary};
    }

    SECTION("support front and back byte references") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto view = sbin.allocate(TestType{}, 16u);
            std::fill(view.begin(), view.end(), value_type{0});
            REQUIRE(view.front() == view.back());
            REQUIRE_FALSE(view.front() != view.back());
            REQUIRE(view.byte_front() == view.byte_back());
            REQUIRE_FALSE(view.byte_front() != view.byte_back());
            view.back() = test_format_specific::element_unit();
            REQUIRE(view.byte_front() != view.byte_back());
            REQUIRE_FALSE(view.byte_front() == view.byte_back());
            view.byte_front() = UINT8_C(0);
            view.byte_back() = UINT8_C(0);
            REQUIRE(view.byte_front() == view.byte_back());
            REQUIRE_FALSE(view.byte_front() != view.byte_back());
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::view<TestType const> validate_encoded{binary};
    }
}

TEST_CASE("vector view float32", "[bsoncxx::vector::view]") {
    SECTION("rejects binary data with an incorrect length") {
        static uint8_t const bytes[] = {0x27, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00};
        auto invalid_length = GENERATE(0u, 1u, 3u, 4u, 5u, 7u, 8u, 9u);
        types::b_binary const binary{binary_sub_type::k_vector, uint32_t(invalid_length), bytes};
        REQUIRE_THROWS_WITH(
            vector::view<vector::formats::f_float32 const>(binary),
            Catch::Matchers::ContainsSubstring("invalid BSON vector"));
    }

    SECTION("rejects binary data from other vector formats") {
        auto bytes = GENERATE(
            format_specific<vector::formats::f_int8>::bytes_empty(),
            format_specific<vector::formats::f_packed_bit>::bytes_empty());
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        REQUIRE_THROWS_WITH(
            vector::view<vector::formats::f_float32 const>(binary),
            Catch::Matchers::ContainsSubstring("invalid BSON vector"));
    }

    SECTION("accepts and correctly decodes elements with infinite value") {
        static uint8_t const bytes[] = {
            0x27, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x7F};
        types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
        vector::view<vector::formats::f_float32 const> view{binary};
        REQUIRE(view.size() == 3u);
        REQUIRE(view[0] < 0.f);
        REQUIRE(view[0] * 0.f != 0.f);
        REQUIRE(view[1] == 0.f);
        REQUIRE(view[2] > 0.f);
        REQUIRE(view[2] * 0.f != 0.f);
    }
}

TEST_CASE("vector view int8_t", "[bsoncxx::vector::view]") {
    SECTION("rejects binary data from other vector formats") {
        auto bytes = GENERATE(
            format_specific<vector::formats::f_float32>::bytes_empty(),
            format_specific<vector::formats::f_packed_bit>::bytes_empty());
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        REQUIRE_THROWS_WITH(
            vector::view<vector::formats::f_int8 const>(binary),
            Catch::Matchers::ContainsSubstring("invalid BSON vector"));
    }
}

TEST_CASE("vector view packed_bit", "[bsoncxx::vector::view]") {
    SECTION("rejects empty vectors with nonzero padding") {
        for (unsigned byte_value = 1u; byte_value <= UINT8_MAX; byte_value++) {
            auto bytes = format_specific<vector::formats::f_packed_bit>::bytes_empty();
            bytes[1] = uint8_t(byte_value);
            types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
            REQUIRE_THROWS_WITH(
                vector::view<vector::formats::f_packed_bit const>(binary),
                Catch::Matchers::ContainsSubstring("invalid BSON vector"));
        }
    }

    SECTION("rejects nonempty vectors with bits set in header padding byte") {
        for (unsigned byte_value = 8u; byte_value <= UINT8_MAX; byte_value++) {
            uint8_t const bytes[] = {0x10, uint8_t(byte_value), 0x00};
            types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
            REQUIRE_THROWS_WITH(
                vector::view<vector::formats::f_packed_bit const>(binary),
                Catch::Matchers::ContainsSubstring("invalid BSON vector"));
        }
    }

    SECTION("rejects nonempty vectors with nonzero values in unused trailing bits") {
        for (unsigned byte_value = 1u; byte_value <= 7u; byte_value++) {
            uint8_t bytes[] = {0x10, uint8_t(byte_value), 0xff};
            types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
            REQUIRE_THROWS_WITH(
                vector::view<vector::formats::f_packed_bit const>(binary),
                Catch::Matchers::ContainsSubstring("invalid BSON vector"));
            // Succeeds when unused bits are then zeroed
            bytes[2] = 0;
            vector::view<vector::formats::f_packed_bit const> view{binary};
            REQUIRE(view.size() == 8u - byte_value);
        }
    }

    SECTION("masks writes to unused portions of the last byte") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto view = sbin.allocate(vector::formats::f_packed_bit{}, 9u);
            std::fill(view.begin(), view.end(), true);
            REQUIRE(view.byte_size() == 2u);
            REQUIRE(view.byte_at(0) == 0xff);
            REQUIRE(view.byte_at(1) == 0x80);
            view.byte_at(1) = 0x7f;
            REQUIRE(view.at(7) == true);
            REQUIRE(view.at(8) == false);
            REQUIRE(view.byte_at(1) == 0x00);
            view.byte_at(1) = 0xff;
            view.byte_at(0) = 0xaa;
            REQUIRE(view.at(0) == true);
            REQUIRE(view.at(1) == false);
            REQUIRE(view.at(2) == true);
            REQUIRE(view.at(3) == false);
            REQUIRE(view.at(4) == true);
            REQUIRE(view.at(5) == false);
            REQUIRE(view.at(6) == true);
            REQUIRE(view.at(7) == false);
            REQUIRE(view.at(8) == true);
            REQUIRE(view.byte_at(0) == 0xaa);
            REQUIRE(view.byte_at(1) == 0x80);
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        REQUIRE(binary.sub_type == binary_sub_type::k_vector);
        std::array<std::uint8_t, 4> expected_bytes{0x10, 7, 0xaa, 0x80};
        binary_eq_bytes(binary, expected_bytes);
    }

    SECTION("validates nonempty vectors with any padding value") {
        for (unsigned byte_value = 0; byte_value <= 7; byte_value++) {
            uint8_t const bytes[] = {0x10, uint8_t(byte_value), 0x00};
            types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
            vector::view<vector::formats::f_packed_bit const> view{binary};
            REQUIRE(view.size() == 8 - byte_value);
        }
    }

    SECTION("rejects binary data from other vector formats") {
        auto bytes = GENERATE(
            format_specific<vector::formats::f_int8>::bytes_empty(),
            format_specific<vector::formats::f_float32>::bytes_empty());
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        REQUIRE_THROWS_WITH(
            vector::view<vector::formats::f_packed_bit const>(binary),
            Catch::Matchers::ContainsSubstring("invalid BSON vector"));
    }

    SECTION("writes and successfully re-validates vectors of any length") {
        for (std::size_t element_count = 0; element_count < 1000; element_count++) {
            using namespace builder::basic;
            bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
                auto view = sbin.allocate(vector::formats::f_packed_bit{}, element_count);
                REQUIRE(view.size() == element_count);
                REQUIRE(view.byte_size() == (element_count + 7u) / 8);
                std::fill(view.byte_begin(), view.byte_end(), UINT8_C(0xFF));
                REQUIRE(view.empty() == (element_count == 0));
                if (!view.empty()) {
                    std::for_each(view.byte_begin(), view.byte_end() - 1, [&](std::uint8_t value) {
                        REQUIRE(value == UINT8_C(0xFF));
                    });
                    std::size_t padding = view.byte_size() * std::size_t(8) - view.size();
                    REQUIRE(view.byte_back() == std::uint8_t(0xFF << padding));
                }
            }));
            types::b_binary const& binary = doc.view()["vector"].get_binary();
            REQUIRE(binary.sub_type == binary_sub_type::k_vector);
            vector::view<vector::formats::f_packed_bit const> view{binary};
            REQUIRE(view.size() == element_count);
            REQUIRE(view.byte_size() == (element_count + 7u) / 8);
            REQUIRE(view.empty() == (element_count == 0u));
            if (!view.empty()) {
                std::for_each(view.byte_begin(), view.byte_end() - 1, [&](std::uint8_t value) {
                    REQUIRE(value == UINT8_C(0xFF));
                });
                std::size_t padding = view.byte_size() * std::size_t(8) - view.size();
                REQUIRE(padding == binary.bytes[1]);
                REQUIRE(view.byte_back() == std::uint8_t(0xFF << padding));
            }
        }
    }
}

} // namespace
