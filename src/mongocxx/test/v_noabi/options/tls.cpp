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

#include <mongocxx/options/tls.hpp>

//

#include <bsoncxx/test/v1/stdx/optional.hh>

#include <string>

#include <bsoncxx/test/catch.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {

TEST_CASE("v1", "[mongocxx][v_noabi][options][tls]") {
    auto const has_value = GENERATE(false, true);

    bsoncxx::v1::stdx::optional<std::string> pem_file;
    bsoncxx::v1::stdx::optional<std::string> pem_password;
    bsoncxx::v1::stdx::optional<std::string> ca_file;
    bsoncxx::v1::stdx::optional<std::string> ca_dir;
    bsoncxx::v1::stdx::optional<std::string> crl_file;
    bsoncxx::v1::stdx::optional<bool> allow_invalid_certificates;

    if (has_value) {
        pem_file.emplace();
        pem_password.emplace();
        ca_file.emplace();
        ca_dir.emplace();
        crl_file.emplace();
        allow_invalid_certificates.emplace();
    }

    using v_noabi = v_noabi::options::tls;
    using v1 = v1::tls;

    SECTION("from_v1") {
        v1 from;

        if (has_value) {
            from.pem_file(*pem_file);
            from.pem_password(*pem_password);
            from.ca_file(*ca_file);
            from.ca_dir(*ca_dir);
            from.crl_file(*crl_file);
            from.allow_invalid_certificates(*allow_invalid_certificates);
        }

        v_noabi const to{from};

        if (has_value) {
            CHECK(to.pem_file() == *pem_file);
            CHECK(to.pem_password() == *pem_password);
            CHECK(to.ca_file() == *ca_file);
            CHECK(to.ca_dir() == *ca_dir);
            CHECK(to.crl_file() == *crl_file);
            CHECK(to.allow_invalid_certificates() == *allow_invalid_certificates);
        } else {
            CHECK_FALSE(to.pem_file().has_value());
            CHECK_FALSE(to.pem_password().has_value());
            CHECK_FALSE(to.ca_file().has_value());
            CHECK_FALSE(to.ca_dir().has_value());
            CHECK_FALSE(to.crl_file().has_value());
            CHECK_FALSE(to.allow_invalid_certificates().has_value());
        }
    }

    SECTION("to_v1") {
        v_noabi from;

        if (has_value) {
            from.pem_file(*pem_file);
            from.pem_password(*pem_password);
            from.ca_file(*ca_file);
            from.ca_dir(*ca_dir);
            from.crl_file(*crl_file);
            from.allow_invalid_certificates(*allow_invalid_certificates);
        }

        v1 const to{from};

        if (has_value) {
            CHECK(to.pem_file() == *pem_file);
            CHECK(to.pem_password() == *pem_password);
            CHECK(to.ca_file() == *ca_file);
            CHECK(to.ca_dir() == *ca_dir);
            CHECK(to.crl_file() == *crl_file);
            CHECK(to.allow_invalid_certificates() == *allow_invalid_certificates);
        } else {
            CHECK_FALSE(to.pem_file().has_value());
            CHECK_FALSE(to.pem_password().has_value());
            CHECK_FALSE(to.ca_file().has_value());
            CHECK_FALSE(to.ca_dir().has_value());
            CHECK_FALSE(to.crl_file().has_value());
            CHECK_FALSE(to.allow_invalid_certificates().has_value());
        }
    }
}

} // namespace mongocxx
