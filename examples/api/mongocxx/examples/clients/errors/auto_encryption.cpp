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

#include <cstdint>
#include <cstring>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/server_error_code.hpp>
#include <mongocxx/options/auto_encryption.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    // Missing keyvault namespace.
    try {
        mongocxx::client client{
            mongocxx::uri{},
            mongocxx::options::client{}.auto_encryption_opts(mongocxx::options::auto_encryption{})}; // Throws.

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::exception const& ex) {
        // CXX-834: libmongoc error code.
        EXPECT(ex.code().category() == mongocxx::server_error_category());
        EXPECT(ex.code().value() == 58); // MONGOC_ERROR_CLIENT_INVALID_ENCRYPTION_ARG
    }

    // Invalid KMS providers.
    try {
        mongocxx::client client{
            mongocxx::uri{},
            mongocxx::options::client{}.auto_encryption_opts(
                mongocxx::options::auto_encryption{}
                    .key_vault_namespace({"keyvault", "datakeys"})
                    .kms_providers(bsoncxx::from_json(R"({"invalid": 1})")))}; // Throws.

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::exception const& ex) {
        // CXX-834: libmongocrypt error code.
        EXPECT(ex.code().category() == mongocxx::server_error_category());
        EXPECT(ex.code().value() == 1); // MONGOCRYPT_GENERIC_ERROR_CODE
    }

    // Incompatible options.
    try {
        mongocxx::client client{
            mongocxx::uri{},
            mongocxx::options::client{}.auto_encryption_opts(
                mongocxx::options::auto_encryption{}.key_vault_client(nullptr).key_vault_pool(nullptr))}; // Throws.

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::exception const& ex) {
        EXPECT(ex.code() == mongocxx::error_code::k_invalid_parameter);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    using bsoncxx::builder::basic::sub_document;

    try {
        (void)mongocxx::client{
            mongocxx::uri{}, mongocxx::options::client{}.auto_encryption_opts(mongocxx::options::auto_encryption{})};

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::exception const& ex) {
        if (std::strstr(ex.what(), "ENABLE_CLIENT_SIDE_ENCRYPTION") != nullptr) {
            // Library may not be configured with TLS/SSL support enabled.
        } else {
            example();
        }
    }
}
