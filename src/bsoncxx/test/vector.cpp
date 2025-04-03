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

#include <algorithm>
#include <array>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/sub_binary.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/vector/accessor.hpp>
#include <bsoncxx/vector/formats.hpp>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

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
    CHECK(std::memcmp(binary.bytes, bytes.data(), bytes.size()) == 0);
}

template <typename Iterator, typename Element>
void iterator_operations(
    Iterator const& begin,
    Iterator const& end,
    std::ptrdiff_t expected_size,
    Element element_unit) {
    CHECK(end - begin == expected_size);

    for (std::ptrdiff_t outer_index = 0; outer_index < 50; outer_index++) {
        Iterator outer_front = begin + outer_index;
        Iterator outer_back = end - outer_index;

        CHECK(outer_front - begin == outer_index);
        CHECK(begin - outer_front == -outer_index);

        CHECK(end - outer_back == outer_index);
        CHECK(outer_back - end == -outer_index);

        for (std::ptrdiff_t inner_index = 0; inner_index < 20; inner_index++) {
            Iterator inner_front = outer_front + inner_index;
            Iterator inner_back = outer_back - inner_index;

            CHECK(inner_front - outer_front == inner_index);
            CHECK(outer_front - inner_front == -inner_index);

            CHECK(outer_back - inner_back == inner_index);
            CHECK(inner_back - outer_back == -inner_index);
        }
    }

    Iterator iter_copy = begin;
    CHECK(iter_copy == begin);
    CHECK(iter_copy >= begin);
    CHECK(iter_copy <= begin);
    CHECK_FALSE(iter_copy != begin);
    CHECK_FALSE(iter_copy < begin);

    CHECK(++iter_copy - begin == 1);
    CHECK(iter_copy > begin);
    CHECK(iter_copy >= begin);
    CHECK(iter_copy != begin);
    CHECK_FALSE(iter_copy == begin);
    CHECK_FALSE(iter_copy <= begin);
    CHECK_FALSE(iter_copy < begin);

    CHECK(--iter_copy - begin == 0);
    CHECK(iter_copy == begin);

    CHECK(iter_copy++ - begin == 0);
    CHECK(iter_copy-- - begin == 1);
    CHECK(iter_copy == begin);

    BSONCXX_PRIVATE_WARNINGS_PUSH();
    BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));

    std::generate(begin, end, [&] { return element_unit; });
    std::for_each(begin, end, [&](auto const& value) { CHECK(value == element_unit); });

    std::copy(begin, begin + (expected_size / 2), begin + (expected_size / 2));
    std::for_each(begin, end, [&](auto const& value) { CHECK(value == element_unit); });

    std::for_each(begin, end, [&](auto&& value) { value = element_unit; });
    std::for_each(begin, end, [&](auto const& value) { CHECK(value == element_unit); });

    std::fill(begin, end, element_unit);
    std::for_each(begin, end, [&](auto const& value) { CHECK(value == element_unit); });

    std::sort(begin, end);
    std::for_each(begin, end, [&](auto const& value) { CHECK(value == element_unit); });

    // Verify ADL chooses our swap() even when std::swap is in scope
    *begin = Element{0};
    CHECK(*begin < *(end - 1));
    CHECK_FALSE(*(end - 1) < *begin);
    {
        using std::swap;
        swap(*begin, *(end - 1));
    }
    CHECK(*(end - 1) < *begin);
    CHECK_FALSE(*begin < *(end - 1));
    *begin = element_unit;

    // Sort, check that a 0 moves from the back to the front.
    *(end - 1) = Element{0};
    std::sort(begin, end);
    CHECK(*begin == Element{0});
    std::for_each(begin + 1, end, [&](auto const& value) { CHECK(value == element_unit); });

    std::for_each(begin, end, [&](auto&& value) { value = Element{0}; });
    std::for_each(begin, end, [&](auto const& value) { CHECK(value != element_unit); });
    std::for_each(begin, end, [&](auto const& value) { CHECK(value < element_unit); });
    std::for_each(begin, end, [&](auto const& value) { CHECK(value <= element_unit); });
    std::for_each(begin, end, [&](auto const& value) { CHECK_FALSE(value == element_unit); });
    std::for_each(begin, end, [&](auto const& value) { CHECK_FALSE(value > element_unit); });
    std::for_each(begin, end, [&](auto const& value) { CHECK_FALSE(value >= element_unit); });

    BSONCXX_PRIVATE_WARNINGS_POP();
}

TEMPLATE_TEST_CASE(
    "all vector accessor formats",
    "[bsoncxx::vector::accessor]",
    vector::formats::f_float32,
    vector::formats::f_int8,
    vector::formats::f_packed_bit) {
    using test_format_specific = format_specific<TestType>;
    using value_type = typename test_format_specific::value_type;

    SECTION("accept a valid vector with no elements") {
        auto bytes = test_format_specific::bytes_empty();
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        vector::accessor<TestType const> vec{binary};
        CHECK(vec.empty());
        CHECK(vec.size() == 0);
        CHECK(vec.byte_size() == 0);
        CHECK_THROWS_WITH_CODE(vec.at(0), bsoncxx::v_noabi::error_code::k_vector_out_of_range);
        CHECK_THROWS_WITH_CODE(vec.byte_at(0), bsoncxx::v_noabi::error_code::k_vector_out_of_range);
    }

    SECTION("decode a valid vector with a single element") {
        auto bytes = test_format_specific::bytes_unit();
        auto element = test_format_specific::element_unit();
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        vector::accessor<TestType const> vec{binary};
        CHECK_FALSE(vec.empty());
        CHECK(vec.size() == 1u);
        CHECK(vec.byte_size() == bytes.size() - 2u);

        BSONCXX_PRIVATE_WARNINGS_PUSH();
        BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));

        CHECK(vec.at(0) == element);
        CHECK(vec[0] == element);

        BSONCXX_PRIVATE_WARNINGS_POP();

        CHECK(vec.byte_at(0) == bytes[2]);
        CHECK(vec.byte_at(bytes.size() - 3u) == bytes[bytes.size() - 1u]);
        CHECK_THROWS_WITH_CODE(vec.at(1), bsoncxx::v_noabi::error_code::k_vector_out_of_range);
        CHECK_THROWS_WITH_CODE(vec.byte_at(bytes.size() - 2u), bsoncxx::v_noabi::error_code::k_vector_out_of_range);
    }

    SECTION("reject binary data of the wrong sub_type") {
        auto bytes = test_format_specific::bytes_empty();
        auto invalid_type = GENERATE(
            binary_sub_type::k_binary, binary_sub_type::k_encrypted, binary_sub_type::k_uuid, binary_sub_type::k_user);
        types::b_binary const binary{invalid_type, bytes.size(), bytes.data()};
        CHECK_THROWS_WITH_CODE(
            vector::accessor<TestType const>{binary}, bsoncxx::v_noabi::error_code::k_invalid_vector);
    }

    SECTION("reject binary data that's too short to include a header") {
        auto bytes = test_format_specific::bytes_empty();
        auto bytes_to_remove = GENERATE(1u, 2u);
        REQUIRE(bytes.size() >= bytes_to_remove);
        types::b_binary const binary{binary_sub_type::k_vector, uint32_t(bytes.size() - bytes_to_remove), bytes.data()};
        CHECK_THROWS_WITH_CODE(
            vector::accessor<TestType const>{binary}, bsoncxx::v_noabi::error_code::k_invalid_vector);
    }

    SECTION("reject empty vectors with any modified header bits") {
        for (unsigned bit_index = 0; bit_index < 16; bit_index++) {
            auto bytes = test_format_specific::bytes_empty();
            bytes[bit_index >> 3u] ^= std::uint8_t(1u << (bit_index & 7u));
            types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
            CHECK_THROWS_WITH_CODE(
                vector::accessor<TestType const>{binary}, bsoncxx::v_noabi::error_code::k_invalid_vector);
        }
    }

    SECTION("encode a single element as expected") {
        using namespace builder::basic;
        auto expected_bytes = test_format_specific::bytes_unit();
        auto element = test_format_specific::element_unit();
        bsoncxx::document::value doc =
            make_document(kvp("vector", [&](sub_binary sbin) { sbin.allocate(TestType{}, 1u)[0u] = element; }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        CHECK(binary.sub_type == binary_sub_type::k_vector);
        binary_eq_bytes(binary, expected_bytes);
        vector::accessor<TestType const> validate_encoded{binary};
    }

    SECTION("support algorithms and operators on element iterators") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            // Avoid multiples of 8, to cover nonzero packed_bit 'padding'.
            auto vec = sbin.allocate(TestType{}, 8007u);
            auto const element_unit = test_format_specific::element_unit();
            iterator_operations(vec.begin(), vec.end(), std::ptrdiff_t(vec.size()), element_unit);

            // Two ways of iterating as const
            BSONCXX_PRIVATE_WARNINGS_PUSH();
            BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));
            std::for_each(vec.cbegin(), vec.cend(), [&](auto const& value) { CHECK_FALSE(value == element_unit); });
            std::for_each(vec.as_const().begin(), vec.as_const().end(), [&](auto const& value) {
                CHECK_FALSE(value == element_unit);
            });
            BSONCXX_PRIVATE_WARNINGS_POP();
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::accessor<TestType const> validate_encoded{binary};
        CHECK(binary.size > 1000u);
    }

    SECTION("support algorithms and operators on byte iterators") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            // Choose a multiple of 8, to avoid the effects of masking unused bits.
            auto vec = sbin.allocate(TestType{}, 8000u);
            uint8_t const element_unit(1);
            iterator_operations(vec.byte_begin(), vec.byte_end(), std::ptrdiff_t(vec.byte_size()), element_unit);

            // Two ways of iterating as const
            std::for_each(
                vec.byte_cbegin(), vec.byte_cend(), [&](auto const& value) { CHECK_FALSE(value == element_unit); });
            std::for_each(vec.as_const().byte_begin(), vec.as_const().byte_end(), [&](auto const& value) {
                CHECK_FALSE(value == element_unit);
            });
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::accessor<TestType const> validate_encoded{binary};
        CHECK(binary.size > 1000u);
    }

    SECTION("support assignment between referenced elements") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto vec = sbin.allocate(TestType{}, 2u);
            vec[0] = test_format_specific::element_unit();
            vec[1] = value_type{0};

            BSONCXX_PRIVATE_WARNINGS_PUSH();
            BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));

            CHECK(vec.at(0) != vec.at(1));
            CHECK_FALSE(vec.at(0) == vec.at(1));
            vec[1] = vec[0];
            CHECK(vec.at(0) == vec.at(1));
            CHECK_FALSE(vec.at(0) != vec.at(1));

            BSONCXX_PRIVATE_WARNINGS_POP();
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::accessor<TestType const> validate_encoded{binary};
    }

    SECTION("support assignment between referenced bytes") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto vec = sbin.allocate(TestType{}, 16u);
            std::fill(vec.begin(), vec.end(), test_format_specific::element_unit());
            *(vec.end() - 2) = value_type{0};
            CHECK(vec.byte_at(vec.byte_size() - 2) != vec.byte_at(vec.byte_size() - 1));
            CHECK_FALSE(vec.byte_at(vec.byte_size() - 2) == vec.byte_at(vec.byte_size() - 1));
            vec.byte_at(vec.byte_size() - 2) = vec.byte_at(vec.byte_size() - 1);
            CHECK(vec.byte_at(vec.byte_size() - 2) == vec.byte_at(vec.byte_size() - 1));
            CHECK_FALSE(vec.byte_at(vec.byte_size() - 2) != vec.byte_at(vec.byte_size() - 1));
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::accessor<TestType const> validate_encoded{binary};
    }

    SECTION("fail to allocate unrepresentably large vectors") {
        using namespace builder::basic;
        // This checks that we can detect overlarge sizes and throw an exception.
        // Detailed checks for the size limit are delegated to Libbson (via libbson_length_for_append)
        CHECK_THROWS_WITH_CODE(
            make_document(kvp("vector", [&](sub_binary sbin) { sbin.allocate(TestType{}, SIZE_MAX); })),
            bsoncxx::v_noabi::error_code::k_vector_too_large);
    }

    SECTION("support front and back element references") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto vec = sbin.allocate(TestType{}, 2u);
            std::fill(vec.begin(), vec.end(), test_format_specific::element_unit());
            *(vec.end() - 1) = value_type{0};

            BSONCXX_PRIVATE_WARNINGS_PUSH();
            BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));

            CHECK(vec.back() == value_type{0});
            CHECK(vec.back() == vec[vec.size() - 1u]);
            CHECK(vec.front() != vec.back());
            CHECK_FALSE(vec.front() == vec.back());
            vec.front() = vec.back();
            CHECK(vec[0] == value_type{0});
            CHECK(vec.front() == vec.back());
            CHECK_FALSE(vec.front() != vec.back());
            CHECK(vec[vec.size() - 1u] == value_type{0});
            vec.back() = test_format_specific::element_unit();
            CHECK(vec[0] == value_type{0});
            CHECK(vec[vec.size() - 1u] != value_type{0});
            CHECK(vec.front() != vec.back());
            CHECK_FALSE(vec.front() == vec.back());

            BSONCXX_PRIVATE_WARNINGS_POP();
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::accessor<TestType const> validate_encoded{binary};
    }

    SECTION("support front and back byte references") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto vec = sbin.allocate(TestType{}, 16u);
            std::fill(vec.begin(), vec.end(), value_type{0});

            BSONCXX_PRIVATE_WARNINGS_PUSH();
            BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));

            CHECK(vec.front() == vec.back());
            CHECK_FALSE(vec.front() != vec.back());
            CHECK(vec.byte_front() == vec.byte_back());
            CHECK_FALSE(vec.byte_front() != vec.byte_back());
            vec.back() = test_format_specific::element_unit();
            CHECK(vec.byte_front() != vec.byte_back());
            CHECK_FALSE(vec.byte_front() == vec.byte_back());
            vec.byte_front() = UINT8_C(0);
            vec.byte_back() = UINT8_C(0);
            CHECK(vec.byte_front() == vec.byte_back());
            CHECK_FALSE(vec.byte_front() != vec.byte_back());

            BSONCXX_PRIVATE_WARNINGS_POP();
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        vector::accessor<TestType const> validate_encoded{binary};
    }
}

TEST_CASE("vector accessor float32", "[bsoncxx::vector::accessor]") {
    SECTION("rejects binary data with an incorrect length") {
        static uint8_t const bytes[] = {0x27, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00};
        auto invalid_length = GENERATE(0u, 1u, 3u, 4u, 5u, 7u, 8u, 9u);
        types::b_binary const binary{binary_sub_type::k_vector, uint32_t(invalid_length), bytes};
        CHECK_THROWS_WITH_CODE(
            vector::accessor<vector::formats::f_float32 const>{binary}, bsoncxx::v_noabi::error_code::k_invalid_vector);
    }

    SECTION("rejects binary data from other vector formats") {
        auto bytes = GENERATE(
            format_specific<vector::formats::f_int8>::bytes_empty(),
            format_specific<vector::formats::f_packed_bit>::bytes_empty());
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        CHECK_THROWS_WITH_CODE(
            vector::accessor<vector::formats::f_float32 const>{binary}, bsoncxx::v_noabi::error_code::k_invalid_vector);
    }

    SECTION("accepts and correctly decodes elements with infinite value") {
        static uint8_t const bytes[] = {
            0x27, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x7F};
        types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
        vector::accessor<vector::formats::f_float32 const> vec{binary};
        REQUIRE(vec.size() == 3u);

        BSONCXX_PRIVATE_WARNINGS_PUSH();
        BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));

        CHECK(vec[0] < 0.f);
        CHECK(vec[0] * 0.f != 0.f);
        CHECK(vec[1] == 0.f);
        CHECK(vec[2] > 0.f);
        CHECK(vec[2] * 0.f != 0.f);

        BSONCXX_PRIVATE_WARNINGS_POP();
    }
}

TEST_CASE("vector accessor int8_t", "[bsoncxx::vector::accessor]") {
    SECTION("rejects binary data from other vector formats") {
        auto bytes = GENERATE(
            format_specific<vector::formats::f_float32>::bytes_empty(),
            format_specific<vector::formats::f_packed_bit>::bytes_empty());
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        CHECK_THROWS_WITH_CODE(
            vector::accessor<vector::formats::f_int8 const>{binary}, bsoncxx::v_noabi::error_code::k_invalid_vector);
    }
}

TEST_CASE("vector accessor packed_bit", "[bsoncxx::vector::accessor]") {
    SECTION("rejects empty vectors with nonzero padding") {
        for (unsigned byte_value = 1u; byte_value <= UINT8_MAX; byte_value++) {
            auto bytes = format_specific<vector::formats::f_packed_bit>::bytes_empty();
            bytes[1] = uint8_t(byte_value);
            types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
            CHECK_THROWS_WITH_CODE(
                vector::accessor<vector::formats::f_packed_bit const>{binary},
                bsoncxx::v_noabi::error_code::k_invalid_vector);
        }
    }

    SECTION("rejects nonempty vectors with reserved values in header padding byte") {
        for (unsigned byte_value = 8u; byte_value <= UINT8_MAX; byte_value++) {
            uint8_t const bytes[] = {0x10, uint8_t(byte_value), 0x00};
            types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
            CHECK_THROWS_WITH_CODE(
                vector::accessor<vector::formats::f_packed_bit const>{binary},
                bsoncxx::v_noabi::error_code::k_invalid_vector);
        }
    }

    SECTION("rejects nonempty vectors with nonzero values in unused trailing bits") {
        for (unsigned byte_value = 1u; byte_value <= 7u; byte_value++) {
            uint8_t bytes[] = {0x10, uint8_t(byte_value), 0xff};
            types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
            CHECK_THROWS_WITH_CODE(
                vector::accessor<vector::formats::f_packed_bit const>{binary},
                bsoncxx::v_noabi::error_code::k_invalid_vector);
            // Succeeds when unused bits are then zeroed
            bytes[2] = 0;
            vector::accessor<vector::formats::f_packed_bit const> vec{binary};
            CHECK(vec.size() == 8u - byte_value);
        }
    }

    SECTION("masks writes to unused portions of the last byte") {
        using namespace builder::basic;
        bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
            auto vec = sbin.allocate(vector::formats::f_packed_bit{}, 9u);
            std::fill(vec.begin(), vec.end(), true);
            std::for_each(vec.begin(), vec.end(), [&](bool value) { CHECK(value == true); });
            std::for_each(vec.cbegin(), vec.cend(), [&](bool value) { CHECK(value == true); });
            CHECK(vec.byte_size() == 2u);
            CHECK(vec.byte_at(0) == 0xff);
            CHECK(vec.byte_at(1) == 0x80);
            vec.byte_at(1) = 0x7f;
            CHECK(vec.at(7) == true);
            CHECK(vec.at(8) == false);
            CHECK(vec.byte_at(1) == 0x00);
            vec.byte_at(1) = 0xff;
            vec.byte_at(0) = 0xaa;
            CHECK(vec.at(0) == true);
            CHECK(vec.at(1) == false);
            CHECK(vec.at(2) == true);
            CHECK(vec.at(3) == false);
            CHECK(vec.at(4) == true);
            CHECK(vec.at(5) == false);
            CHECK(vec.at(6) == true);
            CHECK(vec.at(7) == false);
            CHECK(vec.at(8) == true);
            CHECK(vec.byte_at(0) == 0xaa);
            CHECK(vec.byte_at(1) == 0x80);
        }));
        types::b_binary const& binary = doc.view()["vector"].get_binary();
        CHECK(binary.sub_type == binary_sub_type::k_vector);
        std::array<std::uint8_t, 4> expected_bytes{0x10, 7, 0xaa, 0x80};
        binary_eq_bytes(binary, expected_bytes);
    }

    SECTION("validates nonempty vectors with any padding value") {
        for (unsigned byte_value = 0; byte_value <= 7; byte_value++) {
            uint8_t const bytes[] = {0x10, uint8_t(byte_value), 0x00};
            types::b_binary const binary{binary_sub_type::k_vector, sizeof bytes, bytes};
            vector::accessor<vector::formats::f_packed_bit const> vec{binary};
            CHECK(vec.size() == 8 - byte_value);
        }
    }

    SECTION("rejects binary data from other vector formats") {
        auto bytes = GENERATE(
            format_specific<vector::formats::f_int8>::bytes_empty(),
            format_specific<vector::formats::f_float32>::bytes_empty());
        types::b_binary const binary{binary_sub_type::k_vector, bytes.size(), bytes.data()};
        CHECK_THROWS_WITH_CODE(
            vector::accessor<vector::formats::f_packed_bit const>{binary},
            bsoncxx::v_noabi::error_code::k_invalid_vector);
    }

    SECTION("writes and successfully re-validates vectors of any length") {
        for (std::size_t element_count = 0; element_count < 1000; element_count++) {
            using namespace builder::basic;
            bsoncxx::document::value doc = make_document(kvp("vector", [&](sub_binary sbin) {
                auto vec = sbin.allocate(vector::formats::f_packed_bit{}, element_count);
                REQUIRE(vec.size() == element_count);
                REQUIRE(vec.byte_size() == (element_count + 7u) / 8);
                std::fill(vec.byte_begin(), vec.byte_end(), UINT8_C(0xFF));
                CHECK(vec.empty() == (element_count == 0));
                if (!vec.empty()) {
                    std::for_each(vec.byte_begin(), vec.byte_end() - 1, [&](std::uint8_t value) {
                        CHECK(value == UINT8_C(0xFF));
                    });
                    std::for_each(vec.byte_cbegin(), vec.byte_cend() - 1, [&](std::uint8_t value) {
                        CHECK(value == UINT8_C(0xFF));
                    });
                    std::size_t padding = vec.byte_size() * std::size_t(8) - vec.size();
                    CHECK(vec.byte_back() == std::uint8_t(0xFF << padding));
                }
            }));
            types::b_binary const& binary = doc.view()["vector"].get_binary();
            CHECK(binary.sub_type == binary_sub_type::k_vector);
            vector::accessor<vector::formats::f_packed_bit const> vec{binary};
            REQUIRE(vec.size() == element_count);
            REQUIRE(vec.byte_size() == (element_count + 7u) / 8);
            CHECK(vec.empty() == (element_count == 0u));
            if (!vec.empty()) {
                std::for_each(
                    vec.byte_begin(), vec.byte_end() - 1, [&](std::uint8_t value) { CHECK(value == UINT8_C(0xFF)); });
                std::for_each(
                    vec.byte_cbegin(), vec.byte_cend() - 1, [&](std::uint8_t value) { CHECK(value == UINT8_C(0xFF)); });
                std::size_t padding = vec.byte_size() * std::size_t(8) - vec.size();
                CHECK(padding == binary.bytes[1]);
                CHECK(vec.byte_back() == std::uint8_t(0xFF << padding));
            }
        }
    }
}

} // namespace
