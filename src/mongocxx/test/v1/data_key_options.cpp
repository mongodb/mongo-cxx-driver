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

#include <mongocxx/v1/data_key_options.hpp>

//

#include <mongocxx/v1/data_key_options.hh>

#include <bsoncxx/test/v1/document/value.hh>
#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>

#include <cstdint>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mock.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>

namespace mongocxx {
namespace v1 {

TEST_CASE("ownership", "[mongocxx][v1][data_key_options]") {
    auto const source_key = scoped_bson{R"({"provider": "source"})"};
    auto const target_key = scoped_bson{R"({"provider": "target"})"};

    data_key_options source;
    data_key_options target;

    source.master_key(source_key.value());
    target.master_key(target_key.value());

    SECTION("move") {
        auto move = std::move(source);

        // source is in an assign-or-move-only state.

        CHECK(move.master_key() == source_key.view());

        target = std::move(move);

        // source is in an assign-or-move-only state.

        CHECK(target.master_key() == source_key.view());
    }

    SECTION("copy") {
        auto copy = source;

        CHECK(source.master_key() == source_key.view());
        CHECK(copy.master_key() == source_key.view());

        target = copy;

        CHECK(copy.master_key() == source_key.view());
        CHECK(target.master_key() == source_key.view());
    }
}

TEST_CASE("default", "[mongocxx][v1][data_key_options]") {
    data_key_options const key;

    CHECK_FALSE(key.master_key().has_value());
    CHECK(key.key_alt_names().empty());
    CHECK_FALSE(key.key_material().has_value());
}

TEST_CASE("master_key", "[mongocxx][v1][data_key_options]") {
    auto const v = GENERATE(values({
        scoped_bson{},
        scoped_bson{R"({"provider": "key"})"},
    }));

    CHECK(data_key_options{}.master_key(v.value()).master_key() == v.view());
}

TEST_CASE("key_alt_names", "[mongocxx][v1][data_key_options]") {
    std::vector<std::string> v;

    std::tie(v) = GENERATE(
        table<std::vector<std::string>>({
            {},
            {{"x"}},
            {{"a", "b", "c"}},
        }));

    CHECK(data_key_options{}.key_alt_names(v).key_alt_names() == v);
}

TEST_CASE("key_material", "[mongocxx][v1][data_key_options]") {
    using type = data_key_options::key_material_type;

    type v;

    std::tie(v) = GENERATE(
        table<type>({
            {},
            {{1, 2, 3, 4, 5}},
        }));

    CHECK(data_key_options{}.key_material(v).key_material() == v);
}

TEST_CASE("to_mongoc", "[mongocxx][v1][data_key_options]") {
    data_key_options key;

    auto const master_key = scoped_bson{R"({" provider ": " master "})"};
    auto const key_alt_names = std::vector<std::string>({"a", "b", "c"});
    auto const key_material = std::vector<std::uint8_t>({1, 2, 3});

    key.master_key(master_key.value());
    key.key_alt_names(key_alt_names);
    key.key_material(key_material);

    auto const set_masterkey = libmongoc::client_encryption_datakey_opts_set_masterkey.create_instance();
    auto const set_keyaltnames = libmongoc::client_encryption_datakey_opts_set_keyaltnames.create_instance();
    auto const set_keymaterial = libmongoc::client_encryption_datakey_opts_set_keymaterial.create_instance();

    mongoc_client_encryption_datakey_opts_t* masterkey_ptr = nullptr;
    mongoc_client_encryption_datakey_opts_t* keyaltnames_ptr = nullptr;
    mongoc_client_encryption_datakey_opts_t* keymaterial_ptr = nullptr;

    set_masterkey->interpose([&](mongoc_client_encryption_datakey_opts_t* ptr, bson_t const* masterkey) {
        CHECK(ptr);
        masterkey_ptr = ptr;

        REQUIRE(masterkey);
        CHECK(scoped_bson_view{masterkey}.view() == master_key.view());
    });

    set_keyaltnames->interpose(
        [&](mongoc_client_encryption_datakey_opts_t* ptr, char** keyaltnames, std::uint32_t keyaltnames_count) {
            CHECK(ptr);
            keyaltnames_ptr = ptr;

            REQUIRE(keyaltnames);
            REQUIRE(keyaltnames_count == key_alt_names.size());

            for (std::uint32_t idx = 0u; idx < keyaltnames_count; ++idx) {
                auto const name = keyaltnames[idx];
                REQUIRE(name);
                CHECK(key_alt_names[idx] == name);
            }
        });

    set_keymaterial->interpose(
        [&](mongoc_client_encryption_datakey_opts_t* ptr, std::uint8_t const* data, std::uint32_t len) {
            CHECK(ptr);
            keymaterial_ptr = ptr;

            REQUIRE(data);
            REQUIRE(len == key_material.size());

            CHECK(std::vector<std::uint8_t>(data, data + len) == key_material);
        });

    auto const opts = v1::data_key_options::internal::to_mongoc(key);
    CHECK(opts.get() != nullptr);
    CHECK(opts.get() == masterkey_ptr);
    CHECK(opts.get() == keyaltnames_ptr);
    CHECK(opts.get() == keymaterial_ptr);
}

} // namespace v1
} // namespace mongocxx
