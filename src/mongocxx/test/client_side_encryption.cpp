// Copyright 2020 MongoDB Inc.
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

#include <fstream>
#include <helpers.hpp>
#include <sstream>
#include <string>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/make_value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/client_encryption.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/client_encryption.hpp>
#include <mongocxx/options/data_key.hpp>
#include <mongocxx/test/spec/monitoring.hh>
#include <mongocxx/test_util/client_helpers.hh>
#include <mongocxx/uri.hpp>
#include <mongocxx/write_concern.hpp>

namespace {
const auto kLocalMasterKey =
    "\x32\x78\x34\x34\x2b\x78\x64\x75\x54\x61\x42\x42\x6b\x59\x31\x36\x45\x72"
    "\x35\x44\x75\x41\x44\x61\x67\x68\x76\x53\x34\x76\x77\x64\x6b\x67\x38\x74"
    "\x70\x50\x70\x33\x74\x7a\x36\x67\x56\x30\x31\x41\x31\x43\x77\x62\x44\x39"
    "\x69\x74\x51\x32\x48\x46\x44\x67\x50\x57\x4f\x70\x38\x65\x4d\x61\x43\x31"
    "\x4f\x69\x37\x36\x36\x4a\x7a\x58\x5a\x42\x64\x42\x64\x62\x64\x4d\x75\x72"
    "\x64\x6f\x6e\x4a\x31\x64";

// This is the base64 encoding of LOCALAAAAAAAAAAAAAAAAA==.
const auto kLocalKeyUUID = "\x2c\xe0\x80\x2c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

// This is the base64 encoding of AWSAAAAAAAAAAAAAAAAAAA==.
const auto kAwsKeyUUID = "\x01\x64\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

// This is the base64 encoding of AZUREAAAAAAAAAAAAAAAAA==.
const auto kAzureKeyUUID = "\x01\x95\x11\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

// This is the base64 encoding of GCPAAAAAAAAAAAAAAAAAAA==.
const auto kGcpKeyUUID = "\x18\x23\xc0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

// This is the base64 encoding of KMIPAAAAAAAAAAAAAAAAAA==.
const auto kKmipKeyUUID = "\x28\xc2\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

using bsoncxx::builder::concatenate;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using bsoncxx::builder::basic::sub_document;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using bsoncxx::types::bson_value::make_value;

using namespace mongocxx;

// Takes a path relative to the ENCRYPTION_TESTS_PATH variable, with leading '/'.
bsoncxx::document::value _doc_from_file(stdx::string_view sub_path) {
    char* encryption_tests_path = std::getenv("ENCRYPTION_TESTS_PATH");
    REQUIRE(encryption_tests_path);

    std::string path = std::string(encryption_tests_path) + sub_path.data();
    CAPTURE(path);

    std::ifstream file{path};
    REQUIRE(file);

    std::string file_contents((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());

    return bsoncxx::from_json(file_contents);
}

void _setup_drop_collections(const client& client) {
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);

    auto datakeys = client["keyvault"]["datakeys"];
    datakeys.drop(wc_majority);

    auto coll = client["db"]["coll"];
    coll.drop(wc_majority);
}
// Returns a document with credentials for KMS providers.
// If include_external is true, all KMS providers are set.
// If include_external is false, only the local provider is set.
bsoncxx::document::value _make_kms_doc(bool include_external = true) {
    auto kms_doc = bsoncxx::builder::basic::document{};

    if (include_external) {
        kms_doc.append(kvp("aws", [&](sub_document subdoc) {
            subdoc.append(kvp("secretAccessKey",
                              test_util::getenv_or_fail("MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY")));
            subdoc.append(
                kvp("accessKeyId", test_util::getenv_or_fail("MONGOCXX_TEST_AWS_ACCESS_KEY_ID")));
        }));

        kms_doc.append(kvp("azure", [&](sub_document subdoc) {
            subdoc.append(
                kvp("tenantId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_TENANT_ID")));
            subdoc.append(
                kvp("clientId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_ID")));
            subdoc.append(kvp("clientSecret",
                              test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_SECRET")));
        }));

        kms_doc.append(kvp("gcp", [&](sub_document subdoc) {
            subdoc.append(kvp("email", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_EMAIL")));
            subdoc.append(
                kvp("privateKey", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_PRIVATEKEY")));
        }));

        kms_doc.append(kvp("kmip", [&](sub_document subdoc) {
            subdoc.append(kvp("endpoint", "localhost:5698"));
        }));
    }

    char key_storage[96];
    memcpy(&(key_storage[0]), kLocalMasterKey, 96);

    bsoncxx::types::b_binary local_master_key{
        bsoncxx::binary_sub_type::k_binary, 96, (const uint8_t*)&key_storage};

    kms_doc.append(
        kvp("local", [&](sub_document subdoc) { subdoc.append(kvp("key", local_master_key)); }));

    return {kms_doc.extract()};
}

bsoncxx::document::value _make_tls_opts() {
    bsoncxx::builder::basic::document tls_opts;

    tls_opts.append(kvp("kmip", [&](sub_document subdoc) {
        subdoc.append(
            kvp("tlsCAFile", test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CA_FILE")));
        subdoc.append(
            kvp("tlsCertificateKeyFile",
                test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE")));
    }));

    return tls_opts.extract();
}

void _add_client_encrypted_opts(options::client* client_opts,
                                bsoncxx::document::view_or_value schema_map,
                                bsoncxx::document::view_or_value kms_doc,
                                bsoncxx::document::view_or_value tls_opts,
                                class client* key_vault_client = nullptr) {
    options::auto_encryption auto_encrypt_opts{};

    // KMS
    auto_encrypt_opts.kms_providers(std::move(kms_doc));
    auto_encrypt_opts.tls_opts(std::move(tls_opts));
    auto_encrypt_opts.key_vault_namespace({"keyvault", "datakeys"});

    if (!schema_map.view().empty()) {
        auto_encrypt_opts.schema_map({std::move(schema_map)});
    }

    // For evergreen testing
    char* bypass_spawn = std::getenv("ENCRYPTION_TESTS_BYPASS_SPAWN");
    char* mongocryptd_path = std::getenv("MONGOCRYPTD_PATH");

    if (bypass_spawn || mongocryptd_path) {
        auto cmd = bsoncxx::builder::basic::document{};

        if (bypass_spawn && strcmp(bypass_spawn, "TRUE") == 0) {
            cmd.append(bsoncxx::builder::basic::kvp("mongocryptdBypassSpawn", true));
        }

        if (mongocryptd_path) {
            cmd.append(bsoncxx::builder::basic::kvp("mongocryptdSpawnPath", mongocryptd_path));
        }

        auto_encrypt_opts.extra_options({cmd.extract()});
    }

    if (key_vault_client) {
        auto_encrypt_opts.key_vault_client(key_vault_client);
    }

    client_opts->auto_encryption_opts(std::move(auto_encrypt_opts));
}

void _add_cse_opts(options::client_encryption* opts,
                   class client* client,
                   bool include_aws = true) {
    // KMS providers
    opts->kms_providers(_make_kms_doc(include_aws));

    // TLS options
    opts->tls_opts(_make_tls_opts());

    // Key vault client
    opts->key_vault_client(client);

    // Key vault namespace
    opts->key_vault_namespace({"keyvault", "datakeys"});
}

template <typename Callable>
void run_datakey_and_double_encryption(Callable create_data_key,
                                       stdx::string_view provider,
                                       client* setup_client,
                                       client* client_encrypted,
                                       client_encryption* client_encryption,
                                       mongocxx::spec::apm_checker* apm_checker) {
    // Test creating and using data keys:
    INFO("using KMS provider: " << provider);

    // 1. Call client_encryption.createDataKey()
    auto datakey_id = create_data_key();

    // Expect a BSON binary with subtype 4 to be returned, referred to as datakey_id.
    REQUIRE(datakey_id.view().type() == bsoncxx::type::k_binary);
    REQUIRE(datakey_id.view().get_binary().sub_type == bsoncxx::binary_sub_type::k_uuid);

    // Use client to run a find on keyvault.datakeys by querying with the _id
    // set to the datakey_id.
    auto datakeys = setup_client->database("keyvault").collection("datakeys");
    mongocxx::read_concern rc_majority;
    rc_majority.acknowledge_level(mongocxx::read_concern::level::k_majority);
    datakeys.read_concern(rc_majority);
    auto query = make_document(kvp("_id", datakey_id));
    auto cursor = datakeys.find(query.view());

    // Expect that exactly one document is returned with the correct "masterKey.provider"
    std::size_t i = 0;
    for (auto&& doc : cursor) {
        REQUIRE(doc["masterKey"]["provider"].get_string().value == provider);
        i++;
    }

    REQUIRE(i == 1);

    // Check that client captured a command_started event for the insert command containing a
    // majority writeConcern.
    auto event = document{} << "command_started_event" << open_document << "command"
                            << open_document << "insert"
                            << "datakeys"
                            << "writeConcern" << open_document << "w"
                            << "majority" << close_document << close_document << close_document
                            << finalize;

    auto arr = bsoncxx::builder::basic::array{};
    arr.append(event);
    apm_checker->has(arr.view());
    apm_checker->clear();

    // 2. Call client_encryption.encrypt() with the value "hello there", the algorithm
    // AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic, and the key_id of datakey_id
    options::encrypt opts{};
    opts.algorithm(options::encrypt::encryption_algorithm::k_deterministic);
    opts.key_id(std::move(datakey_id));

    auto to_encrypt = make_value("hello there");
    auto encrypted_val = client_encryption->encrypt(to_encrypt.view(), opts);

    // Expect the return value to be a BSON binary subtype 6, referred to as encrypted
    auto encrypted = encrypted_val.view();
    REQUIRE(encrypted.type() == bsoncxx::type::k_binary);
    REQUIRE(encrypted.get_binary().sub_type == bsoncxx::binary_sub_type::k_encrypted);

    // Use client_encrypted to insert { _id: provider, "value": <encrypted> } into db.coll
    auto insert_doc = make_document(kvp("_id", provider), kvp("value", encrypted));
    client_encrypted->database("db").collection("coll").insert_one(insert_doc.view());

    // Use client_encrypted to run a find querying with _id of provider and expect value
    // to be "hello there"
    auto filter = make_document(kvp("_id", provider));
    auto res = client_encrypted->database("db").collection("coll").find_one(filter.view());
    REQUIRE(res);
    auto decrypted_bson_val = res->view()["value"];
    REQUIRE(decrypted_bson_val.type() == bsoncxx::type::k_string);
    REQUIRE(decrypted_bson_val.get_string().value == stdx::string_view{"hello there"});

    // 3. Call client_encryption.encrypt() with the value "hello there", the algorithm
    // AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic, and the key_alt_name of provider_altname
    options::encrypt opts2{};
    opts2.algorithm(options::encrypt::encryption_algorithm::k_deterministic);
    std::string altname(provider);
    altname += "_altname";
    opts2.key_alt_name(altname);

    auto encrypted_val2 = client_encryption->encrypt(to_encrypt.view(), opts2);

    // Expect the return value to be a BSON binary subtype 6. Expect the value to exactly
    // match the value of "encrypted"
    auto encrypted2 = encrypted_val2.view();
    REQUIRE(encrypted2.type() == bsoncxx::type::k_binary);
    REQUIRE(encrypted2.get_binary().sub_type == bsoncxx::binary_sub_type::k_encrypted);
    REQUIRE(encrypted == encrypted2);

    // Then, test explicit encryption on an auto encrypted field:
    // Use client_encrypted to attempt to insert { "encrypted_placeholder": (encrypted2) }
    // Expect an exception to be thrown, since this is an attempt to auto encrypt an already
    // encrypted value.
    auto double_encrypted = make_document(kvp("encrypted_placeholder", encrypted2));
    REQUIRE_THROWS(
        client_encrypted->database("db").collection("coll").insert_one(double_encrypted.view()));
}

TEST_CASE("Datakey and double encryption", "[client_side_encryption]") {
    instance::current();

    // Setup
    // 1. Create a mongoclient without encryption
    options::client client_opts;
    spec::apm_checker apm_checker;
    client_opts.apm_opts(apm_checker.get_apm_opts(true /* command_started_events_only */));

    class client setup_client {
        uri{}, test_util::add_test_server_api(client_opts)
    };

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    // 2. Drop keyvault.datakeys and db.coll
    _setup_drop_collections(setup_client);

    // 3. Create and configure client_encrypted, client_encryption.
    options::client encrypted_client_opts;
    _add_client_encrypted_opts(
        &encrypted_client_opts,
        document() << "db.coll" << open_document << "bsonType"
                   << "object"
                   << "properties" << open_document << "encrypted_placeholder" << open_document
                   << "encrypt" << open_document << "keyId"
                   << "/placeholder"
                   << "bsonType"
                   << "string"
                   << "algorithm"
                   << "AEAD_AES_256_CBC_HMAC_SHA_512-Random" << close_document << close_document
                   << close_document << close_document << finalize,
        _make_kms_doc(),
        _make_tls_opts());

    class client client_encrypted {
        uri{}, test_util::add_test_server_api(encrypted_client_opts),
    };

    // Configure both with aws and local KMS providers, and schema map
    options::client_encryption cse_opts;
    _add_cse_opts(&cse_opts, &setup_client);
    client_encryption client_encryption{std::move(cse_opts)};

    // Run test with local
    run_datakey_and_double_encryption(
        [&]() {
            // 1. Call client_encryption.createDataKey() with the local KMS provider
            // and keyAltNames set to ["local_altname"].
            options::data_key data_key_opts;
            data_key_opts.key_alt_names({"local_altname"});
            return client_encryption.create_data_key("local", data_key_opts);
        },
        "local",
        &setup_client,
        &client_encrypted,
        &client_encryption,
        &apm_checker);

    // Run with AWS
    run_datakey_and_double_encryption(
        [&]() {
            // 1. Call client_encryption.createDataKey() with the aws KMS provider
            // and keyAltNames set to ["aws_altname"].
            options::data_key data_key_opts;
            data_key_opts.key_alt_names({"aws_altname"});

            auto doc = make_document(
                kvp("region", "us-east-1"),
                kvp("key",
                    "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"));
            data_key_opts.master_key(doc.view());

            return client_encryption.create_data_key("aws", data_key_opts);
        },
        "aws",
        &setup_client,
        &client_encrypted,
        &client_encryption,
        &apm_checker);

    // Run with Azure
    run_datakey_and_double_encryption(
        [&]() {
            // 1. Call client_encryption.createDataKey() with the azure KMS provider
            // and keyAltNames set to ["azure_altname"].
            options::data_key data_key_opts;
            data_key_opts.key_alt_names({"azure_altname"});

            auto doc = make_document(kvp("keyVaultEndpoint", "key-vault-csfle.vault.azure.net"),
                                     kvp("keyName", "key-name-csfle"));
            data_key_opts.master_key(doc.view());

            return client_encryption.create_data_key("azure", data_key_opts);
        },
        "azure",
        &setup_client,
        &client_encrypted,
        &client_encryption,
        &apm_checker);

    // Run with GCP
    run_datakey_and_double_encryption(
        [&]() {
            // 1. Call client_encryption.createDataKey() with the gcp KMS provider
            // and keyAltNames set to ["gcp_altname"].
            options::data_key data_key_opts;
            data_key_opts.key_alt_names({"gcp_altname"});

            auto doc = make_document(kvp("projectId", "devprod-drivers"),
                                     kvp("location", "global"),
                                     kvp("keyRing", "key-ring-csfle"),
                                     kvp("keyName", "key-name-csfle"));
            data_key_opts.master_key(doc.view());

            return client_encryption.create_data_key("gcp", data_key_opts);
        },
        "gcp",
        &setup_client,
        &client_encrypted,
        &client_encryption,
        &apm_checker);

    // Run with KMIP
    run_datakey_and_double_encryption(
        [&]() {
            // 1. Call client_encryption.createDataKey() with the KMIP KMS provider
            // and keyAltNames set to ["kmip_altname"].
            options::data_key data_key_opts;
            data_key_opts.key_alt_names({"kmip_altname"});

            auto doc = make_document();
            data_key_opts.master_key(doc.view());

            return client_encryption.create_data_key("kmip", data_key_opts);
        },
        "kmip",
        &setup_client,
        &client_encrypted,
        &client_encryption,
        &apm_checker);
}

void run_external_key_vault_test(bool with_external_key_vault) {
    class client external_key_vault_client {
        uri{"mongodb://fake-user:fake-pwd@localhost:27017"}, test_util::add_test_server_api()
    };

    // Create a MongoClient without encryption enabled (referred to as client).
    class client client {
        uri{}, test_util::add_test_server_api(),
    };

    // Using client, drop the collections keyvault.datakeys and db.coll.
    _setup_drop_collections(client);

    // Insert the document external/external-key.json into keyvault.datakeys.
    auto datakeys = client["keyvault"]["datakeys"];

    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);
    options::insert insert_opts;
    insert_opts.write_concern(std::move(wc_majority));

    auto external_key = _doc_from_file("/external/external-key.json");
    datakeys.insert_one(external_key.view(), insert_opts);

    auto external_schema = _doc_from_file("/external/external-schema.json");
    auto schema_map = make_document(kvp("db.coll", external_schema));

    // Create a MongoClient configured with auto encryption (referred to as client_encrypted),
    // that is configured with an external key vault client if with_external_key_vault is true
    options::client encrypted_client_opts;

    if (with_external_key_vault) {
        _add_client_encrypted_opts(&encrypted_client_opts,
                                   std::move(schema_map),
                                   _make_kms_doc(false),
                                   _make_tls_opts(),
                                   &external_key_vault_client);
    } else {
        _add_client_encrypted_opts(
            &encrypted_client_opts, std::move(schema_map), _make_kms_doc(false), _make_tls_opts());
    }

    class client client_encrypted {
        uri{}, test_util::add_test_server_api(encrypted_client_opts),
    };

    // A ClientEncryption object (referred to as client_encryption) that is configured with an
    // external key vault client if with_external_key_vault is true.
    options::client_encryption cse_opts;
    if (with_external_key_vault) {
        _add_cse_opts(&cse_opts, &external_key_vault_client, false);
    } else {
        // c driver uses "client" here
        _add_cse_opts(&cse_opts, &client, false);
    }

    client_encryption client_encryption{std::move(cse_opts)};

    // Use client_encrypted to insert the document {"encrypted": "test"} into db.coll.
    auto doc = make_document(kvp("encrypted", "test"));
    auto coll = client_encrypted["db"]["coll"];

    // If withExternalKeyVault == true, expect an authentication exception to be thrown.
    // Otherwise, expect the insert to succeed.
    if (with_external_key_vault) {
        REQUIRE_THROWS(coll.insert_one(doc.view()));
    } else {
        coll.insert_one(doc.view());
    }

    // Use client_encryption to explicitly encrypt the string "test" with key ID
    // LOCALAAAAAAAAAAAAAAAAA== and deterministic algorithm. If withExternalKeyVault == true,
    // expect an authentication exception to be thrown. Otherwise, expect the insert to succeed.

    options::encrypt opts;
    auto key_id = external_key.view()["_id"].get_value();
    opts.key_id(key_id);
    opts.algorithm(options::encrypt::encryption_algorithm::k_deterministic);

    auto value = make_value("test");

    if (with_external_key_vault) {
        REQUIRE_THROWS(client_encryption.encrypt(value, opts));
    } else {
        auto res = client_encryption.encrypt(value, opts);
    }
}

TEST_CASE("External key vault", "[client_side_encryption]") {
    instance::current();

    class client setup_client {
        uri{}, test_util::add_test_server_api(),
    };
    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    run_external_key_vault_test(true);
    run_external_key_vault_test(false);
}

TEST_CASE("BSON size limits and batch splitting", "[client_side_encryption]") {
    instance::current();

    // Create a MongoClient without encryption enabled (referred to as client).
    class client client {
        uri{}, test_util::add_test_server_api(),
    };

    if (test_util::get_max_wire_version(client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    // Load in json schema limits/limits-schema.json and limits/limits-key.json
    auto limits_schema = _doc_from_file("/limits/limits-schema.json");
    auto limits_key = _doc_from_file("/limits/limits-key.json");

    // Using client, drop db.coll and keyvault.datakeys.
    _setup_drop_collections(client);

    // Create the collection db.coll configured with the JSON schema limits/limits-schema.json.
    auto db = client["db"];
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);
    db.write_concern(wc_majority);
    auto cmd = document{} << "create"
                          << "coll"
                          << "validator" << open_document << "$jsonSchema" << limits_schema.view()
                          << close_document << finalize;
    db.run_command(cmd.view());

    // Insert the document limits/limits-key.json into keyvault.datakeys.
    auto datakeys = client["keyvault"]["datakeys"];
    datakeys.write_concern(wc_majority);
    datakeys.insert_one(limits_key.view());

    // Create a MongoClient configured with auto encryption (referred to as client_encrypted),
    // with local KMS provider as follows:
    //     { "local": { "key": <base64 decoding of LOCAL_MASTERKEY> } }
    // and with the keyVaultNamespace set to keyvault.datakeys.
    options::client client_encrypted_opts;
    _add_client_encrypted_opts(
        &client_encrypted_opts, limits_schema.view(), _make_kms_doc(false), _make_tls_opts());

    // Add a counter to verify splits
    int n_inserts = 0;
    options::apm apm_opts;
    apm_opts.on_command_started([&](const events::command_started_event& event) {
        if (event.command_name().compare("insert") == 0) {
            n_inserts++;
        }
    });

    client_encrypted_opts.apm_opts(apm_opts);

    class client client_encrypted {
        uri{}, test_util::add_test_server_api(client_encrypted_opts),
    };

    // Using client_encrypted perform the following operations:
    // Insert { "_id": "over_2mib_under_16mib",
    //          "unencrypted": <the string "a" repeated 2097152 times> }.
    std::string over_2mib_under_16mib(2097152, 'a');
    auto over_2mib_under_16mib_doc = make_document(kvp("_id", "over_2mib_under_16mib"),
                                                   kvp("unencrypted", over_2mib_under_16mib));

    // Expect this to succeed since this is still under the maxBsonObjectSize limit.
    auto coll = client_encrypted["db"]["coll"];
    coll.insert_one(over_2mib_under_16mib_doc.view());
    REQUIRE(n_inserts == 1);

    // Insert the document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_2mib",
    //   "unencrypted": < the string 'a' repeated (2097152 - 2000) times > }
    // Note: limits-doc.json is a 1005 byte BSON document that encrypts to a ~10,000 byte document.
    std::string encryption_exceeds_2mib(2097152 - 2000, 'a');
    auto limits_doc = _doc_from_file("/limits/limits-doc.json");
    auto encryption_exceeds_2mib_doc = document{} << "_id"
                                                  << "encryption_exceeds_2mib"
                                                  << "unencrypted" << encryption_exceeds_2mib
                                                  << concatenate(limits_doc.view()) << finalize;

    // Expect this to succeed since after encryption this still is below the normal
    // maximum BSON document size. Note, before auto encryption this document is under
    // the 2 MiB limit. After encryption it exceeds the 2 MiB limit, but does NOT exceed
    // the 16 MiB limit.
    n_inserts = 0;
    coll.insert_one(encryption_exceeds_2mib_doc.view());
    REQUIRE(n_inserts == 1);

    // Bulk insert the following:
    // { "_id": "over_2mib_1", "unencrypted": <the string 'a' repeated (2097152) times> }
    // { "_id": "over_2mib_2", "unencrypted": <the string 'a' repeated (2097152) times> }
    auto over_2mib_1 = document{} << "_id"
                                  << "over_2mib_1"
                                  << "unencrypted" << over_2mib_under_16mib << finalize;
    auto over_2mib_2 = document{} << "_id"
                                  << "over_2mib_2"
                                  << "unencrypted" << over_2mib_under_16mib << finalize;

    std::vector<bsoncxx::document::view> docs;
    docs.push_back(std::move(over_2mib_1));
    docs.push_back(std::move(over_2mib_2));

    // Expect the bulk write to succeed and split after first doc (i.e. two inserts occur).
    n_inserts = 0;
    auto res = coll.insert_many(docs);
    REQUIRE(n_inserts == 2);

    // Bulk insert the following:

    // The document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_2mib_1",
    //   "unencrypted": < the string 'a' repeated (2097152 - 2000) times > }
    // The document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_2mib_2",
    //   "unencrypted": < the string 'a' repeated (2097152 - 2000) times > }
    docs.clear();
    auto concat_1 = document{} << "_id"
                               << "encryption_exceeds_2mib_1"
                               << "unencrypted" << encryption_exceeds_2mib
                               << concatenate(limits_doc.view()) << finalize;
    auto concat_2 = document{} << "_id"
                               << "encryption_exceeds_2mib_2"
                               << "unencrypted" << encryption_exceeds_2mib
                               << concatenate(limits_doc.view()) << finalize;

    docs.push_back(std::move(concat_1));
    docs.push_back(std::move(concat_2));

    // Expect the bulk write to succeed.
    res = coll.insert_many(docs);

    // Insert { "_id": "under_16mib", "unencrypted": <the string 'a' repeated 16777216 - 2000 times>
    // }
    std::string under_16mib(16777216 - 2000, 'a');
    auto under_16mib_doc = document{} << "_id"
                                      << "under_16mib"
                                      << "unencrypted" << under_16mib << finalize;

    // Expect this to succeed since this is still (just) under the maxBsonObjectSize limit.
    n_inserts = 0;
    coll.insert_one(under_16mib_doc.view());
    REQUIRE(n_inserts == 1);

    // Insert the document limits/limits-doc.json concatenated with
    // { "_id": "encryption_exceeds_16mib",
    //   "unencrypted": < the string 'a' repeated (16777216 - 2000) times > }
    auto encryption_exceeds_16mib = document{} << "_id"
                                               << "encryption_exceeds_16mib"
                                               << "unencrypted" << under_16mib
                                               << concatenate(limits_doc.view()) << finalize;

    // Expect this to fail since encryption results in a document exceeding
    // the maxBsonObjectSize limit.
    REQUIRE_THROWS(coll.insert_one(encryption_exceeds_16mib.view()));
}

TEST_CASE("Views are prohibited", "[client_side_encryption]") {
    instance::current();

    // Create a MongoClient without encryption enabled (referred to as client).
    class client client {
        uri{}, test_util::add_test_server_api()
    };

    if (test_util::get_max_wire_version(client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    // Using client, drop and create a view named db.view with an empty pipeline.
    // E.g. using the command { "create": "view", "viewOn": "coll" }.
    auto db = client["db"];
    db["view"].drop();

    auto cmd = make_document(kvp("create", "view"), kvp("viewOn", "coll"));
    db.run_command(cmd.view());

    // Create a MongoClient configured with auto encryption (referred to as client_encrypted)
    // Configure with the local KMS provider as follows:
    // { "local": { "key": <base64 decoding of LOCAL_MASTERKEY> } }
    // Configure with the keyVaultNamespace set to keyvault.datakeys.
    options::client opts;
    _add_client_encrypted_opts(&opts, {}, _make_kms_doc(), _make_tls_opts());
    class client client_encrypted {
        uri{}, test_util::add_test_server_api(opts)
    };

    // Using client_encrypted, attempt to insert a document into db.view.
    // Expect an exception to be thrown containing the message: "cannot auto encrypt a view".
    REQUIRE_THROWS_MATCHES(client_encrypted["db"]["view"].insert_one(cmd.view()),
                           mongocxx::exception,
                           test_util::mongocxx_exception_matcher{"cannot auto encrypt a view"});
}

void _run_corpus_test(bool use_schema_map) {
    // The corpus test exhaustively enumerates all ways to encrypt all BSON value types.
    // Note, the test data includes BSON binary subtype 4 (or standard UUID),
    // which MUST be decoded and encoded as subtype 4. Run the test as follows.

    // Create a MongoClient without encryption enabled (referred to as client).
    class client client {
        uri{}, test_util::add_test_server_api(),
    };

    auto corpus_schema = _doc_from_file("/corpus/corpus-schema.json");
    auto corpus_key_local = _doc_from_file("/corpus/corpus-key-local.json");
    auto corpus_key_aws = _doc_from_file("/corpus/corpus-key-aws.json");
    auto corpus_key_azure = _doc_from_file("/corpus/corpus-key-azure.json");
    auto corpus_key_gcp = _doc_from_file("/corpus/corpus-key-gcp.json");
    auto corpus_key_kmip = _doc_from_file("/corpus/corpus-key-kmip.json");

    // Using client, drop and create the collection db.coll configured with the included
    // JSON schema corpus/corpus-schema.json.
    _setup_drop_collections(client);

    auto db = client["db"];
    auto cmd = document{} << "create"
                          << "coll"
                          << "validator" << open_document << "$jsonSchema" << corpus_schema.view()
                          << close_document << finalize;

    db.run_command(cmd.view());

    // Insert all of the key documents.
    write_concern wc_majority;
    wc_majority.acknowledge_level(write_concern::level::k_majority);
    options::insert insert_opts;
    insert_opts.write_concern(std::move(wc_majority));

    auto keyvault = client["keyvault"]["datakeys"];
    keyvault.insert_one(std::move(corpus_key_local), insert_opts);
    keyvault.insert_one(std::move(corpus_key_aws), insert_opts);
    keyvault.insert_one(std::move(corpus_key_azure), insert_opts);
    keyvault.insert_one(std::move(corpus_key_gcp), insert_opts);
    keyvault.insert_one(std::move(corpus_key_kmip), insert_opts);

    // Configure kms credentials as follows:
    // {
    //     "aws": { <AWS credentials> },
    //     "azure": { <Azure credentials> },
    //     "gcp": { <GCP credentials> },
    //     "local": { "key": <base64 decoding of LOCAL_MASTERKEY> },
    //     "kmip": { "endpoint": "localhost:5698" }
    // }

    char local_key_id_storage[16];
    char aws_key_id_storage[16];
    char azure_key_id_storage[16];
    char gcp_key_id_storage[16];
    char kmip_key_id_storage[16];
    memcpy(&(local_key_id_storage[0]), kLocalKeyUUID, 16);
    memcpy(&(aws_key_id_storage[0]), kAwsKeyUUID, 16);
    memcpy(&(azure_key_id_storage[0]), kAzureKeyUUID, 16);
    memcpy(&(gcp_key_id_storage[0]), kGcpKeyUUID, 16);
    memcpy(&(kmip_key_id_storage[0]), kKmipKeyUUID, 16);

    bsoncxx::types::b_binary local_key_id{
        bsoncxx::binary_sub_type::k_uuid, 16, (const uint8_t*)&local_key_id_storage};
    bsoncxx::types::b_binary aws_key_id{
        bsoncxx::binary_sub_type::k_uuid, 16, (const uint8_t*)&aws_key_id_storage};
    bsoncxx::types::b_binary azure_key_id{
        bsoncxx::binary_sub_type::k_uuid, 16, (const uint8_t*)&azure_key_id_storage};
    bsoncxx::types::b_binary gcp_key_id{
        bsoncxx::binary_sub_type::k_uuid, 16, (const uint8_t*)&gcp_key_id_storage};
    bsoncxx::types::b_binary kmip_key_id{
        bsoncxx::binary_sub_type::k_uuid, 16, (const uint8_t*)&kmip_key_id_storage};

    auto local_key_value = make_value(local_key_id);
    auto aws_key_value = make_value(aws_key_id);
    auto azure_key_value = make_value(azure_key_id);
    auto gcp_key_value = make_value(gcp_key_id);
    auto kmip_key_value = make_value(kmip_key_id);

    // Create the following and configure both objects with keyVaultNamespace set to
    // keyvault.datakeys:
    // A MongoClient configured with auto encryption (referred to as client_encrypted)
    options::client client_encrypted_opts;
    if (use_schema_map) {
        _add_client_encrypted_opts(
            &client_encrypted_opts, corpus_schema.view(), _make_kms_doc(), _make_tls_opts());
    } else {
        _add_client_encrypted_opts(&client_encrypted_opts, {}, _make_kms_doc(), _make_tls_opts());
    }

    class client client_encrypted {
        uri{}, test_util::add_test_server_api(client_encrypted_opts),
    };

    // A ClientEncryption object (referred to as client_encryption)
    options::client_encryption cse_opts;
    cse_opts.kms_providers(_make_kms_doc());
    cse_opts.tls_opts(_make_tls_opts());
    cse_opts.key_vault_client(&client);
    cse_opts.key_vault_namespace({"keyvault", "datakeys"});
    class client_encryption client_encryption {
        std::move(cse_opts)
    };

    // Load corpus/corpus.json to a variable named corpus.
    auto corpus = _doc_from_file("/corpus/corpus.json");
    auto corpus_encrypted_expected = _doc_from_file("/corpus/corpus-encrypted.json");

    // Create a new BSON document, named corpus_copied.
    auto corpus_copied_builder = bsoncxx::builder::basic::document{};

    // Iterate over each field of corpus.
    for (bsoncxx::document::element ele : corpus.view()) {
        auto field_name_view = ele.key();
        std::string field_name(field_name_view);

        // If the field name is _id, altname_aws, altname_azure, altname_gcp, altname_local, copy
        // the field to corpus_copied.
        std::vector<std::string> copied_fields = {
            "_id", "altname_aws", "altname_azure", "altname_gcp", "altname_kmip", "altname_local"};
        if (std::find(copied_fields.begin(), copied_fields.end(), field_name) !=
            copied_fields.end()) {
            corpus_copied_builder.append(kvp(field_name, ele.get_value()));
            continue;
        }

        auto subdoc_val = ele.get_document();
        auto subdoc = subdoc_val.view();

        // The corpus contains subdocuments with the following fields:
        // kms is either aws or local
        // type is a BSON type string names coming from here)
        // algo is either rand or det for random or deterministic encryption
        // method is either auto, for automatic encryption or explicit for explicit encryption
        // identifier is either id or altname for the key identifier
        // allowed is a boolean indicating whether the encryption for the given parameters is
        // permitted.
        // value is the value to be tested.
        auto kms = subdoc["kms"].get_string().value;
        auto type = subdoc["type"].get_string().value;
        auto algo = subdoc["algo"].get_string().value;
        auto method = subdoc["method"].get_string().value;
        auto identifier = subdoc["identifier"].get_string().value;
        auto allowed = subdoc["allowed"].get_bool().value;
        auto to_encrypt = subdoc["value"].get_value();

        // If method is auto, copy the field to corpus_copied.
        // If method is explicit, encrypt explicitly.
        if (method == stdx::string_view("auto")) {
            corpus_copied_builder.append(kvp(field_name, ele.get_value()));
        } else if (method == stdx::string_view{"explicit"}) {
            options::encrypt encrypt_opts;

            // Encrypt with the algorithm described by algo
            if (algo == stdx::string_view{"rand"}) {
                encrypt_opts.algorithm(options::encrypt::encryption_algorithm::k_random);
            } else if (algo == stdx::string_view{"det"}) {
                encrypt_opts.algorithm(options::encrypt::encryption_algorithm::k_deterministic);
            } else {
                throw exception{error_code::k_invalid_parameter, "unsupported algorithm"};
            }

            if (identifier == stdx::string_view{"id"}) {
                if (kms == stdx::string_view{"local"}) {
                    // If kms is local set the key_id to the UUID with base64 value
                    // LOCALAAAAAAAAAAAAAAAAA==.
                    encrypt_opts.key_id(local_key_value.view());
                } else if (kms == stdx::string_view{"aws"}) {
                    // If kms is aws set the key_id to the UUID with base64 value
                    // AWSAAAAAAAAAAAAAAAAAAA==.
                    encrypt_opts.key_id(aws_key_value.view());
                } else if (kms == stdx::string_view{"azure"}) {
                    // If kms is azure set the key_id to the UUID with base64 value
                    // AZUREAAAAAAAAAAAAAAAAA==.
                    encrypt_opts.key_id(azure_key_value.view());
                } else if (kms == stdx::string_view("gcp")) {
                    // If kms is gcp set the key_id to the UUID with base64 value
                    // GCPAAAAAAAAAAAAAAAAAAA==.
                    encrypt_opts.key_id(gcp_key_value.view());
                } else if (kms == stdx::string_view("kmip")) {
                    // If kms is kmip set the key_id to the UUID with base64 value
                    // KMIPAAAAAAAAAAAAAAAAAA==.
                    encrypt_opts.key_id(kmip_key_value.view());
                } else {
                    throw exception{error_code::k_invalid_parameter, "unsupported kms identifier"};
                }
            } else if (identifier == stdx::string_view{"altname"}) {
                if (kms == stdx::string_view{"local"}) {
                    // If kms is local set the key_alt_name to "local".
                    encrypt_opts.key_alt_name("local");
                } else if (kms == stdx::string_view{"aws"}) {
                    // If kms is aws set the key_alt_name to "aws".
                    encrypt_opts.key_alt_name("aws");
                } else if (kms == stdx::string_view("azure")) {
                    // If kms is azure set the key_alt_name to "azure".
                    encrypt_opts.key_alt_name("azure");
                } else if (kms == stdx::string_view("gcp")) {
                    // If kms is gcp set the key_alt_name to "gcp".
                    encrypt_opts.key_alt_name("gcp");
                } else if (kms == stdx::string_view("kmip")) {
                    // If kms is kmip set the key_alt_name to "kmip".
                    encrypt_opts.key_alt_name("kmip");
                } else {
                    throw exception{error_code::k_invalid_parameter, "unsupported kms altname"};
                }
            } else {
                throw exception{error_code::k_invalid_parameter, "unsupported identifier"};
            }

            if (allowed) {
                try {
                    // If allowed is true, copy the field and encrypted value to corpus_copied.
                    auto encrypted_val =
                        client_encryption.encrypt(to_encrypt, std::move(encrypt_opts));

                    auto new_field = document{} << "kms" << kms << "type" << type << "algo" << algo
                                                << "method" << method << "identifier" << identifier
                                                << "allowed" << allowed << "value" << encrypted_val
                                                << finalize;

                    corpus_copied_builder.append(kvp(field_name, std::move(new_field)));
                } catch (const std::exception& e) {
                    FAIL("caught an exception for encrypting an allowed field "
                         << field_name << ": " << e.what());
                }
            } else {
                REQUIRE_THROWS(client_encryption.encrypt(to_encrypt, std::move(encrypt_opts)));
                corpus_copied_builder.append(kvp(field_name, subdoc));
            }
        }
    }

    auto corpus_copied = corpus_copied_builder.extract();

    auto encrypted_coll = client_encrypted["db"]["coll"];

    // Using client_encrypted, insert corpus_copied into db.coll.
    try {
        encrypted_coll.insert_one(corpus_copied.view(), insert_opts);
    } catch (const std::exception& e) {
        FAIL("failed to insert the corpus document: " << e.what());
    }

    // Using client_encrypted, find the inserted document from db.coll to a variable
    // named corpus_decrypted. Since it should have been automatically decrypted, assert
    // the document exactly matches corpus.
    auto res = encrypted_coll.find_one({});
    REQUIRE(res);
    auto corpus_decrypted = res.value();
    REQUIRE(corpus_decrypted == corpus);

    // Load corpus/corpus_encrypted.json to a variable named corpus_encrypted_expected.
    // Using client find the inserted document from db.coll to a variable named
    // corpus_encrypted_actual.
    res = client["db"]["coll"].find_one({});
    REQUIRE(res);
    auto corpus_encrypted_actual = res.value();

    // Iterate over each field of corpus_encrypted_expected and check the following:
    for (bsoncxx::document::element ele : corpus_encrypted_expected.view()) {
        auto field_name_view = ele.key();
        std::string field_name(field_name_view);

        std::vector<std::string> copied_fields = {"_id", "altname_aws", "altname_local"};
        if (std::find(copied_fields.begin(), copied_fields.end(), field_name) !=
            copied_fields.end()) {
            continue;
        }

        auto subdoc_val = ele.get_document();
        auto subdoc = subdoc_val.view();

        auto algo = subdoc["algo"].get_string().value;
        auto allowed = subdoc["allowed"].get_bool().value;
        auto value = subdoc["value"].get_value();

        auto actual_field = corpus_encrypted_actual.view()[field_name];

        // If the algo is det, that the value equals the value of the corresponding field
        // in corpus_encrypted_actual.
        if (algo == stdx::string_view{"det"}) {
            REQUIRE(value == actual_field["value"].get_value());
        }

        // If the algo is rand and allowed is true, that the value does not equal the value
        // of the corresponding field in corpus_encrypted_actual.
        if (algo == stdx::string_view{"rand"} && allowed) {
            REQUIRE(value != actual_field["value"].get_value());
        }

        // If allowed is true, decrypt the value with client_encryption. Decrypt the value of
        // the corresponding field of corpus_encrypted and validate that they are both equal.
        if (allowed) {
            auto decrypted_actual = client_encryption.decrypt(actual_field["value"].get_value());
            auto decrypted_expected = client_encryption.decrypt(value);
            REQUIRE(decrypted_expected == decrypted_actual);
        } else {
            // If allowed is false, validate the value exactly equals the value of the corresponding
            // field of corpus (neither was encrypted).
            REQUIRE(value == corpus.view()[field_name]["value"].get_value());
        }
    }
}

TEST_CASE("Corpus", "[client_side_encryption]") {
    instance::current();

    // Data keys created with AWS KMS may specify a custom endpoint to contact
    // (instead of the default endpoint derived from the AWS region).

    class client setup_client {
        uri {}
    };

    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }
    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    _run_corpus_test(true);
    _run_corpus_test(false);
}

void _round_trip(class client_encryption* client_encryption,
                 bsoncxx::types::bson_value::view datakey) {
    auto to_encrypt = make_value("test");

    options::encrypt encrypt_opts;
    encrypt_opts.algorithm(options::encrypt::encryption_algorithm::k_deterministic);
    encrypt_opts.key_id(datakey);
    auto encrypted_val = client_encryption->encrypt(to_encrypt, encrypt_opts);
    auto decrypted_val = client_encryption->decrypt(encrypted_val);

    REQUIRE(decrypted_val == to_encrypt);
}

void _run_endpoint_test(mongocxx::client* setup_client,
                        bsoncxx::document::view masterkey,
                        std::string kms_provider,
                        stdx::optional<std::string> error_msg = stdx::nullopt,
                        stdx::optional<std::string> invalid_error_msg = stdx::nullopt) {
    INFO("masterkey" << bsoncxx::to_json(masterkey));

    mongocxx::options::client_encryption ce_opts;
    mongocxx::options::client_encryption ce_opts_invalid;
    bsoncxx::builder::basic::document kms_doc;
    bsoncxx::builder::basic::document kms_doc_invalid;
    bsoncxx::builder::basic::document tls_opts;

    kms_doc.append(kvp("aws", [&](sub_document subdoc) {
        subdoc.append(kvp("secretAccessKey",
                          test_util::getenv_or_fail("MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY")));
        subdoc.append(
            kvp("accessKeyId", test_util::getenv_or_fail("MONGOCXX_TEST_AWS_ACCESS_KEY_ID")));
    }));

    kms_doc.append(kvp("azure", [&](sub_document subdoc) {
        subdoc.append(kvp("tenantId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_TENANT_ID")));
        subdoc.append(kvp("clientId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_ID")));
        subdoc.append(
            kvp("clientSecret", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_SECRET")));
        subdoc.append(kvp("identityPlatformEndpoint", "login.microsoftonline.com:443"));
    }));

    kms_doc.append(kvp("gcp", [&](sub_document subdoc) {
        subdoc.append(kvp("email", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_EMAIL")));
        subdoc.append(kvp("privateKey", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_PRIVATEKEY")));
        subdoc.append(kvp("endpoint", "oauth2.googleapis.com:443"));
    }));

    kms_doc.append(kvp(
        "kmip", [&](sub_document subdoc) { subdoc.append(kvp("endpoint", "localhost:5698")); }));

    tls_opts.append(kvp("kmip", [&](sub_document subdoc) {
        subdoc.append(
            kvp("tlsCAFile", test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CA_FILE")));
        subdoc.append(
            kvp("tlsCertificateKeyFile",
                test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE")));
    }));

    ce_opts.key_vault_client(setup_client);
    ce_opts.key_vault_namespace({"keyvault", "datakeys"});
    ce_opts.kms_providers(kms_doc.view());
    ce_opts.tls_opts(tls_opts.view());
    mongocxx::client_encryption client_encryption{ce_opts};

    kms_doc_invalid.append(kvp("azure", [&](sub_document subdoc) {
        subdoc.append(kvp("tenantId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_TENANT_ID")));
        subdoc.append(kvp("clientId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_ID")));
        subdoc.append(
            kvp("clientSecret", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_SECRET")));
        subdoc.append(kvp("identityPlatformEndpoint", "doesnotexist.invalid:443"));
    }));

    kms_doc_invalid.append(kvp("gcp", [&](sub_document subdoc) {
        subdoc.append(kvp("email", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_EMAIL")));
        subdoc.append(kvp("privateKey", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_PRIVATEKEY")));
        subdoc.append(kvp("endpoint", "doesnotexist.invalid:443"));
    }));

    kms_doc_invalid.append(kvp("kmip", [&](sub_document subdoc) {
        subdoc.append(kvp("endpoint", "doesnotexist.local:5698"));
    }));

    ce_opts_invalid.key_vault_client(setup_client);
    ce_opts_invalid.key_vault_namespace({"keyvault", "datakeys"});
    ce_opts_invalid.kms_providers(kms_doc_invalid.view());
    mongocxx::client_encryption client_encryption_invalid{ce_opts_invalid};

    options::data_key opts;
    opts.master_key(masterkey);
    if (error_msg) {
        REQUIRE_THROWS_MATCHES(client_encryption.create_data_key(kms_provider, std::move(opts)),
                               mongocxx::exception,
                               test_util::mongocxx_exception_matcher{*error_msg});
    } else {
        auto datakey = client_encryption.create_data_key(kms_provider, std::move(opts));
        _round_trip(&client_encryption, datakey);
    }

    if (invalid_error_msg) {
        REQUIRE_THROWS_MATCHES(
            client_encryption_invalid.create_data_key(kms_provider, std::move(opts)),
            mongocxx::exception,
            test_util::mongocxx_exception_matcher{*invalid_error_msg});
    }
}

TEST_CASE("Custom endpoint", "[client_side_encryption]") {
    instance::current();

    // Data keys created with AWS KMS may specify a custom endpoint to contact
    // (instead of the default endpoint derived from the AWS region).

    class client setup_client {
        uri{}, test_util::add_test_server_api(),
    };

    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    // Call client_encryption.createDataKey() with "aws" as the provider and the following
    // masterKey:
    // {
    //   region: "us-east-1",
    //   key: "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
    // }
    // Expect this to succeed. Use the returned UUID of the key to explicitly encrypt and
    // decrypt the string "test" to validate it works.
    SECTION("Test Case 1") {
        auto simple_masterkey = make_document(
            kvp("region", "us-east-1"),
            kvp("key",
                "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"));
        _run_endpoint_test(&setup_client, simple_masterkey.view(), "aws");
    }

    // Call client_encryption.createDataKey() with "aws" as the provider and the following
    // masterKey:
    // {
    //   region: "us-east-1",
    //   key: "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0",
    //   endpoint: "kms.us-east-1.amazonaws.com"
    // }
    // Expect this to succeed. Use the returned UUID of the key to explicitly encrypt and
    // decrypt the string "test" to validate it works.
    SECTION("Test Case 2") {
        auto endpoint_masterkey =
            document{}
            << "region"
            << "us-east-1"
            << "key"
            << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
            << "endpoint"
            << "kms.us-east-1.amazonaws.com" << finalize;
        _run_endpoint_test(&setup_client, endpoint_masterkey.view(), "aws");
    }

    // Call client_encryption.createDataKey() with "aws" as the provider and the following
    // masterKey:
    // {
    //   region: "us-east-1",
    //   key: "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0",
    //   endpoint: "kms.us-east-1.amazonaws.com:443"
    // }
    // Expect this to succeed. Use the returned UUID of the key to explicitly encrypt and
    // decrypt the string "test" to validate it works.
    SECTION("Test Case 3") {
        auto endpoint_masterkey2 =
            document{}
            << "region"
            << "us-east-1"
            << "key"
            << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
            << "endpoint"
            << "kms.us-east-1.amazonaws.com:443" << finalize;
        _run_endpoint_test(&setup_client, endpoint_masterkey2.view(), "aws");
    }

    // Call client_encryption.createDataKey() with "aws" as the provider and the following
    // masterKey:
    // {
    //   region: "us-east-1",
    //   key: "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0",
    //   endpoint: "kms.us-east-1.amazonaws.com:12345"
    // }
    // Expect this to fail with a socket connection error.
    SECTION("Test Case 4") {
        auto socket_error_masterkey =
            document{}
            << "region"
            << "us-east-1"
            << "key"
            << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
            << "endpoint"
            << "kms.us-east-1.amazonaws.com:12345" << finalize;
        _run_endpoint_test(&setup_client, socket_error_masterkey.view(), "aws", {{"error"}});
    }

    // Call client_encryption.createDataKey() with "aws" as the provider and the following
    // masterKey:
    // {
    //   region: "us-east-1",
    //   key: "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0",
    //   endpoint: "kms.us-east-2.amazonaws.com"
    // }
    // Expect this to fail with an exception.
    SECTION("Test Case 5") {
        auto endpoint_error_masterkey =
            document{}
            << "region"
            << "us-east-1"
            << "key"
            << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
            << "endpoint"
            << "kms.us-east-2.amazonaws.com" << finalize;
        _run_endpoint_test(&setup_client, endpoint_error_masterkey.view(), "aws", {{""}});
    }

    // Call client_encryption.createDataKey() with "aws" as the provider and the following
    // masterKey:
    // {
    //   region: "us-east-1",
    //   key: "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0",
    //   endpoint: "doesnotexist.invalid"
    // }
    // Expect this to fail with a network exception indicating failure to resolve
    // "doesnotexist.invalid".
    SECTION("Test Case 6") {
        auto parse_error_masterkey =
            document{}
            << "region"
            << "us-east-1"
            << "key"
            << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
            << "endpoint"
            << "doesnotexist.invalid" << finalize;
        _run_endpoint_test(&setup_client,
                           parse_error_masterkey.view(),
                           "aws",
                           {{"Failed to resolve doesnotexist.invalid: generic server error"}});
    }

    // Call `client_encryption.createDataKey()` with "azure" as the provider and the following
    // masterKey:
    // {
    //     "keyVaultEndpoint": "key-vault-csfle.vault.azure.net",
    //     "keyName": "key-name-csfle"
    // }
    // Expect this to succeed. Use the returned UUID of the key to explicitly encrypt and decrypt
    // the string "test" to validate it works. Call ``client_encryption_invalid.createDataKey()``
    // with the same masterKey.
    // Expect this to fail with a network exception indicating failure to resolve
    // "doesnotexist.invalid".
    SECTION("Test Case 7") {
        auto azure_masterkey = document{} << "keyVaultEndpoint"
                                          << "key-vault-csfle.vault.azure.net"
                                          << "keyName"
                                          << "key-name-csfle" << finalize;
        _run_endpoint_test(&setup_client,
                           azure_masterkey.view(),
                           "azure",
                           stdx::nullopt,
                           {{"Failed to resolve doesnotexist.invalid: generic server error"}});
    }

    // Call `client_encryption.createDataKey()` with "gcp" as the provider and the following
    // masterKey:
    // {
    //   "projectId": "devprod-drivers",
    //   "location": "global",
    //   "keyRing": "key-ring-csfle",
    //   "keyName": "key-name-csfle",
    //   "endpoint": "cloudkms.googleapis.com:443"
    // }
    // Expect this to succeed. Use the returned UUID of the key to explicitly encrypt and decrypt
    // the string "test" to validate it works. Call ``client_encryption_invalid.createDataKey()``
    // with the same masterKey.
    // Expect this to fail with a network exception indicating failure to resolve
    // "doesnotexist.invalid".
    SECTION("Test Case 8") {
        auto gcp_masterkey = document{} << "projectId"
                                        << "devprod-drivers"
                                        << "location"
                                        << "global"
                                        << "keyRing"
                                        << "key-ring-csfle"
                                        << "keyName"
                                        << "key-name-csfle"
                                        << "endpoint"
                                        << "cloudkms.googleapis.com:443" << finalize;
        _run_endpoint_test(&setup_client,
                           gcp_masterkey.view(),
                           "gcp",
                           stdx::nullopt,
                           {{"Failed to resolve doesnotexist.invalid: generic server error"}});
    }

    // Call `client_encryption.createDataKey()` with "gcp" as the provider and the following
    // masterKey:
    // {
    //   "projectId": "devprod-drivers",
    //   "location": "global",
    //   "keyRing": "key-ring-csfle",
    //   "keyName": "key-name-csfle",
    //   "endpoint": "doesnotexist.invalid:443"
    // }
    // Expect this to fail with an exception with a message containing the string: "Invalid KMS
    // response".
    SECTION("Test Case 9") {
        auto gcp_masterkey2 = document{} << "projectId"
                                         << "devprod-drivers"
                                         << "location"
                                         << "global"
                                         << "keyRing"
                                         << "key-ring-csfle"
                                         << "keyName"
                                         << "key-name-csfle"
                                         << "endpoint"
                                         << "doesnotexist.invalid:443" << finalize;
        _run_endpoint_test(&setup_client, gcp_masterkey2.view(), "gcp", {{"Invalid KMS response"}});
    }

    // Call `client_encryption.createDataKey()` with "kmip" as the provider and the following
    // masterKey:
    // {
    //   "keyId": "1"
    // }
    // Expect this to succeed. Use the returned UUID of the key to explicitly encrypt and decrypt
    // the string "test" to validate it works. Call client_encryption_invalid.createDataKey() with
    // the same masterKey. Expect this to fail with a network exception indicating failure to
    // resolve "doesnotexist.local".
    SECTION("Test Case 10") {
        auto kmip_masterkey = document{} << "keyId"
                                         << "1" << finalize;
        _run_endpoint_test(&setup_client,
                           kmip_masterkey.view(),
                           "kmip",
                           stdx::nullopt,
                           {{"Failed to resolve doesnotexist.local: generic server error"}});
    }

    // Call `client_encryption.createDataKey()` with "kmip" as the provider and the following
    // masterKey:
    // {
    //   "keyId": "1",
    //   "endpoint": "localhost:5698"
    // }
    // Expect this to succeed. Use the returned UUID of the key to explicitly encrypt and decrypt
    // the string "test" to validate it works.
    SECTION("Test Case 11") {
        auto kmip_masterkey = document{} << "keyId"
                                         << "1"
                                         << "endpoint"
                                         << "localhost:5698" << finalize;
        _run_endpoint_test(&setup_client, kmip_masterkey.view(), "kmip");
    }

    // Call `client_encryption.createDataKey()` with "kmip" as the provider and the following
    // masterKey:
    // {
    //   "keyId": "1",
    //   "endpoint": "doesnotexist.local:5698"
    // }
    // Expect this to fail with a network exception indicating failure to resolve
    // "doesnotexist.local".
    SECTION("Test Case 12") {
        auto kmip_masterkey = document{} << "keyId"
                                         << "1"
                                         << "endpoint"
                                         << "doesnotexist.local:5698" << finalize;
        _run_endpoint_test(&setup_client,
                           kmip_masterkey.view(),
                           "kmip",
                           {{"Failed to resolve doesnotexist.local: generic server error"}});
    }
}

TEST_CASE("Bypass spawning mongocryptd", "[client_side_encryption]") {
    instance::current();

    class client setup_client {
        uri{}, test_util::add_test_server_api(),
    };
    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    // Via mongocryptdBypassSpawn

    auto external_schema_file = _doc_from_file("/external/external-schema.json");
    auto external_schema = document{} << "db.coll" << external_schema_file << finalize;

    // Create a MongoClient configured with auto encryption (referred to as client_encrypted)
    //
    // Configure the required options. Use the local KMS provider as follows:
    //     { "local": { "key": <base64 decoding of LOCAL_MASTERKEY> } }
    // Configure with the keyVaultNamespace set to keyvault.datakeys.
    //
    // Configure client_encrypted to use the schema external/external-schema.json for db.coll by
    // setting a schema map like: { "db.coll": <contents of external-schema.json>}
    options::client client_encrypted_opts;

    options::auto_encryption auto_encrypt_opts{};
    auto_encrypt_opts.kms_providers(_make_kms_doc());
    auto_encrypt_opts.tls_opts(_make_tls_opts());
    auto_encrypt_opts.key_vault_namespace({"keyvault", "datakeys"});
    auto_encrypt_opts.schema_map({external_schema.view()});

    // Configure the following extraOptions:
    // {
    //   "mongocryptdBypassSpawn": true
    //   "mongocryptdURI": "mongodb://localhost:27021/db?serverSelectionTimeoutMS=1000",
    //   "mongocryptdSpawnArgs": [ "--pidfilepath=bypass-spawning-mongocryptd.pid", "--port=27021"]
    // }
    auto extra = document{} << "mongocryptdBypassSpawn" << true << "mongocryptdURI"
                            << "mongodb://localhost:27021/db?serverSelectionTimeoutMS=1000"
                            << "mongocryptdSpawnArgs" << open_array
                            << "--pidfilepath=bypass-spawning-mongocryptd.pid"
                            << "--port=27021" << close_array << finalize;

    auto_encrypt_opts.extra_options({extra.view()});
    client_encrypted_opts.auto_encryption_opts(std::move(auto_encrypt_opts));

    class client client_encrypted {
        uri{}, test_util::add_test_server_api(client_encrypted_opts)
    };

    // Use client_encrypted to insert the document {"encrypted": "test"} into db.coll.
    // Expect a server selection error propagated from the internal MongoClient failing to
    // connect to mongocryptd on port 27021.
    auto coll = client_encrypted["db"]["coll"];
    REQUIRE_THROWS(coll.insert_one(make_document(kvp("encrypted", "test"))));

    // Via bypassAutoEncryption

    // Create a MongoClient configured with auto encryption (referred to as client_encrypted)
    //
    // Configure the required options. Use the local KMS provider as follows:
    //
    // { "local": { "key": <base64 decoding of LOCAL_MASTERKEY> } }
    // Configure with the keyVaultNamespace set to keyvault.datakeys.
    options::client client_encrypted_opts2;
    options::auto_encryption auto_encrypt_opts2{};
    auto_encrypt_opts2.kms_providers(_make_kms_doc());
    auto_encrypt_opts2.tls_opts(_make_tls_opts());
    auto_encrypt_opts2.key_vault_namespace({"keyvault", "datakeys"});
    auto_encrypt_opts2.schema_map({external_schema.view()});

    // Configure with bypassAutoEncryption=true.
    auto_encrypt_opts2.bypass_auto_encryption(true);

    // Configure the following extraOptions:
    // {
    //   "mongocryptdSpawnArgs": [ "--pidfilepath=bypass-spawning-mongocryptd.pid", "--port=27021"]
    // }
    auto extra2 = document{} << "mongocryptdSpawnArgs" << open_array
                             << "--pidfilepath=bypass-spawning-mongocryptd.pid"
                             << "--port=27021" << close_array << finalize;

    auto_encrypt_opts2.extra_options({extra2.view()});
    client_encrypted_opts2.auto_encryption_opts(std::move(auto_encrypt_opts2));

    class client client_encrypted2 {
        uri{}, test_util::add_test_server_api(client_encrypted_opts2)
    };

    // Use client_encrypted to insert the document {"unencrypted": "test"} into db.coll.
    // Expect this to succeed.
    auto coll2 = client_encrypted2["db"]["coll"];
    coll2.insert_one(make_document(kvp("unencrypted", "test")));

    // Validate that mongocryptd was not spawned. Create a MongoClient to localhost:27021
    // (or whatever was passed via --port) with serverSelectionTimeoutMS=1000. Run a ping
    // command and ensure it fails with a server selection timeout.
    options::client ping_client_opts;

    class client ping_client {
        uri{"mongodb://localhost:27021/?serverSelectionTimeoutMS=1000"},
            test_util::add_test_server_api(),
    };
    REQUIRE_THROWS(ping_client["admin"].run_command(make_document(kvp("ping", 1))));
}

class kms_tls_expired_cert_matcher : public Catch::MatcherBase<mongocxx::exception> {
   public:
    bool match(const mongocxx::exception& exc) const override {
        return (Catch::Contains("certificate has expired") ||  // OpenSSL
                Catch::Contains("CSSMERR_TP_CERT_EXPIRED") ||  // Secure Transport
                Catch::Contains("certificate has expired") ||  // Secure Channel
                Catch::Contains("certificate has expired"))    // LibreSSL
            .match(exc.what());
    }

    std::string describe() const override {
        return "message should reference an expired certificate";
    }
};

TEST_CASE("KMS TLS expired certificate", "[client_side_encryption]") {
    instance::current();

    // Create a mongoclient without encryption.
    options::client client_opts;

    class client setup_client {
        uri{}, test_util::add_test_server_api(client_opts),
    };

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    // Support for detailed certificate verify failure messages required by this test are only
    // available in libmongoc 1.20.0 and newer (CDRIVER-3927).
    if (!mongoc_check_version(1, 20, 0)) {
        WARN("Skipping - libmongoc version is < 1.20.0 (CDRIVER-3927)");
        return;
    }

    // Required CA certificates may not be registered on system. See BUILD-14068.
    if (std::getenv("MONGOCXX_TEST_SKIP_KMS_TLS_TESTS")) {
        WARN("Skipping - KMS TLS tests disabled (BUILD-14068)");
        return;
    }

    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    options::client_encryption cse_opts;
    _add_cse_opts(&cse_opts, &setup_client);
    client_encryption client_encryption{std::move(cse_opts)};
    options::data_key data_key_opts;

    auto doc = make_document(
        kvp("region", "us-east-1"),
        kvp("key", "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"),
        kvp("endpoint", "127.0.0.1:9000"));
    data_key_opts.master_key(doc.view());

    REQUIRE_THROWS_MATCHES(client_encryption.create_data_key("aws", data_key_opts),
                           mongocxx::exception,
                           kms_tls_expired_cert_matcher());
}

class kms_tls_wrong_host_cert_matcher : public Catch::MatcherBase<mongocxx::exception> {
   public:
    bool match(const mongocxx::exception& exc) const override {
        return (Catch::Contains("IP address mismatch") ||                 // OpenSSL
                Catch::Contains("Host name mismatch") ||                  // Secure Transport
                Catch::Contains("hostname doesn't match certificate") ||  // Secure Channel
                Catch::Contains("not present in server certificate"))     // LibreSSL
            .match(exc.what());
    }

    std::string describe() const override {
        return "message should reference an incorrect or unexpected host";
    }
};

TEST_CASE("KMS TLS wrong host certificate", "[client_side_encryption]") {
    instance::current();

    // Create a mongoclient without encryption.
    options::client client_opts;

    class client setup_client {
        uri{}, test_util::add_test_server_api(client_opts),
    };

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    // Support for detailed certificate verify failure messages required by this test are only
    // available in libmongoc 1.20.0 and newer (CDRIVER-3927).
    if (!mongoc_check_version(1, 20, 0)) {
        WARN("Skipping - libmongoc version is < 1.20.0 (CDRIVER-3927)");
        return;
    }

    // Required CA certificates may not be registered on system. See BUILD-14068.
    if (std::getenv("MONGOCXX_TEST_SKIP_KMS_TLS_TESTS")) {
        WARN("Skipping - KMS TLS tests disabled (BUILD-14068)");
        return;
    }

    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    options::client_encryption cse_opts;
    _add_cse_opts(&cse_opts, &setup_client);
    client_encryption client_encryption{std::move(cse_opts)};
    options::data_key data_key_opts;

    auto doc = make_document(
        kvp("region", "us-east-1"),
        kvp("key", "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"),
        kvp("endpoint", "127.0.0.1:9001"));
    data_key_opts.master_key(doc.view());

    REQUIRE_THROWS_MATCHES(client_encryption.create_data_key("aws", data_key_opts),
                           mongocxx::exception,
                           kms_tls_wrong_host_cert_matcher());
}

bsoncxx::document::value make_kms_providers_with_custom_endpoints(stdx::string_view azure,
                                                                  stdx::string_view gcp,
                                                                  stdx::string_view kmip) {
    bsoncxx::builder::basic::document kms_doc;

    kms_doc.append(kvp("aws", [&](sub_document subdoc) {
        subdoc.append(kvp("secretAccessKey",
                          test_util::getenv_or_fail("MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY")));
        subdoc.append(
            kvp("accessKeyId", test_util::getenv_or_fail("MONGOCXX_TEST_AWS_ACCESS_KEY_ID")));
    }));

    kms_doc.append(kvp("azure", [&](sub_document subdoc) {
        subdoc.append(kvp("tenantId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_TENANT_ID")));
        subdoc.append(kvp("clientId", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_ID")));
        subdoc.append(
            kvp("clientSecret", test_util::getenv_or_fail("MONGOCXX_TEST_AZURE_CLIENT_SECRET")));
        subdoc.append(kvp("identityPlatformEndpoint", azure));
    }));

    kms_doc.append(kvp("gcp", [&](sub_document subdoc) {
        subdoc.append(kvp("email", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_EMAIL")));
        subdoc.append(kvp("privateKey", test_util::getenv_or_fail("MONGOCXX_TEST_GCP_PRIVATEKEY")));
        subdoc.append(kvp("endpoint", gcp));
    }));

    kms_doc.append(kvp("kmip", [&](sub_document subdoc) { subdoc.append(kvp("endpoint", kmip)); }));

    return kms_doc.extract();
}

enum struct with_certs { none, ca_only, cert_only, both };

bsoncxx::document::value make_tls_opts_with_certs(with_certs with) {
    bsoncxx::builder::basic::document tls_opts;

    stdx::string_view providers[] = {"aws", "azure", "gcp", "kmip"};

    for (const auto& provider : providers) {
        tls_opts.append(kvp(provider, [&](sub_document subdoc) {
            if (with == with_certs::ca_only || with == with_certs::both) {
                subdoc.append(
                    kvp("tlsCAFile", test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CA_FILE")));
            }

            if (with == with_certs::cert_only || with == with_certs::both) {
                subdoc.append(
                    kvp("tlsCertificateKeyFile",
                        test_util::getenv_or_fail("MONGOCXX_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE")));
            }
        }));
    }

    return tls_opts.extract();
}

client_encryption make_prose_test_11_ce(mongocxx::client* client,
                                        stdx::string_view azure,
                                        stdx::string_view gcp,
                                        stdx::string_view kmip,
                                        with_certs with) {
    options::client_encryption cse_opts;
    cse_opts.key_vault_client(client);
    cse_opts.key_vault_namespace({"keyvault", "datakeys"});
    cse_opts.kms_providers(make_kms_providers_with_custom_endpoints(azure, gcp, kmip));
    cse_opts.tls_opts(make_tls_opts_with_certs(with));
    return client_encryption(std::move(cse_opts));
}

// CDRIVER-4181: may fail due to unexpected invalid hostname errors if C Driver was built with VS
// 2015 and uses Secure Channel (ENABLE_SSL=WINDOWS).
TEST_CASE("KMS TLS Options Tests", "[client_side_encryption][!mayfail]") {
    instance::current();

    auto setup_client = client(uri(), test_util::add_test_server_api());

    if (!mongocxx::test_util::should_run_client_side_encryption_test()) {
        return;
    }

    // Support for detailed certificate verify failure messages required by this test are only
    // available in libmongoc 1.20.0 and newer (CDRIVER-3927).
    if (!mongoc_check_version(1, 20, 0)) {
        WARN("Skipping - libmongoc version is < 1.20.0 (CDRIVER-3927)");
        return;
    }

    // Required CA certificates may not be registered on system. See BUILD-14068.
    if (std::getenv("MONGOCXX_TEST_SKIP_KMS_TLS_TESTS")) {
        WARN("Skipping - KMS TLS tests disabled (BUILD-14068)");
        return;
    }

    if (test_util::get_max_wire_version(setup_client) < 8) {
        // Automatic encryption requires wire version 8.
        WARN("Skipping - max wire version is < 8");
        return;
    }

    auto client_encryption_no_client_cert = make_prose_test_11_ce(
        &setup_client, "127.0.0.1:9002", "127.0.0.1:9002", "127.0.0.1:5698", with_certs::ca_only);
    auto client_encryption_with_tls = make_prose_test_11_ce(
        &setup_client, "127.0.0.1:9002", "127.0.0.1:9002", "127.0.0.1:5698", with_certs::both);
    auto client_encryption_expired = make_prose_test_11_ce(
        &setup_client, "127.0.0.1:9000", "127.0.0.1:9000", "127.0.0.1:9000", with_certs::ca_only);
    auto client_encryption_invalid_hostname = make_prose_test_11_ce(
        &setup_client, "127.0.0.1:9001", "127.0.0.1:9001", "127.0.0.1:9001", with_certs::ca_only);

    const auto expired_cert_matcher = Catch::Contains("expired", Catch::CaseSensitive::No);
    const auto invalid_hostname_matcher = Catch::Matches(
        // Content of error message may vary depending on the SSL library being used.
        ".*(mismatch|doesn't match|not present).*",
        Catch::CaseSensitive::No);

    SECTION("Case 1 - AWS") {
        // Expect an error indicating TLS handshake failed.
        // Note: The remote server may disconnect during the TLS handshake, causing miscellaneous
        // errors instead of a neat handshake failure. Just assert that *an* error occurred.
        CHECK_THROWS_AS(
            client_encryption_no_client_cert.create_data_key(
                "aws",
                options::data_key().master_key(
                    document()
                    << "region"
                    << "us-east-1"
                    << "key"
                    << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
                    << "endpoint"
                    << "127.0.0.1:9002" << finalize)),
            mongocxx::exception);

        // Expect an error from libmongocrypt with a message containing the string: "parse error".
        // This implies TLS handshake succeeded.
        CHECK_THROWS_WITH(
            client_encryption_with_tls.create_data_key(
                "aws",
                options::data_key().master_key(
                    document()
                    << "region"
                    << "us-east-1"
                    << "key"
                    << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
                    << "endpoint"
                    << "127.0.0.1:9002" << finalize)),
            Catch::Contains("parse error", Catch::CaseSensitive::No));

        // Expect an error indicating TLS handshake failed due to an expired certificate.
        CHECK_THROWS_WITH(
            client_encryption_with_tls.create_data_key(
                "aws",
                options::data_key().master_key(
                    document()
                    << "region"
                    << "us-east-1"
                    << "key"
                    << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
                    << "endpoint"
                    << "127.0.0.1:9000" << finalize)),
            expired_cert_matcher);

        // Expect an error indicating TLS handshake failed due to an invalid hostname.
        CHECK_THROWS_WITH(
            client_encryption_with_tls.create_data_key(
                "aws",
                options::data_key().master_key(
                    document()
                    << "region"
                    << "us-east-1"
                    << "key"
                    << "arn:aws:kms:us-east-1:579766882180:key/89fcc2c4-08b0-4bd9-9f25-e30687b580d0"
                    << "endpoint"
                    << "127.0.0.1:9001" << finalize)),
            invalid_hostname_matcher);
    }

    SECTION("Case 2 - Azure") {
        options::data_key opts;

        opts.master_key(document() << "keyVaultEndpoint"
                                   << "doesnotexist.local"
                                   << "keyName"
                                   << "foo" << finalize);

        // Expect an error indicating TLS handshake failed.
        // Note: The remote server may disconnect during the TLS handshake, causing miscellaneous
        // errors instead of a neat handshake failure. Just assert that *an* error occurred.
        CHECK_THROWS_AS(client_encryption_no_client_cert.create_data_key("azure", opts),
                        mongocxx::exception);

        // Expect an error from libmongocrypt with a message containing the string: "HTTP
        // status=404". This implies TLS handshake succeeded.
        CHECK_THROWS_WITH(client_encryption_with_tls.create_data_key("azure", opts),
                          Catch::Contains("HTTP status=404", Catch::CaseSensitive::No));

        // Expect an error indicating TLS handshake failed due to an expired certificate.
        CHECK_THROWS_WITH(client_encryption_expired.create_data_key("azure", opts),
                          expired_cert_matcher);

        // Expect an error indicating TLS handshake failed due to an invalid hostname.
        CHECK_THROWS_WITH(client_encryption_invalid_hostname.create_data_key("azure", opts),
                          invalid_hostname_matcher);
    }

    SECTION("Case 3 - GCP") {
        options::data_key opts;

        opts.master_key(document() << "projectId"
                                   << "foo"
                                   << "location"
                                   << "bar"
                                   << "keyRing"
                                   << "baz"
                                   << "keyName"
                                   << "foo" << finalize);

        // Expect an error indicating TLS handshake failed.
        // Note: The remote server may disconnect during the TLS handshake, causing miscellaneous
        // errors instead of a neat handshake failure. Just assert that *an* error occurred.
        CHECK_THROWS_AS(client_encryption_no_client_cert.create_data_key("gcp", opts),
                        mongocxx::exception);

        // Expect an error from libmongocrypt with a message containing the string: "HTTP
        // status=404". This implies TLS handshake succeeded.
        CHECK_THROWS_WITH(client_encryption_with_tls.create_data_key("gcp", opts),
                          Catch::Contains("HTTP status=404", Catch::CaseSensitive::No));

        // Expect an error indicating TLS handshake failed due to an expired certificate.
        CHECK_THROWS_WITH(client_encryption_expired.create_data_key("gcp", opts),
                          expired_cert_matcher);

        // Expect an error indicating TLS handshake failed due to an invalid hostname.
        CHECK_THROWS_WITH(client_encryption_invalid_hostname.create_data_key("gcp", opts),
                          invalid_hostname_matcher);
    }

    SECTION("Case 4 - KMIP") {
        options::data_key opts;

        opts.master_key({});

        // Expect an error indicating TLS handshake failed.
        // Note: The remote server may disconnect during the TLS handshake, causing miscellaneous
        // errors instead of a neat handshake failure. Just assert that *an* error occurred.
        CHECK_THROWS_AS(client_encryption_no_client_cert.create_data_key("kmip", opts),
                        mongocxx::exception);

        // Expect success.
        CHECK_NOTHROW(client_encryption_with_tls.create_data_key("kmip", opts));

        // Expect an error indicating TLS handshake failed due to an expired certificate.
        CHECK_THROWS_WITH(client_encryption_expired.create_data_key("kmip", opts),
                          expired_cert_matcher);

        // Expect an error indicating TLS handshake failed due to an invalid hostname.
        CHECK_THROWS_WITH(client_encryption_invalid_hostname.create_data_key("kmip", opts),
                          invalid_hostname_matcher);
    }
}

}  // namespace
