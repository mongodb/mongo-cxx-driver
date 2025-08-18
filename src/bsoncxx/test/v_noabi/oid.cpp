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

#include <bsoncxx/oid.hpp>

//

#include <bsoncxx/v1/detail/bit.hpp>

#include <cstdint>
#include <cstring>
#include <ctime>
#include <string>

#include <bsoncxx/exception/error_code.hpp>

#include <bsoncxx/test/system_error.hh>

namespace {

using bsoncxx::v_noabi::oid;
using code = bsoncxx::v_noabi::error_code;

TEST_CASE("exceptions", "[bsoncxx][v_noabi][oid]") {
    char const zeroes[oid::k_oid_length] = {};

    CHECK_THROWS_WITH_CODE(oid{bsoncxx::stdx::string_view{}}, code::k_invalid_oid);
    CHECK_THROWS_WITH_CODE(oid{"invalid"}, code::k_invalid_oid);

    CHECK_THROWS_WITH_CODE((oid{zeroes, oid::k_oid_length - 1u}), code::k_invalid_oid);
    CHECK_NOTHROW((oid{zeroes, oid::k_oid_length + 0u}));
    CHECK_THROWS_WITH_CODE((oid{zeroes, oid::k_oid_length + 1u}), code::k_invalid_oid);
}

struct parsed_oid {
    std::uint32_t timestamp;
    std::uint64_t rand;
    std::uint32_t counter;
};

parsed_oid parse_oid(oid const& oid) {
    parsed_oid parsed{};

    // Parse into component sections
    auto const bytes = oid.bytes();
    std::memcpy(&parsed.timestamp, bytes, 4);
    std::memcpy(&parsed.rand, bytes + 1, 8);
    std::memcpy(&parsed.counter, bytes + 8, 4);

    if (bsoncxx::detail::endian::native != bsoncxx::detail::endian::big) {
#if defined(_WIN32)
        parsed.timestamp = _byteswap_ulong(parsed.timestamp);
        parsed.rand = _byteswap_uint64(parsed.rand) & 0x000000FFFFFFFFFF;
        parsed.counter = _byteswap_ulong(parsed.counter) & 0x00FFFFFF;
#else
        parsed.timestamp = __builtin_bswap32(parsed.timestamp);
        parsed.rand = __builtin_bswap64(parsed.rand) & 0x000000FFFFFFFFFF;
        parsed.counter = __builtin_bswap32(parsed.counter) & 0x00FFFFFF;
#endif
    }

    return parsed;
}

void compare_string(std::time_t const& t, bsoncxx::stdx::string_view time) {
    char time_str[48] = {};
    CHECK(0 != (std::strftime(time_str, sizeof(time_str), "%b %e, %Y %H:%M:%S UTC", std::gmtime(&t))));
    CHECK(time_str == time);
}

TEST_CASE("basic", "[bsoncxx][v_noabi][oid]") {
    SECTION("get_time_t") {
        oid const a{"000000000000000000000000"}; // 0x00000000: "Jan 1st, 1970 00:00:00 UTC"
        oid const b{"7FFFFFFF0000000000000000"}; // 0x7FFFFFFF: "Jan 19th, 2038 03:14:07 UTC"
        oid const c{"800000000000000000000000"}; // 0x80000000: "Jan 19th, 2038 03:14:08 UTC"
        oid const d{"FFFFFFFF0000000000000000"}; // 0xFFFFFFFF: "Feb 7th, 2106 06:28:15 UTC"

        auto const ta = a.get_time_t();
        auto const tb = b.get_time_t();
        auto const tc = c.get_time_t();
        auto const td = d.get_time_t();

        REQUIRE(ta == 0x00000000);
        REQUIRE(tb == 0x7FFFFFFF);
        REQUIRE(tc == 0x80000000);
        REQUIRE(td == 0xFFFFFFFF);

        compare_string(ta, "Jan  1, 1970 00:00:00 UTC");
        compare_string(tb, "Jan 19, 2038 03:14:07 UTC");
        compare_string(tc, "Jan 19, 2038 03:14:08 UTC");
        compare_string(td, "Feb  7, 2106 06:28:15 UTC");
    }

    // Ensure that after a new process is created through a fork() or similar process creation operation, the "random
    // number unique to a machine and process" is no longer the same as the parent process that created the new process.
    // Defer testing multi-process (fork) behavior to the C Driver.
    SECTION("rand and counter") {
        oid oid1;
        oid oid2;

        auto const parsed1 = parse_oid(oid1);
        auto const parsed2 = parse_oid(oid2);

        CHECK(parsed1.rand == parsed2.rand);
        CHECK(parsed2.counter == parsed1.counter + 1u);
    }

    SECTION("to_string") {
        char const zeroes[oid::k_oid_length]{};
        oid o{zeroes, sizeof(zeroes)};
        CHECK((oid{zeroes, sizeof(zeroes)}).to_string() == "000000000000000000000000");
    }

    SECTION("comparison") {
        // Timestamp: 946771199 (0x386e94ff)
        // Value:     286462997 (0x11131415)
        // Counter:   2171427   (0x212223)
        oid const o{"386e94ff1112131415212223"};

        CHECK(o == o);
        CHECK_FALSE(o != o);
        CHECK_FALSE(o > o);
        CHECK_FALSE(o < o);
        CHECK(o >= o);
        CHECK(o <= o);

        {
            std::time_t time = o.get_time_t();
            char str[sizeof("YYYY-MM-DD HH:MM:SS")];
            CHECK(std::strftime(str, sizeof(str), "%F %T", std::gmtime(&time)) == sizeof(str) - 1u);
            CHECK(std::string(str) == "2000-01-01 23:59:59");
        }

        CHECK(o < oid{"389622001112131415212223"}); // Timestamp: 2000-02-01 00:00:00
        CHECK(o > oid{"386d43801112131415212223"}); // Timestamp: 2000-01-01 00:00:00
        CHECK(o < oid{"386e94ffffffffffff212223"}); // Value: 1099511627775
        CHECK(o > oid{"386e94ff0000000000212223"}); // Value: 0
        CHECK(o < oid{"386e94ff1112131415ffffff"}); // Counter: 16777215
        CHECK(o > oid{"386e94ff1112131415000000"}); // Counter: 0
    }
}

TEST_CASE("v1", "[bsoncxx][v_noabi][oid]") {
    using v1 = bsoncxx::v1::oid;
    using v_noabi = bsoncxx::v_noabi::oid;
    using bsoncxx::v_noabi::from_v1;
    using bsoncxx::v_noabi::to_v1;

    SECTION("from_v1") {
        v1 from{"111111111111111111111111"};
        v_noabi const to = from;

        CHECK(to == to_v1(to));
        CHECK(to == from);
        CHECK(to != v1{"000000000000000000000000"});
    }

    SECTION("to_v1") {
        v_noabi from{"111111111111111111111111"};
        v1 const to{from};

        CHECK(to == from_v1(to));
        CHECK(to == from);
        CHECK(to != v_noabi{"000000000000000000000000"});
    }
}

} // namespace
