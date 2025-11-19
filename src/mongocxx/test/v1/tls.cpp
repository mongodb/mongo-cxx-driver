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

#include <mongocxx/v1/tls.hpp>

//

#include <string>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][tls]") {
    tls source;
    tls target;

    auto const source_value = "source";
    auto const target_value = "target";

    source.pem_file(source_value);
    target.pem_file(target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.pem_file() == source_value);

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.pem_file() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.pem_file() == source_value);
        CHECK(copy.pem_file() == source_value);

        target = copy;

        CHECK(copy.pem_file() == source_value);
        CHECK(target.pem_file() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][tls]") {
    tls const v;

    CHECK_FALSE(v.pem_file().has_value());
    CHECK_FALSE(v.pem_password().has_value());
    CHECK_FALSE(v.ca_file().has_value());
    CHECK_FALSE(v.ca_dir().has_value());
    CHECK_FALSE(v.crl_file().has_value());
    CHECK_FALSE(v.allow_invalid_certificates().has_value());
}

TEST_CASE("pem_file", "[mongocxx][v1][tls]") {
    auto const v = GENERATE(as<std::string>{}, std::string{}, "", "abc", "pem_file");

    CHECK(tls{}.pem_file(v).pem_file() == v);
}

TEST_CASE("pem_password", "[mongocxx][v1][tls]") {
    auto const v = GENERATE(as<std::string>{}, std::string{}, "", "abc", "pem_password");

    CHECK(tls{}.pem_password(v).pem_password() == v);
}

TEST_CASE("ca_file", "[mongocxx][v1][tls]") {
    auto const v = GENERATE(as<std::string>{}, std::string{}, "", "abc", "ca_file");

    CHECK(tls{}.ca_file(v).ca_file() == v);
}

TEST_CASE("ca_dir", "[mongocxx][v1][tls]") {
    auto const v = GENERATE(as<std::string>{}, std::string{}, "", "abc", "ca_dir");

    CHECK(tls{}.ca_dir(v).ca_dir() == v);
}

TEST_CASE("crl_file", "[mongocxx][v1][tls]") {
    auto const v = GENERATE(as<std::string>{}, std::string{}, "", "abc", "crl_file");

    CHECK(tls{}.crl_file(v).crl_file() == v);
}

TEST_CASE("allow_invalid_certificates", "[mongocxx][v1][tls]") {
    auto const v = GENERATE(false, true);

    CHECK(tls{}.allow_invalid_certificates(v).allow_invalid_certificates() == v);
}

} // namespace v1
} // namespace mongocxx
