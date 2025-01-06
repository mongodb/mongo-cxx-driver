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

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types/bson_value/make_value.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/client_encryption.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/auto_encryption.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/data_key.hpp>
#include <mongocxx/options/encrypt.hpp>

#include <examples/macros.hh>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using bsoncxx::types::bson_value::make_value;

using namespace mongocxx;

namespace {

int const kKeyLength = 96;

using ns_pair = std::pair<std::string, std::string>;
void create_json_schema_file(
    bsoncxx::document::view_or_value kms_providers,
    ns_pair key_vault_ns,
    class client* key_vault_client) {
    options::client_encryption client_encryption_opts{};
    client_encryption_opts.key_vault_namespace(std::move(key_vault_ns));
    client_encryption_opts.kms_providers(kms_providers);
    client_encryption_opts.key_vault_client(key_vault_client);

    class client_encryption client_encryption{std::move(client_encryption_opts)};

    // Create a new data key and json schema for the encryptedField.
    auto data_key_id = client_encryption.create_data_key("local");

    // Create a new json schema for the encryptedField.
    auto json_schema = document{} << "properties" << open_document << "encryptedField" << open_document << "encrypt"
                                  << open_document << "keyId" << open_array << data_key_id << close_array << "bsonType"
                                  << "string"
                                  << "algorithm"
                                  << "AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic" << close_document << close_document
                                  << close_document << "bsonType"
                                  << "object" << finalize;

    // Write to a file.
    std::ofstream schema_file;
    schema_file.open("jsonSchema.json");
    schema_file << bsoncxx::to_json(json_schema);
    schema_file.close();
}

bsoncxx::document::value doc_from_file(std::string path) {
    std::ifstream file{path};
    if (!file) {
        throw std::runtime_error("could not open file");
    }

    std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return bsoncxx::from_json(file_contents);
}

} // namespace

int EXAMPLES_CDECL main() {
    instance inst{};

    // This must be the same master key that was used to create
    // the encryption key; here, we use a random key as a placeholder.
    std::uint8_t key_storage[kKeyLength];
    std::generate_n(key_storage, kKeyLength, []() { return static_cast<std::uint8_t>(std::rand()); });
    bsoncxx::types::b_binary local_master_key{bsoncxx::binary_sub_type::k_binary, kKeyLength, key_storage};

    auto kms_providers = document{} << "local" << open_document << "key" << local_master_key << close_document
                                    << finalize;

    // The MongoClient used to access the key vault.
    class client key_vault_client{uri{}};
    auto key_vault = key_vault_client["keyvault"]["datakeys"];

    // Ensure that two data keys cannot share the same keyAltName by adding
    // a unique index on this field.
    key_vault.drop();

    mongocxx::options::index index_options{};
    index_options.unique(true);
    auto expression = document{} << "keyAltNames" << open_document << "$exists" << true << close_document << finalize;
    index_options.partial_filter_expression(expression.view());
    key_vault.create_index(make_document(kvp("keyAltNames", 1)), index_options);

    create_json_schema_file(kms_providers.view(), {"keyvault", "datakeys"}, &key_vault_client);

    // Load the JSON Schema and construct the local schema_map option.
    auto json_schema = doc_from_file("jsonSchema.json");
    auto schema_map = make_document(kvp("test.coll", json_schema.view()));

    options::auto_encryption auto_encrypt_opts{};
    auto_encrypt_opts.kms_providers(kms_providers.view());
    auto_encrypt_opts.key_vault_namespace({"keyvault", "datakeys"});
    auto_encrypt_opts.schema_map({schema_map.view()});

    options::client client_opts;
    client_opts.auto_encryption_opts(std::move(auto_encrypt_opts));
    class client client{uri{}, std::move(client_opts)};

    auto coll = client["test"]["coll"];

    // Clear old data
    coll.drop();

    coll.insert_one(make_document(kvp("encryptedField", "123456789")));

    auto res = coll.find_one({});
    std::cout << "\nDocument retrieved with auto-encrypted client:\n" << bsoncxx::to_json(*res) << std::endl;

    class client unencrypted_client{uri{}};
    auto unencrypted_coll = unencrypted_client["test"]["coll"];
    auto res2 = unencrypted_coll.find_one({});
    std::cout << "\nDocument retrieved with unencrypted client:\n" << bsoncxx::to_json(*res2) << std::endl;
}
