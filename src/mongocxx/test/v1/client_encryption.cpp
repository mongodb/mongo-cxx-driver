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
// key_vault_clientations under the License.

#include <mongocxx/v1/client_encryption.hh>

//

#include <mongocxx/v1/client.hpp>
#include <mongocxx/v1/encrypt_options.hpp>
#include <mongocxx/v1/string_options.hpp>
#include <mongocxx/v1/text_options.hpp>

#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/exception.hh>

#include <mongocxx/test/private/scoped_bson.hh>

#include <cstdint>
#include <utility>

#include <bsoncxx/private/suppress_deprecation_warnings.hh>

#include <mongocxx/private/mock.hh>
#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace mongocxx {
namespace v1 {

namespace {

struct identity_type {};

} // namespace

TEST_CASE("ownership", "[mongocxx][v1][client_encryption]") {
    identity_type id1;
    identity_type id2;

    auto const ce1 = reinterpret_cast<mongoc_client_encryption_t*>(&id1);
    auto const ce2 = reinterpret_cast<mongoc_client_encryption_t*>(&id2);

    int destroy_count = 0;

    auto destroy = libmongoc::client_encryption_destroy.create_instance();
    destroy
        ->interpose([&](mongoc_client_encryption_t* ptr) -> void {
            if (ptr) {
                if (ptr != ce1 && ptr != ce2) {
                    FAIL_CHECK("unexpected mongoc_client_encryption_t");
                }
                ++destroy_count;
            }
        })
        .forever();

    auto source = client_encryption::internal::make(ce1);
    auto target = client_encryption::internal::make(ce2);

    REQUIRE(client_encryption::internal::as_mongoc(source) == ce1);
    REQUIRE(client_encryption::internal::as_mongoc(target) == ce2);

    SECTION("move") {
        {
            auto move = std::move(source);

            // source is in an assign-or-destroy-only state.

            CHECK(client_encryption::internal::as_mongoc(move) == ce1);
            CHECK(destroy_count == 0);

            target = std::move(move);

            // move is in an assign-or-destroy-only state.

            CHECK(client_encryption::internal::as_mongoc(target) == ce1);
            CHECK(destroy_count == 1);
        }

        CHECK(destroy_count == 1);
    }
}

TEST_CASE("ownership", "[mongocxx][v1][client_encryption][options]") {
    identity_type source_identity;
    identity_type target_identity;

    auto const source_value = reinterpret_cast<v1::client*>(&source_identity);
    auto const target_value = reinterpret_cast<v1::client*>(&target_identity);

    v1::client_encryption::options source;
    v1::client_encryption::options target;

    source.key_vault_client(source_value);
    target.key_vault_client(target_value);

    REQUIRE(source.key_vault_client() == source_value);
    REQUIRE(target.key_vault_client() == target_value);

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-destroy-only state.

        CHECK(move.key_vault_client() == source_value);

        target = std::move(move);

        // source is in an assign-or-destroy-only state.

        CHECK(target.key_vault_client() == source_value);
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.key_vault_client() == source_value);
        CHECK(copy.key_vault_client() == source_value);

        target = copy;

        CHECK(copy.key_vault_client() == source_value);
        CHECK(target.key_vault_client() == source_value);
    }
}

TEST_CASE("default", "[mongocxx][v1][client_encryption][options]") {
    client_encryption::options const opts;

    CHECK(opts.key_vault_client() == nullptr);
    CHECK_FALSE(opts.key_vault_namespace().has_value());
    CHECK_FALSE(opts.kms_providers().has_value());
    CHECK_FALSE(opts.tls_opts().has_value());
}

TEST_CASE("key_vault_client", "[mongocxx][v1][client_encryption][options]") {
    identity_type id1;
    identity_type id2;

    auto const client1 = reinterpret_cast<v1::client*>(&id1);
    auto const client2 = reinterpret_cast<v1::client*>(&id2);

    auto const v = GENERATE(false, true) ? client1 : client2;

    CHECK(client_encryption::options{}.key_vault_client(v).key_vault_client() == v);
}

TEST_CASE("key_vault_namespace", "[mongocxx][v1][client_encryption][options]") {
    auto const v = GENERATE(
        values<client_encryption::options::ns_pair>({
            {},
            {"db", "coll"},
        }));

    CHECK(client_encryption::options{}.key_vault_namespace(v).key_vault_namespace() == v);
}

TEST_CASE("kms_providers", "[mongocxx][v1][client_encryption][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"});

    CHECK(client_encryption::options{}.kms_providers(v.value()).kms_providers() == v.view());
}

TEST_CASE("tls_opts", "[mongocxx][v1][client_encryption][options]") {
    auto const v = GENERATE(scoped_bson{}, scoped_bson{R"({"x": 1})"});

    CHECK(client_encryption::options{}.tls_opts(v.value()).tls_opts() == v.view());
}

// `encrypt()` hands the "stringOpts" field to libmongoc. `string_opts` and the deprecated
// `text_opts` are distinct fields; when both are set, `string_opts` wins.
BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN

TEST_CASE("encrypt: string_opts and text_opts", "[mongocxx][v1][client_encryption]") {
    // Records what reaches libmongoc for the "stringOpts" field.
    struct recorder {
        int string_opts_new = 0;
        int set_string_opts = 0;
        bsoncxx::v1::stdx::optional<bool> case_sensitive;
        bsoncxx::v1::stdx::optional<std::int32_t> prefix_max_query_length;
        bsoncxx::v1::stdx::optional<std::int32_t> suffix_max_query_length;
        bsoncxx::v1::stdx::optional<std::int32_t> substring_max_length;
    } rec;

    identity_type id;
    auto const ce_ptr = reinterpret_cast<mongoc_client_encryption_t*>(&id);

    auto const destroy = libmongoc::client_encryption_destroy.create_instance();
    destroy->interpose([](mongoc_client_encryption_t*) {}).forever();

    // Leave the output value untouched; `encrypt()` returns it as-is.
    auto const encrypt = libmongoc::client_encryption_encrypt.create_instance();
    encrypt
        ->interpose([](mongoc_client_encryption_t*,
                       bson_value_t const*,
                       mongoc_client_encryption_encrypt_opts_t const*,
                       bson_value_t*,
                       bson_error_t*) { return true; })
        .forever();

    auto const opts_new = libmongoc::client_encryption_encrypt_opts_new.create_instance();
    opts_new->interpose([] { return reinterpret_cast<mongoc_client_encryption_encrypt_opts_t*>(1); }).forever();
    auto const opts_destroy = libmongoc::client_encryption_encrypt_opts_destroy.create_instance();
    opts_destroy->interpose([](mongoc_client_encryption_encrypt_opts_t*) {}).forever();

    auto const str_new = libmongoc::client_encryption_encrypt_string_opts_new.create_instance();
    str_new
        ->interpose([&] {
            ++rec.string_opts_new;
            return reinterpret_cast<mongoc_client_encryption_encrypt_string_opts_t*>(2);
        })
        .forever();
    auto const str_destroy = libmongoc::client_encryption_encrypt_string_opts_destroy.create_instance();
    str_destroy->interpose([](mongoc_client_encryption_encrypt_string_opts_t*) {}).forever();

    auto const set_string_opts = libmongoc::client_encryption_encrypt_opts_set_string_opts.create_instance();
    set_string_opts
        ->interpose([&](mongoc_client_encryption_encrypt_opts_t*,
                        mongoc_client_encryption_encrypt_string_opts_t const*) { ++rec.set_string_opts; })
        .forever();

    auto const set_cs = libmongoc::client_encryption_encrypt_string_opts_set_case_sensitive.create_instance();
    set_cs->interpose([&](mongoc_client_encryption_encrypt_string_opts_t*, bool v) { rec.case_sensitive = v; })
        .forever();

    auto const prefix_new = libmongoc::client_encryption_encrypt_string_prefix_opts_new.create_instance();
    prefix_new->interpose([] { return reinterpret_cast<mongoc_client_encryption_encrypt_string_prefix_opts_t*>(3); })
        .forever();
    auto const prefix_destroy = libmongoc::client_encryption_encrypt_string_prefix_opts_destroy.create_instance();
    prefix_destroy->interpose([](mongoc_client_encryption_encrypt_string_prefix_opts_t*) {}).forever();
    auto const prefix_set_max =
        libmongoc::client_encryption_encrypt_string_prefix_opts_set_str_max_query_length.create_instance();
    prefix_set_max
        ->interpose([&](mongoc_client_encryption_encrypt_string_prefix_opts_t*, std::int32_t v) {
            rec.prefix_max_query_length = v;
        })
        .forever();
    auto const set_prefix = libmongoc::client_encryption_encrypt_string_opts_set_prefix.create_instance();
    set_prefix
        ->interpose([](mongoc_client_encryption_encrypt_string_opts_t*,
                       mongoc_client_encryption_encrypt_string_prefix_opts_t const*) {})
        .forever();

    auto const suffix_new = libmongoc::client_encryption_encrypt_string_suffix_opts_new.create_instance();
    suffix_new->interpose([] { return reinterpret_cast<mongoc_client_encryption_encrypt_string_suffix_opts_t*>(4); })
        .forever();
    auto const suffix_destroy = libmongoc::client_encryption_encrypt_string_suffix_opts_destroy.create_instance();
    suffix_destroy->interpose([](mongoc_client_encryption_encrypt_string_suffix_opts_t*) {}).forever();
    auto const suffix_set_max =
        libmongoc::client_encryption_encrypt_string_suffix_opts_set_str_max_query_length.create_instance();
    suffix_set_max
        ->interpose([&](mongoc_client_encryption_encrypt_string_suffix_opts_t*, std::int32_t v) {
            rec.suffix_max_query_length = v;
        })
        .forever();
    auto const set_suffix = libmongoc::client_encryption_encrypt_string_opts_set_suffix.create_instance();
    set_suffix
        ->interpose([](mongoc_client_encryption_encrypt_string_opts_t*,
                       mongoc_client_encryption_encrypt_string_suffix_opts_t const*) {})
        .forever();

    auto const substring_new = libmongoc::client_encryption_encrypt_string_substring_opts_new.create_instance();
    substring_new
        ->interpose([] { return reinterpret_cast<mongoc_client_encryption_encrypt_string_substring_opts_t*>(5); })
        .forever();
    auto const substring_destroy = libmongoc::client_encryption_encrypt_string_substring_opts_destroy.create_instance();
    substring_destroy->interpose([](mongoc_client_encryption_encrypt_string_substring_opts_t*) {}).forever();
    auto const substring_set_max_len =
        libmongoc::client_encryption_encrypt_string_substring_opts_set_str_max_length.create_instance();
    substring_set_max_len
        ->interpose([&](mongoc_client_encryption_encrypt_string_substring_opts_t*, std::int32_t v) {
            rec.substring_max_length = v;
        })
        .forever();
    auto const set_substring = libmongoc::client_encryption_encrypt_string_opts_set_substring.create_instance();
    set_substring
        ->interpose([](mongoc_client_encryption_encrypt_string_opts_t*,
                       mongoc_client_encryption_encrypt_string_substring_opts_t const*) {})
        .forever();

    auto ce = client_encryption::internal::make(ce_ptr);
    auto const value = bsoncxx::v1::types::value{"plaintext"};

    auto const text = [] {
        return std::move(
            v1::text_options{}
                .case_sensitive(true)
                .suffix_opts(v1::text_options::suffix{}.str_max_query_length(11))
                .substring_opts(v1::text_options::substring{}.str_max_length(20)));
    };

    auto const string = [] {
        return std::move(
            v1::string_options{}.case_sensitive(false).prefix_opts(
                v1::string_options::prefix{}.str_max_query_length(10)));
    };

    SECTION("text_opts alone is passed through") {
        ce.encrypt(value, v1::encrypt_options{}.text_opts(text()));

        CHECK(rec.string_opts_new == 1);
        CHECK(rec.set_string_opts == 1);
        CHECK(rec.case_sensitive == true);
        CHECK(rec.suffix_max_query_length == 11);
        CHECK(rec.substring_max_length == 20);
        CHECK_FALSE(rec.prefix_max_query_length.has_value());
    }

    SECTION("string_opts alone is passed through") {
        ce.encrypt(value, v1::encrypt_options{}.string_opts(string()));

        CHECK(rec.string_opts_new == 1);
        CHECK(rec.set_string_opts == 1);
        CHECK(rec.case_sensitive == false);
        CHECK(rec.prefix_max_query_length == 10);
        CHECK_FALSE(rec.suffix_max_query_length.has_value());
        CHECK_FALSE(rec.substring_max_length.has_value());
    }

    SECTION("string_opts takes precedence over text_opts") {
        ce.encrypt(value, v1::encrypt_options{}.text_opts(text()).string_opts(string()));

        // Exactly one "stringOpts" is built, and it is the one from `string_opts`.
        CHECK(rec.string_opts_new == 1);
        CHECK(rec.set_string_opts == 1);
        CHECK(rec.case_sensitive == false);
        CHECK(rec.prefix_max_query_length == 10);

        // Nothing from `text_opts` leaks through.
        CHECK_FALSE(rec.suffix_max_query_length.has_value());
        CHECK_FALSE(rec.substring_max_length.has_value());
    }

    SECTION("neither set means no stringOpts at all") {
        ce.encrypt(value, v1::encrypt_options{});

        CHECK(rec.string_opts_new == 0);
        CHECK(rec.set_string_opts == 0);
    }
}

BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END

} // namespace v1
} // namespace mongocxx
